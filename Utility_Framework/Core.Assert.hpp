#pragma once
#include <cstdlib>

#define DEFINE__ASSERT(expression, ...) \
	if(!(expression)) \
	{ \
		 __debugbreak(); \
		std::abort(); \
	} \
	else {}

#ifdef NDEBUG
#define CORE_ASSERT(...)
#else
#define CORE_ASSERT(expression, ...) DEFINE__ASSERT(expression, __VA_ARGS__);
#endif
