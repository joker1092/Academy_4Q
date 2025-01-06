#pragma once
#include <cstdio>
#include <print>

//Console Ŭ���� -> ������ C#�� Console Ŭ������ ����
class CoreConsole
{
public:
	template<typename... T>
	void Write(std::format_string<T...> fmt, T&&... args)
	{
        std::print(fmt, static_cast<T&&>(args)...);
	}

	template<typename... T>
	void Write(FILE* file, std::format_string<T...> fmt, T&&... args)
	{
        std::print(file, fmt, static_cast<T&&>(args)...);
	}

	template<typename... T>
	void WriteLine(FILE* file, std::format_string<T...> fmt, T&&... args)
	{
        std::println(file, fmt, static_cast<T&&>(args)...);
	}

	template<typename... T>
	void WriteLine(std::format_string<T...> fmt, T&&... args)
	{
		return WriteLine(stdout, fmt, static_cast<T&&>(args)...);
	}

};

inline static CoreConsole Console;
