#pragma once

struct LogCategory
{
	const char* name;

	explicit consteval LogCategory(const char* name)
		: name(name)
	{}

	~LogCategory() noexcept = default;
	LogCategory(const LogCategory&) noexcept = default;
	LogCategory& operator=(const LogCategory&) noexcept = default;
	LogCategory(LogCategory&&) noexcept = default;
	LogCategory& operator=(LogCategory&&) noexcept = default;
};
