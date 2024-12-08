#include "Core.Mathf.h"
#include "Core.Random.h"
#include <cmath>
#include <algorithm>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

Mathf::Vector3 Mathf::Vector3::Rand()
{
	Random<float> randX(0.0f, 1.0f);
	Random<float> randY(0.0f, 1.0f);
	Random<float> randZ(0.0f, 1.0f);

	Vector3 v = Vector3(randX.Generate(), randY.Generate(), randZ.Generate());
	return v.normalized();
}
const std::string Mathf::Vector3::print() const
{
	std::string s = "(";
	s += std::to_string(_v.x) + ", " + std::to_string(_v.y) + ", " + std::to_string(_v.z) + ")";
	return s;
}

const Mathf::xVector Mathf::Vector3::Zero = XMVectorZero();
const Mathf::xVector Mathf::Vector3::Up = XMVectorSet(+0.0f, +1.0f, +0.0f, +0.0f);
const Mathf::xVector Mathf::Vector3::Down = XMVectorSet(+0.0f, -1.0f, +0.0f, +0.0f);
const Mathf::xVector Mathf::Vector3::Left = XMVectorSet(-1.0f, +0.0f, +0.0f, +0.0f);
const Mathf::xVector Mathf::Vector3::Right = XMVectorSet(+1.0f, +0.0f, +0.0f, +0.0f);
const Mathf::xVector Mathf::Vector3::Forward = XMVectorSet(+0.0f, +0.0f, +1.0f, +0.0f);
const Mathf::xVector Mathf::Vector3::Back = XMVectorSet(+0.0f, +0.0f, -1.0f, +0.0f);

const Mathf::Vector3 Mathf::Vector3::ZeroF3 = Vector3();
const Mathf::Vector3 Mathf::Vector3::UpF3 = Vector3(+0.0f, +1.0f, +0.0f);
const Mathf::Vector3 Mathf::Vector3::DownF3 = Vector3(+0.0f, -1.0f, +0.0f);
const Mathf::Vector3 Mathf::Vector3::LeftF3 = Vector3(-1.0f, +0.0f, +0.0f);
const Mathf::Vector3 Mathf::Vector3::RightF3 = Vector3(+1.0f, +0.0f, +0.0f);
const Mathf::Vector3 Mathf::Vector3::ForwardF3 = Vector3(+0.0f, +0.0f, +1.0f);
const Mathf::Vector3 Mathf::Vector3::BackF3 = Vector3(+0.0f, +0.0f, -1.0f);

const Mathf::Vector3 Mathf::Vector3::XAxis = Vector3(1.0f, 0.0f, 0.0f);
const Mathf::Vector3 Mathf::Vector3::YAxis = Vector3(0.0f, 1.0f, 0.0f);
const Mathf::Vector3 Mathf::Vector3::ZAxis = Vector3(0.0f, 0.0f, 1.0f);

const Mathf::xVector Mathf::Vector2::Zero = XMVectorZero();
const Mathf::xVector Mathf::Vector2::Up = XMVectorSet(+0.0f, +1.0f, +0.0f, +0.0f);
const Mathf::xVector Mathf::Vector2::Down = XMVectorSet(+0.0f, -1.0f, +0.0f, +0.0f);
const Mathf::xVector Mathf::Vector2::Left = XMVectorSet(-1.0f, +0.0f, +0.0f, +0.0f);
const Mathf::xVector Mathf::Vector2::Right = XMVectorSet(+1.0f, +0.0f, +0.0f, +0.0f);

const Mathf::Vector2 Mathf::Vector2::ZeroF2 = Vector2();
const Mathf::Vector2 Mathf::Vector2::UpF2 = Vector2(+0.0f, +1.0f);
const Mathf::Vector2 Mathf::Vector2::DownF2 = Vector2(+0.0f, -1.0f);
const Mathf::Vector2 Mathf::Vector2::LeftF2 = Vector2(-1.0f, +0.0f);
const Mathf::Vector2 Mathf::Vector2::RightF2 = Vector2(+1.0f, +0.0f);

