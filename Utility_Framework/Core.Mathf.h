#pragma once
#include "Core.Definition.h"

using namespace DirectX;

namespace Mathf
{
	using xMatrix = XMMATRIX;
	using xVector = XMVECTOR;

	template<class T> inline T lerp(T low, T high, float t) { return low + static_cast<T>((high - low) * t); }
	template<class T> void ClampedIncrementOrDecrement(T& val, int upOrDown, int lo, int hi)
	{
		int newVal = static_cast<int>(val) + upOrDown;
		if (upOrDown > 0) newVal = newVal >= hi ? (hi - 1) : newVal;
		else              newVal = newVal < lo ? lo : newVal;
		val = static_cast<T>(newVal);
	}

	template<class T> void Clamp(T& val, const T lo, const T hi)
	{
		val = min(val, lo);
		val = max(val, hi);
	}

	template<class T> T Clamp(const T& val, const T lo, const T hi)
	{
		T _val = val;
		Clamp(_val, lo, hi);
		return _val;
	}

	constexpr float PI = 3.14159265358979323846f;
	constexpr float Deg2Rad = PI / 180.0f;
	constexpr float Rad2Deg = 180.0f / PI;

	using VecPair = std::pair<XMVECTOR, XMVECTOR>;

	struct Vector3
	{
		static Vector3 Rand();
		static const xVector Zero;
		static const xVector Up;
		static const xVector Down;
		static const xVector Left;
		static const xVector Right;
		static const xVector Forward;
		static const xVector Back;

		static const Vector3 ZeroF3;
		static const Vector3 UpF3;
		static const Vector3 DownF3;
		static const Vector3 LeftF3;
		static const Vector3 RightF3;
		static const Vector3 ForwardF3;
		static const Vector3 BackF3;

		static const Vector3 XAxis;
		static const Vector3 YAxis;
		static const Vector3 ZAxis;

		Vector3();
		Vector3(const Vector3& v_in);
		Vector3(float, float, float);
		Vector3(float);
		Vector3(const XMFLOAT3& f3);
		Vector3(const XMVECTOR& v_in);

		operator XMVECTOR() const;
		operator XMFLOAT3() const;
		bool operator ==(const Vector3&) const;
		inline Vector3 operator +(const Vector3& v) { return Vector3(this->x() + v.x(), this->y() + v.y(), this->z() + v.z()); };
		inline Vector3& operator +=(const Vector3& v) { *this = *this + v; return *this; };

		inline float& x() { return _v.x; }
		inline float& y() { return _v.y; }
		inline float& z() { return _v.z; }
		inline float& x() const { return const_cast<float&>(_v.x); }
		inline float& y() const { return const_cast<float&>(_v.y); }
		inline float& z() const { return const_cast<float&>(_v.z); }

		void normalize();
		const Vector3 normalized() const;
		const std::string print() const;

		XMFLOAT3 _v;
	};

	struct Vector2
	{
		static const xVector Zero;
		static const xVector Up;
		static const xVector Down;
		static const xVector Left;
		static const xVector Right;

		static const Vector2 ZeroF2;
		static const Vector2 UpF2;
		static const Vector2 DownF2;
		static const Vector2 LeftF2;
		static const Vector2 RightF2;

		Vector2();
		Vector2(const Vector3& v_in);
		Vector2(const Vector2& v_in);
		Vector2(float, float);
		Vector2(int, int);
		Vector2(unsigned, unsigned);
		Vector2(float);
		Vector2(const XMFLOAT3& f3);
		Vector2(const XMFLOAT2& f2);
		Vector2(const XMVECTOR& v_in);

		operator XMVECTOR() const;
		operator XMFLOAT2() const;
		bool operator ==(const Vector2&) const;
		inline Vector2& operator +=(const Vector2& v) { *this = *this + v; return *this; };

		inline float& x() { return _v.x; }
		inline float& y() { return _v.y; }
		inline float& x() const { return const_cast<float&>(_v.x); }
		inline float& y() const { return const_cast<float&>(_v.y); }

		void normalize();
		const Vector2 normalized() const;

		XMFLOAT2 _v;
	};


	struct Vector4
	{
		//union 
		float x, y, z, w;

		Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		Vector4(const Vector3& v3) : x(v3.x()), y(v3.y()), z(v3.z()), w(0.0f) {}
		Vector4(const Vector3& v3, float _w) : x(v3.x()), y(v3.y()), z(v3.z()), w(_w) {}

		operator XMVECTOR() const;
	};


