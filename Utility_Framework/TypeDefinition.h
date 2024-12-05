#pragma once
#include <Unknwnbase.h>
#include <combaseapi.h>
#include <iostream>

typedef unsigned int uint32;
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned long long uint64;
typedef long long int64;
typedef DECIMAL decimal;
typedef FILE* File;

enum class MouseKey : int
{
	LEFT = 0,
	RIGHT,
	MIDDLE,
	MAX = 3
};