#include "LogicalDevice.h"
#include "DXStates.h"

LogicalDevice::LogicalDevice(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources) : deviceResources(deviceResources)
{
    DX::States::Device = deviceResources->GetD3DDevice();
    DX::States::Context = deviceResources->GetD3DDeviceContext();

    CreateDepthState();
    CreateRasterizerStates();
}

LogicalDevice::~LogicalDevice()
{

}

void LogicalDevice::CreateDepthState()
{

    {
        // Create Depth Stencil
        D3D11_DEPTH_STENCIL_DESC dsDesc;

        // Depth test parameters
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

        // Stencil test parameters
        dsDesc.StencilEnable = true;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create depth stencil state
        DirectX11::ThrowIfFailed(DX::States::Device->CreateDepthStencilState(&dsDesc, _depthStateLess.ReleaseAndGetAddressOf()));

        DirectX::SetName(_depthStateLess.Get(), "Depth Stencil State Less");
    }

    {
        // Create Depth Stencil
        D3D11_DEPTH_STENCIL_DESC dsDesc;

        // Depth test parameters
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        // Stencil test parameters
        dsDesc.StencilEnable = true;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create depth stencil state
        DirectX11::ThrowIfFailed(DX::States::Device->CreateDepthStencilState(&dsDesc, _depthStateLessEqual.ReleaseAndGetAddressOf()));

        DirectX::SetName(_depthStateLessEqual.Get(), "Depth Stencil State Less Equal");
    }

}

void LogicalDevice::CreateRasterizerStates()
{
    {
		// Create Solid Rasterizer
		D3D11_RASTERIZER_DESC2 rasterDesc;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FrontCounterClockwise = 1;
		rasterDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
		rasterDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterDesc.DepthClipEnable = 1;
		rasterDesc.ScissorEnable = 0;
		rasterDesc.MultisampleEnable = 0;
		rasterDesc.AntialiasedLineEnable = 1;
		rasterDesc.ForcedSampleCount = 0;
		rasterDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateSolid.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateSolid.Get(), "Solid Rasterizer");
	}


    {
        // Create No Cull Rasterizer
        D3D11_RASTERIZER_DESC2 rasterDesc;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FrontCounterClockwise = 1;
        rasterDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        rasterDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterDesc.DepthClipEnable = 1;
        rasterDesc.ScissorEnable = 0;
        rasterDesc.MultisampleEnable = 0;
        rasterDesc.AntialiasedLineEnable = 1;
        rasterDesc.ForcedSampleCount = 0;
        rasterDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateSolidNoCull.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateSolid.Get(), "Solid No Cull Rasterizer");
    }

    {
        // Create No Cull Rasterizer
        D3D11_RASTERIZER_DESC2 rasterDesc;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FrontCounterClockwise = 1;
        rasterDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        rasterDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterDesc.DepthClipEnable = 1;
        rasterDesc.ScissorEnable = 0;
        rasterDesc.MultisampleEnable = 0;
        rasterDesc.AntialiasedLineEnable = 1;
        rasterDesc.ForcedSampleCount = 0;
        rasterDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateSolidFrontCull.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateSolid.Get(), "Solid Front Cull Rasterizer");
    }
    
    {
        // Create Wireframe Rasterizer
        D3D11_RASTERIZER_DESC2 rasterDesc;
        rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FrontCounterClockwise = 1;
        rasterDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        rasterDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterDesc.DepthClipEnable = 1;
        rasterDesc.ScissorEnable = 0;
        rasterDesc.MultisampleEnable = 0;
        rasterDesc.AntialiasedLineEnable = 1;
        rasterDesc.ForcedSampleCount = 0;
        rasterDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateWireframe.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateWireframe.Get(), "Wireframe Rasterizer");
    }


    // Might not be supported on old GPU's
    // Supported from Maxwell and Skylake architecture onwards
    //if (Options::Graphics::ConservativeRasterization)
    //{
        // Create Conservative Rasterizer
        D3D11_RASTERIZER_DESC2 rasterDesc;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FrontCounterClockwise = 1;
        rasterDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        rasterDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterDesc.DepthClipEnable = 1;
        rasterDesc.ScissorEnable = 0;
        rasterDesc.MultisampleEnable = 0;
        rasterDesc.AntialiasedLineEnable = 1;
        rasterDesc.ForcedSampleCount = 0;
        rasterDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_ON;

        DirectX11::ThrowIfFailed(DX::States::Device->CreateRasterizerState2(&rasterDesc, _rasterizerStateConservative.ReleaseAndGetAddressOf()));
        DirectX::SetName(_rasterizerStateConservative.Get(), "Conservative Rasterizer");
    //}
}