	/// 3D
	//===============================================================================================
	struct Point
	{
		Point() : pos(0, 0, 0) {}
		Point(float x, float y, float z) : pos(x, y, z) {}
		Point(const XMFLOAT3& f3) : pos(f3) {}
		Point(const XMVECTOR& vec) { XMStoreFloat3(&pos, vec); }
		Point(const Point& r) : pos(r.pos.x, r.pos.y, r.pos.z) {}
		//-------------------------------------------------------------------
		operator XMFLOAT3() const { return pos; }
		operator XMVECTOR() const { return XMLoadFloat3(&pos); }
		Point					operator*(float f)	const { return Point(pos.x * f, pos.y * f, pos.z * f); }
		Point					operator-(const Point& p) { return Point(pos.x - p.pos.x, pos.y - p.pos.y, pos.z - p.pos.z); }
		inline static float		Distance(const Point& p1, const Point& p2) { XMVECTOR AB = XMLoadFloat3(&p2.pos) - XMLoadFloat3(&p1.pos); return sqrtf(XMVector3Dot(AB, AB).m128_f32[0]); }
		//-------------------------------------------------------------------
		XMFLOAT3 pos;
	};

	class Quaternion
	{
	public:
		static Quaternion Identity();
		static Quaternion FromAxisAngle(const XMVECTOR& axis, const float angle);
		static Quaternion Lerp(const Quaternion& from, const Quaternion& to, float t);
		static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t);
		static Vector3 ToEulerRad(const Quaternion& Q);
		static Vector3 ToEulerDeg(const Quaternion& Q);

		Quaternion(const XMMATRIX& rotMatrix);
		Quaternion(float s, const XMVECTOR& v);
		//-----------------------------------------------
		Quaternion  operator+(const Quaternion& q) const;
		Quaternion  operator*(const Quaternion& q) const;
		Quaternion  operator*(float c) const;
		bool		operator==(const Quaternion& q) const;
		float		Dot(const Quaternion& q) const;
		float		Len() const;
		Quaternion  Inverse() const;
		Quaternion  Conjugate() const;
		xMatrix		Matrix() const;
		Quaternion& Normalize();

		Vector3 TransformVector(const Vector3& v) const;

	private:	// used by operator()s
		Quaternion(float s, const Vector3& v);
		Quaternion();

	public:
		// Q = [S, <V>]
		Vector3 V;
		float S;
	};

	//===============================================================================================

	struct FrustumPlaneset
	{
		Vector4 abcd[6];
		enum EPlaneset
		{
			PL_RIGHT = 0,
			PL_LEFT,
			PL_TOP,
			PL_BOTTOM,
			PL_FAR,
			PL_NEAR
		};

		inline static FrustumPlaneset ExtractFromMatrix(const XMMATRIX& m)
		{
			FrustumPlaneset viewPlanes;
			viewPlanes.abcd[FrustumPlaneset::PL_RIGHT] = Vector4(
				m.r[0].m128_f32[3] - m.r[0].m128_f32[0],
				m.r[1].m128_f32[3] - m.r[1].m128_f32[0],
				m.r[2].m128_f32[3] - m.r[2].m128_f32[0],
				m.r[3].m128_f32[3] - m.r[3].m128_f32[0]
			);
			viewPlanes.abcd[FrustumPlaneset::PL_LEFT] = Vector4(
				m.r[0].m128_f32[3] + m.r[0].m128_f32[0],
				m.r[1].m128_f32[3] + m.r[1].m128_f32[0],
				m.r[2].m128_f32[3] + m.r[2].m128_f32[0],
				m.r[3].m128_f32[3] + m.r[3].m128_f32[0]
			);
			viewPlanes.abcd[FrustumPlaneset::PL_TOP] = Vector4(
				m.r[0].m128_f32[3] - m.r[0].m128_f32[1],
				m.r[1].m128_f32[3] - m.r[1].m128_f32[1],
				m.r[2].m128_f32[3] - m.r[2].m128_f32[1],
				m.r[3].m128_f32[3] - m.r[3].m128_f32[1]
			);
			viewPlanes.abcd[FrustumPlaneset::PL_BOTTOM] = Vector4(
				m.r[0].m128_f32[3] + m.r[0].m128_f32[1],
				m.r[1].m128_f32[3] + m.r[1].m128_f32[1],
				m.r[2].m128_f32[3] + m.r[2].m128_f32[1],
				m.r[3].m128_f32[3] + m.r[3].m128_f32[1]
			);
			viewPlanes.abcd[FrustumPlaneset::PL_FAR] = Vector4(
				m.r[0].m128_f32[3] - m.r[0].m128_f32[2],
				m.r[1].m128_f32[3] - m.r[1].m128_f32[2],
				m.r[2].m128_f32[3] - m.r[2].m128_f32[2],
				m.r[3].m128_f32[3] - m.r[3].m128_f32[2]
			);
			viewPlanes.abcd[FrustumPlaneset::PL_NEAR] = Vector4(
				m.r[0].m128_f32[2],
				m.r[1].m128_f32[2],
				m.r[2].m128_f32[2],
				m.r[3].m128_f32[2]
			);
			return viewPlanes;
		}
	};

}