Mathf::Vector3::Vector3() : _v(XMFLOAT3(0.0f, 0.0f, 0.0f)) {}
Mathf::Vector3::Vector3(const Vector3& v_in) : _v(v_in._v) {}
Mathf::Vector3::Vector3(const XMFLOAT3& f3) : _v(f3) {}
Mathf::Vector3::Vector3(const XMVECTOR& v_in) { XMStoreFloat3(&_v, v_in); }
Mathf::Vector3::Vector3(float x, float y, float z) : _v(x, y, z) {}
Mathf::Vector3::Vector3(float x) : _v(x, x, x) {}

Mathf::Vector2::Vector2() : _v(XMFLOAT2(0.0f, 0.0f)) {}
Mathf::Vector2::Vector2(const Vector3& v3) : _v(v3.x(), v3.y()) {}
Mathf::Vector2::Vector2(const Vector2& v_in) : _v(v_in._v) {}
Mathf::Vector2::Vector2(float x, float y) : _v(x, y) {}
Mathf::Vector2::Vector2(int x, int y) : _v(static_cast<float>(x), static_cast<float>(y)) {}
Mathf::Vector2::Vector2(unsigned x, unsigned y) : _v(static_cast<float>(x), static_cast<float>(y)) {}
Mathf::Vector2::Vector2(float f) : _v(f, f) {}
Mathf::Vector2::Vector2(const XMFLOAT2& f2) : _v(f2) {}
Mathf::Vector2::Vector2(const XMFLOAT3& f3) : _v(f3.x, f3.y) {}
Mathf::Vector2::Vector2(const XMVECTOR& v_in) { XMStoreFloat2(&_v, v_in); }

Mathf::Vector3::operator XMVECTOR() const { return XMLoadFloat3(&_v); }
Mathf::Vector3::operator XMFLOAT3() const { return _v; }
bool Mathf::Vector3::operator==(const Vector3& v) const { return v._v.x == _v.x && v._v.y == _v.y && v._v.z == _v.z; }
Mathf::Vector2::operator XMVECTOR() const { return XMLoadFloat2(&_v); }
bool Mathf::Vector2::operator==(const Vector2& v) const { return v._v.x == _v.x && v._v.y == _v.y; }
Mathf::Vector2::operator XMFLOAT2() const { return _v; }

void Mathf::Vector3::normalize()
{
	XMVECTOR v = XMLoadFloat3(&_v);
	_v = Mathf::Vector3(XMVector3Normalize(v));
}

const Mathf::Vector3 Mathf::Vector3::normalized() const
{
	XMVECTOR v = XMLoadFloat3(&_v);
	return Mathf::Vector3(XMVector3Normalize(v));
}

void Mathf::Vector2::normalize()
{
	XMVECTOR v = XMLoadFloat2(&_v);
	_v = Mathf::Vector2(XMVector2Normalize(v));
}

const Mathf::Vector2 Mathf::Vector2::normalized() const
{
	XMVECTOR v = XMLoadFloat2(&_v);
	return Mathf::Vector2(XMVector2Normalize(v));
}

Mathf::Vector4::operator XMVECTOR() const { return XMLoadFloat4(&XMFLOAT4(x, y, z, w)); }

Mathf::Quaternion::Quaternion(float s, const Vector3& v)
	:
	S(s),
	V(v)
{
}

Mathf::Quaternion::Quaternion() : S(0), V() {}

Mathf::Quaternion::Quaternion(const XMMATRIX& rotMatrix)
{
	XMVECTOR scl = XMVectorZero();
	XMVECTOR quat = XMVectorZero();
	XMVECTOR trans = XMVectorZero();
	XMMatrixDecompose(&scl, &quat, &trans, XMMatrixTranspose(rotMatrix));

	*this = Quaternion(quat.m128_f32[3], quat).Conjugate();
	int a = 5;
}

