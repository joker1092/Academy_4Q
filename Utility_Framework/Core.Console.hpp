#pragma once
#include "fmt/include/fmt/core.h"
#include <cstdio>

using namespace fmt;
//Console 클래스 -> 사용법은 C#의 Console 클래스와 유사
class CoreConsole
{
public:
	template<typename... T>
	void Write(format_string<T...> fmt, T&&... args)
	{
		const auto& vargs = fmt::make_format_args(args...);
		if (!detail::use_utf8()) return detail::vprint_mojibake(stdout, fmt, vargs);
		return detail::is_locking<T...>() ? vprint_buffered(stdout, fmt, vargs)
			: vprint(fmt, vargs);
	}

	template<typename... T>
	void Write(FILE* file, format_string<T...> fmt, T&&... args)
	{
		const auto& vargs = fmt::make_format_args(args...);
		if (!detail::use_utf8()) return detail::vprint_mojibake(file, fmt, vargs);
		return detail::is_locking<T...>() ? vprint_buffered(file, fmt, vargs)
			: vprint(file, fmt, vargs);
	}

	template<typename... T>
	void WriteLine(FILE* file, format_string<T...> fmt, T&&... args)
	{
		const auto& vargs = fmt::make_format_args(args...);
		return detail::use_utf8() ? vprintln(f, fmt, vargs)
			: detail::vprint_mojibake(f, fmt, vargs, true);
	}

	template<typename... T>
	void WriteLine(format_string<T...> fmt, T&&... args)
	{
		return WriteLine(stdout, fmt, static_cast<T&&>(args)...);
	}

};

inline static CoreConsole Console;