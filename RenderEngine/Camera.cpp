#include "Camera.h"

Camera::Camera(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, float fov) :
	m_fov(fov), m_aspectRatio(deviceResources->GetAspectRatio()), m_nearPlane(0.1f), m_farPlane(100.f),
	m_exposure(1.f), m_speed(2.f), m_sensitivity(50.0f), m_yaw(270.f), m_pitch(0.f), m_roll(0.f),
	m_position{ 0.f, 0.f, 0.f, 0.f }, m_front{ 0.f, 0.f, -1.f, 0.f }
{
}

Mathf::xMatrix Camera::GetViewMatrix() const
{
	Mathf::xVector at = { 0.f, 0.f, 0.f, 0.f };
	Mathf::xVector up = { 0.f, 1.f, 0.f, 0.f };
	Mathf::xVector pos = GetPosition();

	return DirectX::XMMatrixLookAtRH(pos, DirectX::XMVectorAdd(pos, m_front), up);
}

void Camera::Update(float deltaSeconds)
{
	float velocity = m_speed * deltaSeconds;

	if (InputManagement->IsKeyPressed('w'))
	{
		m_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ velocity, velocity, velocity, velocity },
			m_front,
			m_position
		);
	}
	if (InputManagement->IsKeyPressed('S'))
	{
		m_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ -velocity, -velocity, -velocity, -velocity },
			m_front,
			m_position
		);
	}
	if (InputManagement->IsKeyPressed('A'))
	{
		m_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ -velocity, -velocity, -velocity, -velocity },
			GetRight(),
			m_position
		);
	}
	if (InputManagement->IsKeyPressed('D'))
	{
		m_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ velocity, velocity, velocity, velocity },
			GetRight(),
			m_position
		);
	}
	if (InputManagement->IsMouseButtonPressed(MouseKey::RIGHT))
	{
		float mouseSensitive = m_sensitivity * deltaSeconds;
		float2 mouseDelta = { InputManagement->GetMouseDelta().x, InputManagement->GetMouseDelta().y };
		m_yaw += mouseDelta.x * mouseSensitive;
		m_pitch -= mouseDelta.y * mouseSensitive;
	}

	Mathf::Clamp(m_yaw, 0.f, 360.f);
	Mathf::Clamp(m_pitch, -89.f, 89.f);

	float3 front;

	float cosPitch = DirectX::XMScalarCos(DirectX::XMConvertToRadians(m_pitch));
	front.x = DirectX::XMScalarCos(DirectX::XMConvertToRadians(m_yaw)) * cosPitch;
	front.y = DirectX::XMScalarSin(DirectX::XMConvertToRadians(m_pitch));
	front.z = DirectX::XMScalarSin(DirectX::XMConvertToRadians(m_yaw)) * cosPitch;

	m_front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));

	Mathf::xVector right = 
		DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_front, { 0.f, 1.f, 0.f, 0.f }));
	m_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, m_front));
}

Mathf::xVector Camera::GetRight() const
{
	return DirectX::XMVector3Normalize(DirectX::XMVector3Cross(m_front, m_up));
}

Mathf::xVector Camera::GetPosition() const
{
	return m_position;
}

Mathf::xVector Camera::GetViewPosition() const
{
	return m_front;
}

void Camera::SetPosition(Mathf::xVector position)
{
	m_position = position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_position = { x, y, z, 0.f };
}
