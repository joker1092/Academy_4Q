#pragma once
#include "../InputManager.h"
#include "DeviceResources.h"

class Camera
{
public:
	inline Camera(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, float fov = 70.0f)
		: _fov(fov), deviceResources(deviceResources), _speed(2.0f), _position{ 0.0f, 0.0f, 0.0f, 0.0f }, _front{ 0.0f, 0.0f, -1.0f, 0.0f },
		_mouseSensitivity(50.0f), yaw(270.0f), pitch(0.0f), roll(0.0f), exposure(1.0f), fnear(0.1f), ffar(100.0f), isfree(1)
	{
		//window->ResizeEvent += [this](UINT w, UINT h) { };
	};
	inline ~Camera() {};

	inline DirectX::XMMATRIX GetViewMatrix() const
	{
		DirectX::XMVECTOR at = { 0.0f, 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR pos = GetPosition();
		return DirectX::XMMatrixLookAtRH(pos, DirectX::XMVectorAdd(pos, _front), up);
	}
	inline DirectX::XMMATRIX GetProjectionMatrix() const
	{
		return DirectX::XMMatrixPerspectiveFovRH(
			_fov / 100.0f, 
            deviceResources->GetAspectRatio(),
			fnear,
			ffar
		);
	}

	void Update(float deltaSeconds)
	{
		float speed = _speed * deltaSeconds;

		//if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::LeftControl))
		//{
		//	static double lastpress = 0.0f;
		//	// DirectX11::ThrowIfFailed F key and debounce
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::F) && ((time.total - lastpress) > 0.5f))
		//	{
		//		isfree = !isfree;

		//		lastpress = time.total;
		//	}
		//}

		//if (isfree)
		//{
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::W))
		//	{
		//		_position = DirectX::XMVectorMultiplyAdd(
		//			DirectX::XMVECTOR{ speed, speed, speed, speed },
		//			_front,
		//			_position
		//		);
		//	}
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::S))
		//	{
		//		_position = DirectX::XMVectorMultiplyAdd(
		//			DirectX::XMVECTOR{ -speed, -speed, -speed, -speed },
		//			_front,
		//			_position
		//		);
		//	}
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::A))
		//	{
		//		_position = DirectX::XMVectorMultiplyAdd(
		//			DirectX::XMVECTOR{ -speed, -speed, -speed, -speed },
		//			GetRight(),
		//			_position
		//		);
		//	}
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::D))
		//	{
		//		_position = DirectX::XMVectorMultiplyAdd(
		//			DirectX::XMVECTOR{ speed, speed, speed, speed },
		//			GetRight(),
		//			_position
		//		);
		//	}
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedMouseRightPress())
		//	{
		//		float mousesens = _mouseSensitivity * time.delta;
		//		DirectX::XMFLOAT2 delta = InputSystem::Instance->GetDeltaMouse();
		//		yaw += delta.x * mousesens;
		//		pitch -= delta.y * mousesens;
		//	}
		//}
		//else
		//{
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::W))
		//	{
		//		_position = DirectX::XMVectorAdd(_position, DirectX::XMVECTOR{ 0.0f , 0.0f , -speed, 0.0f });
		//	}
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::S))
		//	{
		//		_position = DirectX::XMVectorAdd(_position, DirectX::XMVECTOR{ 0.0f , 0.0f , speed, 0.0f });
		//	}
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::A))
		//	{
		//		_position = DirectX::XMVectorAdd(_position, DirectX::XMVECTOR{ -speed , 0.0f , 0.0f, 0.0f });
		//	}
		//	if (InputSystem::Instance->DirectX11::ThrowIfFailedKeyPress(DirectX::Keyboard::Keys::D))
		//	{
		//		_position = DirectX::XMVectorAdd(_position, DirectX::XMVECTOR{ speed , 0.0f , 0.0f, 0.0f });
		//	}
		//}

		if (yaw > 360)
		{
			yaw = 0;
		}
		if (yaw < 0)
		{
			yaw = 360;
		}
		if (pitch > 89)
		{
			pitch = 89;
		}
		if (pitch < -89)
		{
			pitch = -89;
		}

		DirectX::XMFLOAT3 front;

		float cospitch = DirectX::XMScalarCos(DirectX::XMConvertToRadians(pitch));
		front.x = DirectX::XMScalarCos(DirectX::XMConvertToRadians(yaw)) * cospitch;
		front.y = DirectX::XMScalarSin(DirectX::XMConvertToRadians(pitch));
		front.z = DirectX::XMScalarSin(DirectX::XMConvertToRadians(yaw)) * cospitch;
		_front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));

		DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_front, DirectX::XMVECTOR{ 0.0f, 1.0f, 0.0f, 1.0f }));
		_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, _front));
	}

	inline DirectX::XMVECTOR GetRight()
	{
		return DirectX::XMVector3Normalize(
			DirectX::XMVector3Cross(_front, _up)
		);
	}

	inline DirectX::XMVECTOR GetPosition() const
	{
		return _position;
	}
	inline DirectX::XMVECTOR GetViewPosition() const
	{
		return _front;
	}
	inline void SetPosition(DirectX::XMFLOAT3 pos)
	{
		_position = DirectX::XMLoadFloat3(&pos);
	}
	inline void SetPosition(float x, float y, float z)
	{
		_position = DirectX::XMVECTOR{x, y, z, 0.0f};
	}


	float yaw;
	float pitch;
	float roll;

	float exposure;
	float fnear;
	float ffar;
	BOOL isfree;

private:
	std::shared_ptr<DirectX11::DeviceResources>	deviceResources;
	DirectX::XMVECTOR	_position;
	DirectX::XMVECTOR	_front;
	DirectX::XMVECTOR	_up;
	float			_fov;
	float			_speed;
	float			_mouseSensitivity;

};
