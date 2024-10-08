#pragma once

#include <list>
#include <memory>

class ResourceManager;
class UniqueAny;
class ResourceHandle;

/**
 * Base class for any resource type
 */
class Resource
{
public:
	using Ptr = std::unique_ptr<Resource>;

	enum class Thread
	{
		Any,
		Loading,
		Render,
		None
	};

	using InitFn = UniqueAny (*)(UniqueAny&&, ResourceManager&, ResourceHandle);

	struct InitStep
	{
		Thread thread;
		InitFn init;
	};

	struct DeinitStep
	{
		Thread thread;
		InitFn deinit;
	};

	using InitSteps = std::list<InitStep>;
	using DeinitSteps = std::list<DeinitStep>;

public:
	Resource() = default;
	virtual ~Resource() = default;

	// prohibit copying and moving so we can safely store raw references
	// to the resource as long as the resource is loaded
	Resource(const Resource&) = delete;
	Resource& operator=(Resource&) = delete;
	Resource(Resource&&) = delete;
	Resource& operator=(Resource&&) = delete;

	virtual bool isValid() const = 0;

	static InitSteps GetInitSteps() { return {}; }
	virtual DeinitSteps getDeinitSteps() const { return {}; };

	static Thread GetCreateThread() { return Thread::Any; }
};
