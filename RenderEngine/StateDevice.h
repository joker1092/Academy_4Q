#pragma once
#include "Core.Minimal.h"
#include "DeviceResources.h"
#include "RenderTarget.h"

enum class RasterizerType
{
	Solid,
	SolidNoCull,
	SolidFrontCull,
	Wireframe,
	//Conservative
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
        m_Device(deviceResources->GetD3DDevice()),
        m_DeviceContext(deviceResources->GetD3DDeviceContext()),
        m_viewPort(deviceResources->GetScreenViewport()),
        m_RenderTargetViewBackBuffer(deviceResources->GetBackBufferRenderTargetView()),
        m_DepthStencilViewMain(deviceResources->GetDepthStencilView())
    {
    }
	~StateDevice() = default;

    void SetRenderTarget();
	void SetRenderTarget(const RenderTarget* target);
    void SetRenderTarget(const RenderTarget* mainTarget, const RenderTarget* second);
    void SetRenderTargetBackBuffer();

    void SetDepthStencilState(DepthStencilType type);
    void SetRasterizerState(RasterizerType type);

    void DrawIndexed(const uint32 indexCount, const uint32 startIndexLocation = 0, const int32 baseVertexLocation = 0);
    void Draw(const uint32 vertexCount, const uint32 startVertexLocation = 0);
    void DrawInstanced(const uint32 vertexCountPerInstance, const uint32 instanceCount, const uint32 startVertexLocation = 0, const uint32 startInstanceLocation = 0);
    void DrawIndexedInstanced(const uint32 indexCountPerInstance, const uint32 instanceCount, const uint32 startIndexLocation = 0, const int32 baseVertexLocation = 0, const uint32 startInstanceLocation = 0);

    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
    void ClearRenderTargetBackBuffer(DirectX::XMVECTORF32 clearColor);

private:
    void CreateDepthState();
    void CreateRasterizerState();

private:
    ID3D11Device*                   m_Device{};
    ID3D11DeviceContext*            m_DeviceContext{};
    ID3D11RenderTargetView*         m_RenderTargetViewBackBuffer{};
    ID3D11DepthStencilView*         m_DepthStencilViewMain{};

    ComPtr<ID3D11DepthStencilState> m_DepthStencilStateLess{};
    ComPtr<ID3D11DepthStencilState> m_DepthStencilStateLessEqual{};
    ComPtr<ID3D11RasterizerState>   m_RasterizerStateSolid{};
    ComPtr<ID3D11RasterizerState>   m_RasterizerStateSolidNoCull{};
    ComPtr<ID3D11RasterizerState>   m_RasterizerStateSolidFrontCull{};
    ComPtr<ID3D11RasterizerState>   m_RasterizerStateWireframe{};
    //ComPtr<ID3D11RasterizerState>   m_RasterizerStateConservative{};

    D3D11_VIEWPORT m_viewPort{};
};