Mathf::Quaternion::Quaternion(float s, const XMVECTOR& v)
	:
	S(s),
	V(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2])
{
}

Mathf::Quaternion Mathf::Quaternion::Identity()
{
	return Quaternion(1, Vector3(0.0f, 0.0f, 0.0f));
}


Mathf::Quaternion Mathf::Quaternion::FromAxisAngle(const XMVECTOR& axis, const float angle)
{
	const float half_angle = angle / 2;
	Quaternion Q = Quaternion::Identity();
	Q.S = cosf(half_angle);
	Q.V = axis * sinf(half_angle);
	return Q;
}

Mathf::Quaternion Mathf::Quaternion::Lerp(const Mathf::Quaternion& from, const Mathf::Quaternion& to, float t)
{
	return  from * (1.0f - t) + to * t;
}

Mathf::Quaternion Mathf::Quaternion::Slerp(const Mathf::Quaternion& from, const Mathf::Quaternion& to, float t)
{
	double alpha = std::acos(from.Dot(to));
	if (alpha < 0.00001) return from;
	double sina = std::sin(alpha);
	double sinta = std::sin(t * alpha);
	Quaternion interpolated = from * static_cast<float>(std::sin(alpha - t * alpha) / sina) +
		to * static_cast<float>(sinta / sina);
	interpolated.Normalize();
	return interpolated;
}

Mathf::Vector3 Mathf::Quaternion::ToEulerRad(const Mathf::Quaternion& Q)
{
	double ysqr = Q.V.y() * Q.V.y();
	double t0 = -2.0f * (ysqr + Q.V.z() * Q.V.z()) + 1.0f;
	double t1 = +2.0f * (Q.V.x() * Q.V.y() - Q.S * Q.V.z());
	double t2 = -2.0f * (Q.V.x() * Q.V.z() + Q.S * Q.V.y());
	double t3 = +2.0f * (Q.V.y() * Q.V.z() - Q.S * Q.V.x());
	double t4 = -2.0f * (Q.V.x() * Q.V.x() + ysqr) + 1.0f;

	t2 = t2 > 1.0f ? 1.0f : t2;
	t2 = t2 < -1.0f ? -1.0f : t2;

	float pitch = static_cast<float>(std::asin(t2));
	float roll = static_cast<float>(std::atan2(t3, t4));
	float yaw = static_cast<float>(std::atan2(t1, t0));
	return Vector3(roll, pitch, yaw);
}

Mathf::Vector3 Mathf::Quaternion::ToEulerDeg(const Mathf::Quaternion& Q)
{
	Mathf::Vector3 eul = Mathf::Quaternion::ToEulerRad(Q);
	eul.x() *= Mathf::Rad2Deg;
	eul.y() *= Mathf::Rad2Deg;
	eul.z() *= Mathf::Rad2Deg;
	return eul;
}


Mathf::Quaternion Mathf::Quaternion::operator+(const Quaternion& q) const
{
	Quaternion result;
	XMVECTOR V1 = XMVectorSet(V.x(), V.y(), V.z(), 0);
	XMVECTOR V2 = XMVectorSet(q.V.x(), q.V.y(), q.V.z(), 0);

	result.S = this->S + q.S;
	result.V = V1 + V2;
	return result;
}

Mathf::Quaternion Mathf::Quaternion::operator*(const Quaternion& q) const
{
	Quaternion result;
	XMVECTOR V1 = XMVectorSet(V.x(), V.y(), V.z(), 0);
	XMVECTOR V2 = XMVectorSet(q.V.x(), q.V.y(), q.V.z(), 0);

	result.S = this->S * q.S - XMVector3Dot(V1, V2).m128_f32[0];
	result.V = this->S * V2 + q.S * V1 + XMVector3Cross(V1, V2);
	return result;
}

