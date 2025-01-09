#pragma once
#include "Core.Minimal.h"
#include "../InputManager.h"
#include "DeviceResources.h"

class Camera
{
public:
	Camera(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, float fov = 70.0f);
	~Camera() = default;

	Mathf::xMatrix GetViewMatrix() const;
	Mathf::xMatrix GetProjectionMatrix() const;

	void Update(float deltaSeconds);

	Mathf::xVector GetRight();
	Mathf::xVector GetPosition() const;
	Mathf::xVector GetViewPosition() const;
	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetPosition(float x, float y, float z);

	float yaw;
	float pitch;
	float roll;

	float exposure;
	float fnear;
	float ffar;

private:
	std::shared_ptr<DirectX11::DeviceResources>	deviceResources{};
	Mathf::xVector	_position{};
	Mathf::xVector	_front{};
	Mathf::xVector	_up{};
	float			_fov{};
	float			_speed{};
	float			_mouseSensitivity{};

};
