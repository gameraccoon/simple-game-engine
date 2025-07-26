#include "EngineCommon/precomp.h"

#ifdef FAKE_NETWORK

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "HAL/Network/Debug/FakeNetwork.h"

namespace HAL
{
	struct FakeNetworkManager::Impl
	{
		using ReceivedMessagesVector = std::vector<std::pair<ConnectionId, Network::Message>>;

		struct OpenPortData
		{
			std::unordered_set<ConnectionId> openConnections;
			ReceivedMessagesVector receivedMessages;
		};

		struct DelayedMessage
		{
			Network::Message message;
			std::chrono::system_clock::time_point deliveryTime;
		};

		static inline std::unordered_map<u16, std::unique_ptr<OpenPortData>> openPorts;
		static inline std::unordered_map<ConnectionId, ConnectionId> openConnections;
		static inline std::unordered_map<ConnectionId, u16> portByClientConnection;
		static inline ReceivedMessagesVector receivedClientMessages;
		static inline ConnectionId nextConnectionId = 0;
		static inline std::optional<std::chrono::system_clock::time_point> fakeTime;
		static inline std::mutex dataMutex;
		static inline bool pretendConnected = false;

		static inline std::unordered_map<ConnectionId, std::vector<DelayedMessage>> messagesOnTheWay;
		static inline std::chrono::system_clock::duration messageDelay = std::chrono::milliseconds(0);

		static void removeMessagesForConnection(std::vector<std::pair<ConnectionId, Network::Message>>& messages, ConnectionId connection)
		{
			messages.erase(
				std::remove_if(
					messages.begin(),
					messages.end(),
					[connection](const std::pair<ConnectionId, Network::Message>& messagePair) {
						return messagePair.first == connection;
					}
				),
				messages.end()
			);
		}

		ConnectionId addServerConnectionAndReturnClientConnectionId(u16 port)
		{
			if (pretendConnected)
			{
				const ConnectionId clientConnection = nextConnectionId++;
				const ConnectionId serverConnection = nextConnectionId++;

				openConnections.emplace(clientConnection, serverConnection);
				portByClientConnection.emplace(clientConnection, port);
				return clientConnection;
			}

			if (const auto it = openPorts.find(port); it != openPorts.end())
			{
				const ConnectionId clientConnection = nextConnectionId++;
				const ConnectionId serverConnection = nextConnectionId++;

				openConnections.emplace(clientConnection, serverConnection);
				openConnections.emplace(serverConnection, clientConnection);
				it->second->openConnections.emplace(serverConnection);
				portByClientConnection.emplace(clientConnection, port);
				return clientConnection;
			}
			return InvalidConnectionId;
		}

		void scheduleMessage(const ConnectionId connectionId, const Network::Message& message)
		{
			if (pretendConnected)
			{
				return;
			}

			std::vector<DelayedMessage>& delayedMessages = messagesOnTheWay[connectionId];
			std::chrono::system_clock::time_point deliveryTime = getTime() + messageDelay;
			const auto it = std::upper_bound(delayedMessages.begin(), delayedMessages.end(), deliveryTime, [](const std::chrono::system_clock::time_point deliveryTime, const DelayedMessage& message) {
				return deliveryTime < message.deliveryTime;
			});

			delayedMessages.emplace(it, std::move(message), deliveryTime);
		}

		ReceivedMessagesVector& GetMessageVectorRefFromSenderConnectionId(const ConnectionId sendingSideConnectionId)
		{
			if (const auto portIt = portByClientConnection.find(sendingSideConnectionId); portIt != portByClientConnection.end())
			{
				// client-to-server
				return openPorts[portIt->second]->receivedMessages;
			}
			else
			{
				// server-to-client
				return receivedClientMessages;
			}
		}

