#pragma once

#include <array>
#include <optional>
#include <span>

struct SteamNetworkingIPAddr;

namespace HAL
{
	namespace Network
	{
		class NetworkAddress
		{
		public:
			NetworkAddress(const NetworkAddress& other);
			NetworkAddress& operator=(const NetworkAddress& other);
			NetworkAddress(NetworkAddress&&) = default;
			NetworkAddress& operator=(NetworkAddress&&) = default;
			~NetworkAddress();

			static std::optional<NetworkAddress> FromString(const std::string& str);
			static NetworkAddress Ipv4(std::array<u8, 4> addr, u16 port);
			static NetworkAddress Ipv6(std::array<u8, 16> addr, u16 port);

#ifndef FAKE_NETWORK
			const SteamNetworkingIPAddr& getInternalAddress() const;
#else
			bool isLocalhost() const;
			u16 getPort() const;
#endif // FAKE_NETWORK

		private:
			struct Impl;

		private:
			explicit NetworkAddress(std::unique_ptr<Impl>&& pimpl);

		private:
			std::unique_ptr<Impl> mPimpl;
		};

		class Message
		{
		public:
			Message() = default;
			explicit Message(u32 type);
			// copies the data
			explicit Message(std::byte* rawData, size_t rawDataSize);
			// copies the payload
			explicit Message(u32 type, const std::vector<std::byte>& payload);

			// reading the message
			u32 readMessageType() const;
			std::span<const std::byte> getPayloadRef() const;

			// populating the message
			void resizePayload(size_t payloadSize);
			void reservePayload(size_t payloadSize);
			void setMessageType(u32 type);
			std::vector<std::byte>& getDataMutRef() { return data; }

			// serialization
			size_t getDataSize() const;
			std::span<const std::byte> getDataRef() const;

		private:
			static constexpr size_t headerSize = sizeof(u32);

			// data stores some meta information + payload and also can have extra unused space
			// use cursorPos to determine data end (always set when message received from ConnectionManager)
			//                  /cursorPos
			// [header][payload][preallocated]
			std::vector<std::byte> data;

			// cursorPos will be used to determine the size of the data that we want to send
			// if cursorPos == 0, then we send all the data
			// if cursorPos == payloadStartPos or cursorPos == headerSize then only header with the empty data will be sent
			// otherwise the data before cursorPos is sent
			size_t cursorPos = 0;
		};

		struct DebugBehavior
		{
			float packetLossPct_Send = 0.0f;
			float packetLossPct_Recv = 0.0f;
			int packetLagMs_Send = 0;
			int packetLagMs_Recv = 0;
			float packetReorderPct_Send = 0.0f;
			float packetReorderPct_Recv = 0.0f;
			int packetReorder_TimeMs = 15;
			float packetDupPct_Send = 0.0f;
			float packetDupPct_Recv = 0.0f;
			int packetDup_TimeMaxMs = 10;
			int rateLimitBps_Send = 0; // bytes per second
			int rateLimitBps_Recv = 0; // bytes per second
			int rateLimitOneBurstBytes_Send = 16 * 1024;
			int rateLimitOneBurstBytes_Recv = 16 * 1024;
		};
	} // namespace Network
} // namespace HAL
