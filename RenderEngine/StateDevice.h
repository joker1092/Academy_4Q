#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"

enum class RasterizerType
{
	Solid,
	SolidNoCull,
	SolidFrontCull,
	Wireframe,
	Conservative
};

enum class DepthStencilType
{
	Less,
	LessEqual,
};

class StateDevice
{
public:
	StateDevice(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources) :
		m_DeviceResources(deviceResources) {
	}
	~StateDevice() = default;

	void SetRenderTarget(const )
};