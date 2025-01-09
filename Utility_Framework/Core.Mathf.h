#pragma once
#include "Core.Definition.h"

using namespace DirectX;

namespace Mathf
{
    using xMatrix = XMMATRIX;
    using xVector = XMVECTOR;
    using Color3 = SimpleMath::Vector3;
    using Color4 = SimpleMath::Color;
    using xVColor4 = XMVECTORF32;
    using Vector2 = SimpleMath::Vector2;
    using Vector3 = SimpleMath::Vector3;
    using Vector4 = SimpleMath::Vector4;
    using Matrix = SimpleMath::Matrix;
    using Quaternion = SimpleMath::Quaternion;

	static xVector xVectorZero = XMVectorZero();
	static xVector xVectorOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	static xVector xVectorUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    template<class T> inline T lerp(T low, T high, float t)
    {
        return low + static_cast<T>((high - low) * t);
    }

    template<class T> void ClampedIncrementOrDecrement(T& val, int upOrDown, int lo, int hi)
    {
        int newVal = static_cast<int>(val) + upOrDown;
        if (upOrDown > 0) newVal = newVal >= hi ? (hi - 1) : newVal;
        else              newVal = newVal < lo ? lo : newVal;
        val = static_cast<T>(newVal);
    }

    template<class T> void Clamp(T& val, const T lo, const T hi)
    {
        val = max(lo, min(hi, val));
    }

	template<class T> void Wrap(T& val, const T lo, const T hi)
	{
		if (val >= hi) val = lo;
		if (val < lo) val = hi;
	}

    template<class T> T Clamp(const T& val, const T lo, const T hi)
    {
        T _val = val;
        Clamp(_val, lo, hi);
        return _val;
    }

    constexpr float Deg2Rad = XM_PI / 180.0f;
    constexpr float Rad2Deg = 180.0f / XM_PI;
}
