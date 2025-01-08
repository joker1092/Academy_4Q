#include "StateDevice.h"

void StateDevice::SetRenderTarget()
{
    ID3D11RenderTargetView* views[]{ nullptr, nullptr };

    m_DeviceContext->OMSetRenderTargets(2, views, nullptr);
    m_DeviceContext->RSSetViewports(1, &m_viewPort);
}

void StateDevice::SetRenderTarget(const RenderTarget* target)
{
    ID3D11RenderTargetView* views[]{ target->GetRTV() };

    m_DeviceContext->OMSetRenderTargets(1, views, target->GetDSV());
    m_DeviceContext->RSSetViewports(1, &m_viewPort);
}

void StateDevice::SetRenderTarget(const RenderTarget* mainTarget, const RenderTarget* second)
{
    ID3D11RenderTargetView* views[]{ mainTarget->GetRTV(), second->GetRTV() };

    m_DeviceContext->OMSetRenderTargets(2, views, mainTarget->GetDSV());
    m_DeviceContext->RSSetViewports(1, &m_viewPort);
}

void StateDevice::SetRenderTargetBackBuffer()
{
    ID3D11RenderTargetView* views[]{ m_RenderTargetViewBackBuffer, nullptr };

    m_DeviceContext->OMSetRenderTargets(2, views, m_DepthStencilViewMain);
    m_DeviceContext->RSSetViewports(1, &m_viewPort);
}

void StateDevice::SetDepthStencilState(DepthStencilType type)
{
    switch (type)
    {
    case DepthStencilType::Less:
        m_DeviceContext->OMSetDepthStencilState(m_DepthStencilStateLess.Get(), 0);
        break;
    case DepthStencilType::LessEqual:
        m_DeviceContext->OMSetDepthStencilState(m_DepthStencilStateLessEqual.Get(), 0);
        break;
    }
}

void StateDevice::SetRasterizerState(RasterizerType type)
{
    switch (type)
    {
    case RasterizerType::Solid:
        m_DeviceContext->RSSetState(m_RasterizerStateSolid.Get());
        break;
    case RasterizerType::SolidNoCull:
        m_DeviceContext->RSSetState(m_RasterizerStateSolidNoCull.Get());
        break;
    case RasterizerType::SolidFrontCull:
        m_DeviceContext->RSSetState(m_RasterizerStateSolidFrontCull.Get());
        break;
    case RasterizerType::Wireframe:
        m_DeviceContext->RSSetState(m_RasterizerStateWireframe.Get());
        break;
    //case RasterizerType::Conservative:
    //    m_DeviceContext->RSSetState(m_RasterizerStateConservative.Get());
    //    break;
    }
}

