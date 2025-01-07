#pragma once
#include <Unknwnbase.h>
#include <combaseapi.h>
#include <iostream>
#include <DirectXMath.h>

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef DECIMAL decimal;
typedef FILE* File;

using int3 = DirectX::XMINT3;
using float2 = DirectX::XMFLOAT2;
using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;

using uint8  = uint8_t;
using int8   = int8_t;
using uint16 = uint16_t;
using int16  = int16_t;
using uint32 = uint32_t;
using int32  = int32_t;
using uint64 = uint64_t;
using int64  = int64_t;
using guid = uint64_t;
using flag = uint32;
using mask = uint32;
using constant = uint32;
using bool32 = uint32;

enum class MouseKey : int
{
	LEFT = 0,
	RIGHT,
	MIDDLE,
	MAX = 3
};

struct BitFlag
{
public:
    operator flag() const
    {
        return m_flag;
    }
    //helper functions
    constexpr void Set(flag flag) noexcept
    {
        m_flag |= 1U << flag;
    }
    constexpr void Clear(flag flag) noexcept
    {
        m_flag &= ~(1U << flag);
    }
    constexpr void Toggle(flag flag) noexcept
    {
        m_flag ^= 1U << flag;
    }
    constexpr bool32 Test(flag flag) const noexcept
    {
        return m_flag & (1U << flag);
    }

    BitFlag& operator|= (flag flag) noexcept
	{
		m_flag |= flag;
		return *this;
	}

	BitFlag& operator&= (flag flag) noexcept
	{
		m_flag &= flag;
		return *this;
	}

    BitFlag& operator^= (flag flag) noexcept
    {
        m_flag ^= flag;
		return *this;
    }

private:
    flag m_flag{};
};
