#ifndef _LOG_H
#define _LOG_H
#include <cstdio>
#include <iomanip>

#ifdef _WIN32
	#undef APIENTRY
	#define NOMINMAX
	#include <Windows.h>
#endif // _WIN32

#ifdef _DEBUG
namespace mblog
{
	enum class TextColor : uint8_t
	{
		Default = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN,
		Error = FOREGROUND_RED,
		Warning = FOREGROUND_GREEN | FOREGROUND_BLUE,
		Success = FOREGROUND_GREEN
	};

	template <auto _Color = TextColor::Default, class... _Args>
	void print(const char* format, _Args&&... args)
	{
		std::time_t current_time = std::time(nullptr);
		std::tm date_time {};
		localtime_s(&date_time, &current_time);

#ifdef WIN32
		HANDLE console {};
		if constexpr (_Color != TextColor::Default)
		{
			console = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(console, static_cast<uint8_t>(_Color));
		}
#endif

		char format_buffer[256];
#if 1
		std::snprintf(format_buffer, sizeof(format_buffer),
			"[%d-%.2d-%.2d %.2d:%.2d:%.2d] %s\n",
			date_time.tm_year + 1900, date_time.tm_mon + 1, date_time.tm_mday,
			date_time.tm_hour, date_time.tm_min, date_time.tm_sec, format);
		std::printf(format_buffer, args...);
#else
		auto time = std::put_time(&date_time, "%F %T");
		std::ostringstream stream;
		stream << time;

		std::snprintf(format_buffer, sizeof(format_buffer),
			format, std::forward<_Args>(args)...);

		m_log_file << "[" + stream.str() + "] " + format_buffer;
#endif
#ifdef WIN32
		if constexpr (_Color != TextColor::Default)
			SetConsoleTextAttribute(console, static_cast<uint8_t>(TextColor::Default));
#endif
	}

	static inline void print(const char* message)
	{
		mblog::print("%s", message);
	}

	template <class... _Args>
	static void alert(const char* format, _Args&&... args)
	{
		mblog::print<TextColor::Error>(format, std::forward<_Args>(args)...);
	}

	static inline void alert(const char* message)
	{
		mblog::alert("%s", message);
	}

	template <class... _Args>
	static void warning(const char* format, _Args&&... args)
	{
		mblog::print<TextColor::Warning>(format, std::forward<_Args>(args)...);
	}
	static inline void warning(const char* message)
	{
		mblog::warning("%s", message);
	}

	template <class... _Args>
	static void success(const char* format, _Args&&... args)
	{
		mblog::print<TextColor::Success>(format, std::forward<_Args>(args)...);
	}
	static inline void success(const char* message)
	{
		mblog::success("%s", message);
	}
}
#endif // _DEBUG

#ifdef _DEBUG

	#define log_print(message, ...)				\
		mblog::print(message, __VA_ARGS__);		\

	#define log_warning(message, ...)			\
		mblog::warning(message, __VA_ARGS__)	\

	#define log_alert(message, ...)				\
		mblog::alert(message, __VA_ARGS__)		\

	#define log_success(message, ...)			\
		mblog::success(message, __VA_ARGS__)	\

#else

	#define log_print(message, ...)	void(0)
	#define log_warning(message, ...) void(0)
	#define log_alert(message, ...) void(0)
	#define log_success(message, ...) void(0)

#endif // _DEBUG

#endif // !_LOG_H
