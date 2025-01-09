#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"
#include "RenderTarget.h"

enum class RasterizerType
{
	Solid = 0,
	SolidNoCull = 1,
	SolidFrontCull = 2,
	Wireframe = 3,
	Conservative = 4
};

enum class DepthStencilType
{
	Less = 0,
	LessEqual = 1
};

class LogicalDevice
{
public:
	LogicalDevice(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
	~LogicalDevice() = default;

	void SetRenderTarget(const RenderTarget* target);
	void SetRenderTargets(const RenderTarget* maintarget, const RenderTarget* second);
	void SetRenderTargetBackbuffer();
	void SetRenderTargetNothing();
	void SetDepthStencil(DepthStencilType type);
	void SetRasterizer(RasterizerType type);

	void DrawIndexed(const UINT idxCount, const UINT idxStart = 0, const UINT baseVertexLocation = 0) const;
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
	void ClearBackbuffer();

    DirectX11::DeviceResources* GetDeviceResources() { return deviceResources.get(); }

private:
	void CreateDepthState();
	void CreateRasterizerStates();

private:
	ComPtr<ID3D11DepthStencilState>		_depthStateLess;
	ComPtr<ID3D11DepthStencilState>		_depthStateLessEqual;
	ComPtr<ID3D11RasterizerState2>		_rasterizerStateSolid;
	ComPtr<ID3D11RasterizerState2>		_rasterizerStateSolidNoCull;
	ComPtr<ID3D11RasterizerState2>		_rasterizerStateSolidFrontCull;
	ComPtr<ID3D11RasterizerState2>		_rasterizerStateWireframe;
	ComPtr<ID3D11RasterizerState2>		_rasterizerStateConservative;
    std::shared_ptr<DirectX11::DeviceResources> deviceResources;
};

