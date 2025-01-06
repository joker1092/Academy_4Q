#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"
#include "../InputManager.h"

class Camera
{
public:
	Camera(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, float fov = 70.f);
	~Camera() = default;

	Mathf::xMatrix GetViewMatrix() const;

	void Update(float deltaSeconds);

	Mathf::xVector GetRight() const;
	Mathf::xVector GetPosition() const;
	Mathf::xVector GetViewPosition() const;
	void SetPosition(Mathf::xVector position);
	void SetPosition(float x, float y, float z);

	float m_aspectRatio{};
	float m_exposure{};

	float m_nearPlane{};
	float m_farPlane{};
	

	float m_yaw{};
	float m_pitch{};
	float m_roll{};
private:

	Mathf::xVector m_position{};
	Mathf::xVector m_front{};
	Mathf::xVector m_up{};

	float m_fov{};
	float m_speed{};
	float m_sensitivity{};
};