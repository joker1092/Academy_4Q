#include "Camera.h"

Camera::Camera(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, float fov) :
	_fov(fov), 
	deviceResources(deviceResources), 
	_speed(2.0f), 
	_position{ Mathf::xVectorZero },
	_front{ 0.0f, 0.0f, -1.0f, 0.0f },
	_up{ Mathf::xVectorZero },
	_mouseSensitivity(50.0f), 
	yaw(270.0f), 
	pitch(0.0f), 
	roll(0.0f), 
	exposure(1.0f), 
	fnear(0.1f), 
	ffar(100.0f)
{
}

Mathf::xMatrix Camera::GetViewMatrix() const
{
	Mathf::xVector pos = GetPosition();
	return DirectX::XMMatrixLookAtRH(pos, DirectX::XMVectorAdd(pos, _front), _up);
}

Mathf::xMatrix Camera::GetProjectionMatrix() const
{
	return DirectX::XMMatrixPerspectiveFovRH(
		_fov * 0.01f,
		deviceResources->GetAspectRatio(),
		fnear,
		ffar
	);
}

void Camera::Update(float deltaSeconds)
{
	float speed = _speed * deltaSeconds;

	if (InputManagement->IsKeyDown('W'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ speed, speed, speed, speed },
			_front,
			_position
		);
	}
	if (InputManagement->IsKeyDown('S'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ -speed, -speed, -speed, -speed },
			_front,
			_position
		);
	}
	if (InputManagement->IsKeyDown('A'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ -speed, -speed, -speed, -speed },
			GetRight(),
			_position
		);
	}
	if (InputManagement->IsKeyDown('D'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ speed, speed, speed, speed },
			GetRight(),
			_position
		);
	}
	if (InputManagement->IsMouseButtonDown(MouseKey::MIDDLE))
	{
		float mousesens = _mouseSensitivity * deltaSeconds;
		float2 delta = InputManagement->GetMouseDelta();

		yaw += delta.x * mousesens;
		pitch -= delta.y * mousesens;
	}

	Mathf::Wrap(yaw, 0.0f, 360.0f);
	Mathf::Wrap(pitch, -89.0f, 89.0f);

	float3 front{};

	float cospitch = DirectX::XMScalarCos(DirectX::XMConvertToRadians(pitch));
	front.x = DirectX::XMScalarCos(DirectX::XMConvertToRadians(yaw)) * cospitch;
	front.y = DirectX::XMScalarSin(DirectX::XMConvertToRadians(pitch));
	front.z = DirectX::XMScalarSin(DirectX::XMConvertToRadians(yaw)) * cospitch;
	_front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));

	Mathf::xVector right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_front, Mathf::xVectorUp));
	_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, _front));
}

Mathf::xVector Camera::GetRight()
{
	return DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(_front, _up)
	);
}

Mathf::xVector Camera::GetPosition() const
{
	return _position;
}

Mathf::xVector Camera::GetViewPosition() const
{
	return _front;
}

void Camera::SetPosition(DirectX::XMFLOAT3 pos)
{
	_position = DirectX::XMLoadFloat3(&pos);
}

void Camera::SetPosition(float x, float y, float z)
{
	_position = Mathf::xVector{ x, y, z, 0.0f };
}
