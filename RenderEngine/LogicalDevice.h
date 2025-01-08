#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"
#include "RenderTarget.h"

namespace DX
{
    namespace States
    {
        extern ID3D11Device3* Device;
        extern ID3D11DeviceContext3* Context;
    }
}

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
	~LogicalDevice();

	//inline void SetViewport(const D3D11_VIEWPORT viewport) { _activeViewport = viewport; };
	inline void SetRenderTarget(const RenderTarget* target)
	{
        auto viewPort = deviceResources->GetScreenViewport();

        deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(1, target->GetRTVAddress(), target->GetDSV());
        deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
	}
	// Uses main target depth buffer
	inline void SetRenderTargets(const RenderTarget* maintarget, const RenderTarget* second)
	{
		ID3D11RenderTargetView* views[]{ maintarget->GetRTV(), second->GetRTV() };
        auto viewPort = deviceResources->GetScreenViewport();

        deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(2, views, maintarget->GetDSV());
        deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
	}
	inline void SetRenderTargetBackbuffer()
	{
		ID3D11RenderTargetView* views[]{ deviceResources->GetBackBufferRenderTargetView(), nullptr };
        auto viewPort = deviceResources->GetScreenViewport();

        deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(2, views, deviceResources->GetDepthStencilView());
        deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
	}
	inline void SetRenderTargetNothing()
	{
		ID3D11RenderTargetView* views[]{ nullptr, nullptr };
        auto viewPort = deviceResources->GetScreenViewport();

        deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(2, views, nullptr);
        deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
	}
	void SetDepthStencil(DepthStencilType type)
	{
		switch (type)
		{
		case DepthStencilType::Less:
            deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(_depthStateLess.Get(), 0);
			return;
		case DepthStencilType::LessEqual:
            deviceResources->GetD3DDeviceContext()->OMSetDepthStencilState(_depthStateLessEqual.Get(), 0);
			return;
		default:
			break;
		}
	}
	void SetRasterizer(RasterizerType type)
	{
		switch (type)
		{
		case RasterizerType::Solid:
            deviceResources->GetD3DDeviceContext()->RSSetState(_rasterizerStateSolid.Get());
			return;
		case RasterizerType::SolidNoCull:
            deviceResources->GetD3DDeviceContext()->RSSetState(_rasterizerStateSolidNoCull.Get());
			return;
		case RasterizerType::SolidFrontCull:
            deviceResources->GetD3DDeviceContext()->RSSetState(_rasterizerStateSolidFrontCull.Get());
			return;
		case RasterizerType::Wireframe:
            deviceResources->GetD3DDeviceContext()->RSSetState(_rasterizerStateWireframe.Get());
			return;
		case RasterizerType::Conservative:
            deviceResources->GetD3DDeviceContext()->RSSetState(_rasterizerStateConservative.Get());
			return;
		default:
			break;
		}
	}

	inline void DrawIndexed(const UINT idxCount, const UINT idxStart = 0, const UINT baseVertexLocation = 0) const
	{
        deviceResources->GetD3DDeviceContext()->DrawIndexed(idxCount, idxStart, baseVertexLocation);
	}
	//inline void SetViewport(const D3D11_VIEWPORT& viewport) { DX::States::Context->RSSetViewports(1, &viewport); }
	inline void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { deviceResources->GetD3DDeviceContext()->IASetPrimitiveTopology(topology); };
	inline void ClearBackbuffer()
    {
        deviceResources->GetD3DDeviceContext()->ClearRenderTargetView(deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
        deviceResources->GetD3DDeviceContext()->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

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

	//D3D11_VIEWPORT _activeViewport;
};

