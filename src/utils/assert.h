#ifndef _ASSERT_H
#define _ASSERT_H
#include <cstdio>
#include <memory>

// Shut up MSVC C4005
#undef assert

#ifdef _WIN32
	#define NOMINMAX // who the fuck did that?
	#include <Windows.h>
#endif // _WIN32

template <class... _Args>
static void __assert_impl__(const char* format, _Args&&... args)
{
#if defined (_WIN32) && !defined(_DEBUG)
	char full_error_message[1024];
	_snprintf_s(full_error_message, sizeof(full_error_message), format, std::forward<_Args>(args)...);
	MessageBoxA(nullptr, full_error_message, "Exception!", MB_OK | MB_ICONERROR);
#else
	std::printf(format, std::forward<_Args>(args)...);
#endif // _WIN32
	exit(EXIT_FAILURE);
}

#define __validate_condition__(condition)			\
	if (!bool(condition))							\

#define core_assert(condition, format, ...)			\
	__validate_condition__(condition)				\
	__assert_impl__(format, __VA_ARGS__);	

#define core_assert_immediatly(format, ...)			\
	__assert_impl__(format, __VA_ARGS__);

#ifdef _DEBUG

	#define assert(condition, format, ...)			\
		core_assert(condition, format, __VA_ARGS__)	\

#else

	#define assert(condition, format, ...) void(0)	\

#endif // _DEBUG

#endif // !_ASSERT_H