Mathf::Quaternion Mathf::Quaternion::operator*(float c) const
{
	Quaternion result;
	result.S = c * S;
	result.V = Vector3(V.x() * c, V.y() * c, V.z() * c);
	return result;
}


bool Mathf::Quaternion::operator==(const Mathf::Quaternion& q) const
{
	double epsilons[4] = { 99999.0, 99999.0, 99999.0, 99999.0 };
	epsilons[0] = static_cast<double>(q.V.x()) - static_cast<double>(this->V.x());
	epsilons[1] = static_cast<double>(q.V.y()) - static_cast<double>(this->V.y());
	epsilons[2] = static_cast<double>(q.V.z()) - static_cast<double>(this->V.z());
	epsilons[3] = static_cast<double>(q.S) - static_cast<double>(this->S);
	bool same_x = std::abs(epsilons[0]) < 0.000001;
	bool same_y = std::abs(epsilons[1]) < 0.000001;
	bool same_z = std::abs(epsilons[2]) < 0.000001;
	bool same_w = std::abs(epsilons[3]) < 0.000001;
	return same_x && same_y && same_z && same_w;
}

float Mathf::Quaternion::Dot(const Mathf::Quaternion& q) const
{
	XMVECTOR V1 = XMVectorSet(V.x(), V.y(), V.z(), 0);
	XMVECTOR V2 = XMVectorSet(q.V.x(), q.V.y(), q.V.z(), 0);
	return std::max(-1.0f, std::min(S * q.S + XMVector3Dot(V1, V2).m128_f32[0], 1.0f));
}

float Mathf::Quaternion::Len() const
{
	return sqrt(S * S + V.x() * V.x() + V.y() * V.y() + V.z() * V.z());
}

Mathf::Quaternion Mathf::Quaternion::Inverse() const
{
	Quaternion result;
	float f = 1.0f / (S * S + V.x() * V.x() + V.y() * V.y() + V.z() * V.z());
	result.S = f * S;
	result.V = Vector3(-V.x() * f, -V.y() * f, -V.z() * f);
	return result;
}

Mathf::Quaternion Mathf::Quaternion::Conjugate() const
{
	Quaternion result;
	result.S = S;
	result.V = Vector3(-V.x(), -V.y(), -V.z());
	return result;
}

Mathf::xMatrix Mathf::Quaternion::Matrix() const
{
	XMMATRIX m = XMMatrixIdentity();
	float y2 = V.y() * V.y();
	float z2 = V.z() * V.z();
	float x2 = V.x() * V.x();
	float xy = V.x() * V.y();
	float sz = S * V.z();
	float xz = V.x() * V.z();
	float sy = S * V.y();
	float yz = V.y() * V.z();
	float sx = S * V.x();

	XMVECTOR r0 = XMVectorSet(1.0f - 2.0f * (y2 + z2), 2 * (xy - sz), 2 * (xz + sy), 0);
	XMVECTOR r1 = XMVectorSet(2 * (xy + sz), 1.0f - 2.0f * (x2 + z2), 2 * (yz - sx), 0);
	XMVECTOR r2 = XMVectorSet(2 * (xz - sy), 2 * (yz + sx), 1.0f - 2.0f * (x2 + y2), 0);
	XMVECTOR r3 = XMVectorSet(0, 0, 0, 1);

	m.r[0] = r0;
	m.r[1] = r1;
	m.r[2] = r2;
	m.r[3] = r3;
	return XMMatrixTranspose(m);
}

Mathf::Quaternion& Mathf::Quaternion::Normalize()
{
	float len = Len();
	if (len > 0.00001)
	{
		S = S / len;
		V.x() = V.x() / len;
		V.y() = V.y() / len;
		V.z() = V.z() / len;
	}
	return *this;
}

Mathf::Vector3 Mathf::Quaternion::TransformVector(const Vector3& v) const
{
	Quaternion pure(0.0f, v);
	Quaternion rotated = *this * pure * this->Conjugate();
	return Vector3(rotated.V);
}