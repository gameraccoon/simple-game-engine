#pragma once

#define COMPILER_UNKNOWN 0
#define COMPILER_GCC 1
#define COMPILER_CLANG 2
#define COMPILER_MSVC 3

// clang-format off

// define USED_COMPILER macro
#if defined(__clang__)
	#define USED_COMPILER COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
	#define USED_COMPILER COMPILER_GCC
#elif defined(_MSC_VER)
	#define USED_COMPILER COMPILER_MSVC
#else
	// Other compilers
	#define USED_COMPILER COMPILER_UNKNOWN
#endif

// MAYBE_UNUSED (for suppressing 'variable unused' warning)
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(maybe_unused)
		#define MAYBE_UNUSED [[maybe_unused]]
	#endif
#endif
#ifndef MAYBE_UNUSED
	#ifdef WIN32
		#define MAYBE_UNUSED
	#else
		#define MAYBE_UNUSED __attribute__((unused))
	#endif
#endif

// CONSTEXPR_ASSERT (to fail compilation when the condition is not passed in a constexpr function)
#define CONSTEXPR_ASSERT(cond, msg) ((cond) ? void() : std::abort())

// ASSUME (to give a compiler hint that the condition is true at the point where this macro appears)
// Note: this is dangerous and may result in undifined behaviour if used not carefully
#if USED_COMPILER == COMPILER_CLANG
	#define ASSUME(cond) __builtin_assume((cond))
#elif USED_COMPILER == COMPILER_GCC
	#define ASSUME(cond) __builtin_expect((cond), 1)
#elif USED_COMPILER == COMPILER_MSVC
	#define ASSUME(cond) __assume((cond))
#else
	#define ASSUME(cond)
#endif

// disable clang diagnostics locally
#if USED_COMPILER == COMPILER_CLANG
	#define DISABLE_DIAG_PUSH(diagnostic_name) \
		_Pragma("clang diagnostic push") \
		_Pragma("clang diagnostic ignored \"diagnostic_name\"")

	#define DISABLE_DIAG_POP _Pragma("clang diagnostic pop")
#else
	#define DISABLE_DIAG_PUSH(diagnostic)
	#define DISABLE_DIAG_POP
#endif

// clang-format on