		void receiveScheduledMessages(ConnectionId receivingConnectionId)
		{
			if (pretendConnected)
			{
				return;
			}

			const auto connectionIt = openConnections.find(receivingConnectionId);
			if (connectionIt == openConnections.end())
			{
				// connection that we try to access is closed
				ReportError("Connection %u is closed", receivingConnectionId);
				return;
			}
			const ConnectionId sendingSideConnectionId = connectionIt->second;

			std::vector<DelayedMessage>& delayedMessages = messagesOnTheWay[sendingSideConnectionId];
			const std::chrono::system_clock::time_point timeNow = getTime();
			const auto receivedMessagesEnd = std::upper_bound(delayedMessages.begin(), delayedMessages.end(), timeNow, [](const std::chrono::system_clock::time_point timeNow, const DelayedMessage& message) {
				return timeNow < message.deliveryTime;
			});

			for (auto it = delayedMessages.begin(); it != receivedMessagesEnd; ++it)
			{
				GetMessageVectorRefFromSenderConnectionId(sendingSideConnectionId).emplace_back(receivingConnectionId, std::move(it->message));
			}

			delayedMessages.erase(delayedMessages.begin(), receivedMessagesEnd);
		}

		std::chrono::system_clock::time_point getTime() const
		{
			if (fakeTime.has_value())
			{
				return fakeTime.value();
			}
			return std::chrono::system_clock::now();
		}
	};

	FakeNetworkManager::FakeNetworkManager()
		: mPimpl(HS_NEW Impl())
	{
	}

	FakeNetworkManager::~FakeNetworkManager() = default;

	FakeNetworkManager::OpenPortResult FakeNetworkManager::startListeningToPort(u16 port)
	{
		std::lock_guard l(mPimpl->dataMutex);
		if (mPimpl->openPorts.contains(port))
		{
			return OpenPortResult{ OpenPortResult::Status::AlreadyOpened };
		}

		mPimpl->openPorts.emplace(port, std::make_unique<Impl::OpenPortData>());

		return OpenPortResult{ OpenPortResult::Status::Success };
	}

	bool FakeNetworkManager::isPortOpen(const u16 port) const
	{
		std::lock_guard l(mPimpl->dataMutex);
		return mPimpl->openPorts.contains(port);
	}

	void FakeNetworkManager::stopListeningToPort(const u16 port)
	{
		std::lock_guard l(mPimpl->dataMutex);
		const auto portDataIt = mPimpl->openPorts.find(port);
		if (portDataIt == mPimpl->openPorts.end())
		{
			// the port already closed
			return;
		}

		// clear messages before closing connections to avoid iterating over them during removement
		portDataIt->second->receivedMessages.clear();

		// make a copy since original map will be modified while closing the connection
		const auto openConnectionsCopy = portDataIt->second->openConnections;
		for (const ConnectionId connection : openConnectionsCopy)
		{
			dropConnection(connection);
		}

		mPimpl->openPorts.erase(portDataIt);
	}

	FakeNetworkManager::ConnectResult FakeNetworkManager::connectToServer(Network::NetworkAddress address)
	{
		std::lock_guard l(mPimpl->dataMutex);
		if (!address.isLocalhost())
		{
			ReportFatalError("Real network connections are not supported in FAKE_NETWORK mode, use 127.0.0.1 to simulate local connection");
			return ConnectResult{ ConnectResult::Status::Failure, InvalidConnectionId };
		}

		const u16 port = address.getPort();

		const ConnectionId clientConnectionId = mPimpl->addServerConnectionAndReturnClientConnectionId(port);

		if (clientConnectionId != InvalidConnectionId)
		{
			return ConnectResult{ ConnectResult::Status::Success, clientConnectionId };
		}

		return ConnectResult{ ConnectResult::Status::Failure, InvalidConnectionId };
	}

	bool FakeNetworkManager::isConnectionOpen(const ConnectionId connection) const
	{
		std::lock_guard l(mPimpl->dataMutex);
		return mPimpl->openConnections.contains(connection);
	}