void StateDevice::DrawIndexed(const uint32 indexCount, const uint32 startIndexLocation, const int32 baseVertexLocation)
{
    m_DeviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void StateDevice::Draw(const uint32 vertexCount, const uint32 startVertexLocation)
{
    m_DeviceContext->Draw(vertexCount, startVertexLocation);
}

void StateDevice::DrawInstanced(const uint32 vertexCountPerInstance, const uint32 instanceCount, const uint32 startVertexLocation, const uint32 startInstanceLocation)
{
    m_DeviceContext->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void StateDevice::DrawIndexedInstanced(const uint32 indexCountPerInstance, const uint32 instanceCount, const uint32 startIndexLocation, const int32 baseVertexLocation, const uint32 startInstanceLocation)
{
    m_DeviceContext->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void StateDevice::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
    m_DeviceContext->IASetPrimitiveTopology(topology);
}

void StateDevice::ClearRenderTargetBackBuffer(DirectX::XMVECTORF32 clearColor)
{
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetViewBackBuffer, clearColor);
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilViewMain, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void StateDevice::CreateDepthState()
{
    CD3D11_DEPTH_STENCIL_DESC depthStencilDesc{
        true,
        D3D11_DEPTH_WRITE_MASK_ALL,
        D3D11_COMPARISON_LESS,
        true,
        0xFF,
        0xFF,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_INCR,
        D3D11_COMPARISON_ALWAYS,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_DECR,
        D3D11_COMPARISON_ALWAYS
    };

    DirectX11::ThrowIfFailed(
        m_Device->CreateDepthStencilState(
            &depthStencilDesc,
            m_DepthStencilStateLess.GetAddressOf()
        )
    );
    DirectX::SetName(m_DepthStencilStateLess.Get(), "Depth_Stencil_State_Less");

    CD3D11_DEPTH_STENCIL_DESC depthStencilDescEqual{
        true,
        D3D11_DEPTH_WRITE_MASK_ALL,
        D3D11_COMPARISON_LESS_EQUAL,
        true,
        0xFF,
        0xFF,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_INCR,
        D3D11_COMPARISON_ALWAYS,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_DECR,
        D3D11_COMPARISON_ALWAYS
    };

    DirectX11::ThrowIfFailed(
        m_Device->CreateDepthStencilState(
            &depthStencilDescEqual,
            m_DepthStencilStateLessEqual.GetAddressOf()
        )
    );
    DirectX::SetName(m_DepthStencilStateLessEqual.Get(), "Depth_Stencil_State_Less_Equal");
}

void StateDevice::CreateRasterizerState()
{
    CD3D11_RASTERIZER_DESC rasterizerDescSolid{
        D3D11_FILL_SOLID,
        D3D11_CULL_BACK,
        true,
        D3D11_DEFAULT_DEPTH_BIAS,
        D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        true,
        false,
        false,
        true,
    };

    DirectX11::ThrowIfFailed(
        m_Device->CreateRasterizerState(
            &rasterizerDescSolid,
            m_RasterizerStateSolid.GetAddressOf()
        )
    );
    DirectX::SetName(m_RasterizerStateSolid.Get(), "Rasterizer_State_Solid");

    CD3D11_RASTERIZER_DESC rasterizerDescSolidNoCull{
        D3D11_FILL_SOLID,
        D3D11_CULL_NONE,
        true,
        D3D11_DEFAULT_DEPTH_BIAS,
        D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        true,
        false,
        false,
        true,
    };

    DirectX11::ThrowIfFailed(
        m_Device->CreateRasterizerState(
            &rasterizerDescSolidNoCull,
            m_RasterizerStateSolidNoCull.GetAddressOf()
        )
    );
    DirectX::SetName(m_RasterizerStateSolidNoCull.Get(), "Rasterizer_State_Solid_No_Cull");

    CD3D11_RASTERIZER_DESC rasterizerDescSolidFrontCull{
        D3D11_FILL_SOLID,
        D3D11_CULL_FRONT,
        true,
        D3D11_DEFAULT_DEPTH_BIAS,
        D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        true,
        false,
        false,
        true,
    };

    DirectX11::ThrowIfFailed(
        m_Device->CreateRasterizerState(
            &rasterizerDescSolidFrontCull,
            m_RasterizerStateSolidFrontCull.GetAddressOf()
        )
    );
    DirectX::SetName(m_RasterizerStateSolidFrontCull.Get(), "Rasterizer_State_Solid_Front_Cull");

    CD3D11_RASTERIZER_DESC rasterizerDescWireframe{
        D3D11_FILL_WIREFRAME,
        D3D11_CULL_BACK,
        true,
        D3D11_DEFAULT_DEPTH_BIAS,
        D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
        D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
        true,
        false,
        false,
        true,
    };

    DirectX11::ThrowIfFailed(
        m_Device->CreateRasterizerState(
            &rasterizerDescWireframe,
            m_RasterizerStateWireframe.GetAddressOf()
        )
    );
    DirectX::SetName(m_RasterizerStateWireframe.Get(), "Rasterizer_State_Wireframe");

    //CD3D11_RASTERIZER_DESC rasterizerDescConservative{
    //    D3D11_FILL_SOLID,
    //    D3D11_CULL_NONE,
    //    true,
    //    D3D11_DEFAULT_DEPTH_BIAS,
    //    D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
    //    D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
    //    true,
    //    true,
    //    false,
    //    true,
    //};

    //DirectX11::ThrowIfFailed(
    //    m_Device->CreateRasterizerState(
    //        &rasterizerDescConservative,
    //        m_RasterizerStateConservative.GetAddressOf()
    //    )
    //);

    //DirectX::SetName(m_RasterizerStateConservative.Get(), "Rasterizer_State_Conservative");


}
