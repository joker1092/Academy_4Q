#include "LogicalDevice.h"
#include "DXStates.h"

LogicalDevice::LogicalDevice(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources) : deviceResources(deviceResources)
{
    DX::States::Device = deviceResources->GetD3DDevice();
    DX::States::Context = deviceResources->GetD3DDeviceContext();

    CreateDepthState();
    CreateRasterizerStates();
}

void LogicalDevice::SetRenderTarget(const RenderTarget* target)
{
    auto viewPort = deviceResources->GetScreenViewport();
    deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(1, target->GetRTVAddress(), target->GetDSV());
    deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
}

void LogicalDevice::SetRenderTargets(const RenderTarget* maintarget, const RenderTarget* second)
{
    ID3D11RenderTargetView* views[]{ maintarget->GetRTV(), second->GetRTV() };
    auto viewPort = deviceResources->GetScreenViewport();

    deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(2, views, maintarget->GetDSV());
    deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
}

void LogicalDevice::SetRenderTargetBackbuffer()
{
    ID3D11RenderTargetView* views[]{ deviceResources->GetBackBufferRenderTargetView(), nullptr };
    auto viewPort = deviceResources->GetScreenViewport();

    deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(2, views, deviceResources->GetDepthStencilView());
    deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
}

void LogicalDevice::SetRenderTargetNothing()
{
    ID3D11RenderTargetView* views[]{ nullptr, nullptr };
    auto viewPort = deviceResources->GetScreenViewport();

    deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(2, views, nullptr);
    deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewPort);
}

void LogicalDevice::SetDepthStencil(DepthStencilType type)
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

void LogicalDevice::SetRasterizer(RasterizerType type)
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

void LogicalDevice::DrawIndexed(const UINT idxCount, const UINT idxStart, const UINT baseVertexLocation) const
{
    deviceResources->GetD3DDeviceContext()->DrawIndexed(idxCount, idxStart, baseVertexLocation);
}

void LogicalDevice::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
    deviceResources->GetD3DDeviceContext()->IASetPrimitiveTopology(topology);
}

void LogicalDevice::ClearBackbuffer()
{
    deviceResources->GetD3DDeviceContext()->ClearRenderTargetView(deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
    deviceResources->GetD3DDeviceContext()->ClearDepthStencilView(deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void LogicalDevice::CreateDepthState()
{
    {
        // Create Depth Stencil
        CD3D11_DEPTH_STENCIL_DESC dsDesc{ CD3D11_DEFAULT() };
        dsDesc.StencilEnable = TRUE;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;

        // Create depth stencil state
        DirectX11::ThrowIfFailed(DX::States::Device->CreateDepthStencilState(&dsDesc, _depthStateLess.ReleaseAndGetAddressOf()));
        DirectX::SetName(_depthStateLess.Get(), "Depth Stencil State Less");
    }

    {
        // Create Depth Stencil
        CD3D11_DEPTH_STENCIL_DESC dsDesc{ CD3D11_DEFAULT() };
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        dsDesc.StencilEnable = TRUE;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;

        // Create depth stencil state
        DirectX11::ThrowIfFailed(DX::States::Device->CreateDepthStencilState(&dsDesc, _depthStateLessEqual.ReleaseAndGetAddressOf()));

        DirectX::SetName(_depthStateLessEqual.Get(), "Depth Stencil State Less Equal");
    }

}

void LogicalDevice::CreateRasterizerStates()
{
    {
        // Create Solid Rasterizer
        CD3D11_RASTERIZER_DESC2 rasterDesc{ CD3D11_DEFAULT() };
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.AntialiasedLineEnable = FALSE;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateSolid.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateSolid.Get(), "Solid Rasterizer");
    }


    {
        // Create No Cull Rasterizer
        CD3D11_RASTERIZER_DESC2 rasterDesc{ CD3D11_DEFAULT() };
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.AntialiasedLineEnable = TRUE;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateSolidNoCull.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateSolid.Get(), "Solid No Cull Rasterizer");
    }

    {
        // Create No Cull Rasterizer
        CD3D11_RASTERIZER_DESC2 rasterDesc{ CD3D11_DEFAULT() };
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.AntialiasedLineEnable = TRUE;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateSolidFrontCull.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateSolid.Get(), "Solid Front Cull Rasterizer");
    }

    {
        // Create Wireframe Rasterizer
        CD3D11_RASTERIZER_DESC2 rasterDesc{ CD3D11_DEFAULT() };
        rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.AntialiasedLineEnable = TRUE;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateWireframe.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateWireframe.Get(), "Wireframe Rasterizer");
    }

    {
        CD3D11_RASTERIZER_DESC2 rasterDesc{ CD3D11_DEFAULT() };
        rasterDesc.FrontCounterClockwise = TRUE;
        rasterDesc.AntialiasedLineEnable = TRUE;
        rasterDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_ON;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateConservative.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateConservative.Get(), "Conservative Rasterizer");
    }
}