	void FakeNetworkManager::dropConnection(const ConnectionId connection)
	{
		std::lock_guard l(mPimpl->dataMutex);
		if (const auto connectionPairIt = mPimpl->openConnections.find(connection); connectionPairIt != mPimpl->openConnections.end())
		{
			ConnectionId clientSideConnectionId = InvalidConnectionId;
			ConnectionId serverSideConnectionId = InvalidConnectionId;
			u16 serverPort = 0;

			auto portIt = mPimpl->portByClientConnection.find(connection);
			if (portIt == mPimpl->portByClientConnection.end())
			{
				// server-to-client connection
				clientSideConnectionId = connectionPairIt->second;
				serverSideConnectionId = connection;

				portIt = mPimpl->portByClientConnection.find(connectionPairIt->second);
				if (portIt != mPimpl->portByClientConnection.end())
				{
					serverPort = portIt->second;
				}
				else
				{
					ReportError("Port for connection wasn't found on any of the sides: %u %u", clientSideConnectionId, serverSideConnectionId);
				}
			}
			else
			{
				// client-to-server connection
				clientSideConnectionId = connection;
				serverSideConnectionId = connectionPairIt->second;
				serverPort = portIt->second;
			}

			mPimpl->openConnections.erase(clientSideConnectionId);
			mPimpl->openConnections.erase(serverSideConnectionId);
			mPimpl->portByClientConnection.erase(clientSideConnectionId);

			if (const auto portDataIt = mPimpl->openPorts.find(serverPort); portDataIt != mPimpl->openPorts.end())
			{
				Impl::removeMessagesForConnection(portDataIt->second->receivedMessages, serverSideConnectionId);
				portDataIt->second->openConnections.erase(serverSideConnectionId);
			}
			Impl::removeMessagesForConnection(mPimpl->receivedClientMessages, clientSideConnectionId);
		}
	}

	FakeNetworkManager::SendMessageResult FakeNetworkManager::sendMessage(const ConnectionId connectionId, const Network::Message& message, const MessageReliability reliability)
	{
		std::lock_guard l(mPimpl->dataMutex);

		if (reliability == MessageReliability::Unreliable || reliability == MessageReliability::UnreliableAllowSkip)
		{
			constexpr int messageLossPercentage = 0;
			if (std::rand() % 100 < messageLossPercentage)
			{
				// imitate sending and loosing
				return SendMessageResult{ SendMessageResult::Status::Success };
			}
		}

		mPimpl->scheduleMessage(connectionId, std::move(message));
		return SendMessageResult{ SendMessageResult::Status::Success };
	}

	std::vector<std::pair<ConnectionId, Network::Message>> FakeNetworkManager::consumeReceivedMessages(const u16 port)
	{
		std::lock_guard l(mPimpl->dataMutex);
		if (const auto it = mPimpl->openPorts.find(port); it != mPimpl->openPorts.end())
		{
			// this is not effecient at all, but this is debug code
			for (const ConnectionId connectionId : it->second->openConnections)
			{
				mPimpl->receiveScheduledMessages(connectionId);
			}
			return std::move(it->second->receivedMessages);
		}

		ReportError("Port %u is closed", port);
		return {};
	}

	std::vector<std::pair<ConnectionId, Network::Message>> FakeNetworkManager::consumeReceivedClientMessages(const ConnectionId connectionId)
	{
		std::lock_guard l(mPimpl->dataMutex);
		mPimpl->receiveScheduledMessages(connectionId);
		// this works only for one client, we need separation when we simulate other clients
		return std::move(mPimpl->receivedClientMessages);
	}

	void FakeNetworkManager::setDebugDelayMilliseconds(const int milliseconds)
	{
		using namespace std::chrono_literals;
		mPimpl->messageDelay = milliseconds * 1ms;
	}

	void FakeNetworkManager::debugAdvanceTimeMilliseconds(const int milliseconds)
	{
		using namespace std::chrono_literals;
		mPimpl->fakeTime = mPimpl->getTime() + milliseconds * 1ms;
	}

	void FakeNetworkManager::setPretendConnected(const bool pretendConnected)
	{
		mPimpl->pretendConnected = pretendConnected;
	}
} // namespace HAL

#endif // FAKE_NETWORK
