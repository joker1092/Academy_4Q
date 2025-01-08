#include "SceneRenderer.h"

SceneRenderer::SceneRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources) : deviceResources(deviceResources)
{
	// Create our renering device which includes swapchain
	_device = std::make_unique<LogicalDevice>(deviceResources);
}

SceneRenderer::~SceneRenderer()
{
	_mpso.reset();
	_device.reset();
}

void SceneRenderer::Initialize()
{
	// State object that performs rendering operations in steps
	_mpso = std::make_unique<MPSO>(_device.get());

	// Load cubemap
	Texture2D tex = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\cubemap.dds"));

	// Create and set cubemap to that state object
	_mpso->CreateCubeMap(tex);

}

void SceneRenderer::SetCamera(Camera* camera)
{
	_camera = camera;
}

void SceneRenderer::StagePrepare()
{
#ifdef _DEBUG
	assert(_camera != nullptr);
#endif

	_device->ClearBackbuffer();

	// Craete camera and scene buffers
	DirectX::XMMATRIX proj = _camera->GetProjectionMatrix();
	DirectX::XMMATRIX view = _camera->GetViewMatrix();

	DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
	DirectX::XMStoreFloat3(&pos, _camera->GetPosition());

	CameraBuffer cbuff{
		DirectX::XMMatrixMultiplyTranspose(view, proj),
		pos,
		0,
		DirectX::XMFLOAT3{0.0,0.0f,0.0f},
		0
	};
	SceneBuffer sbuff{};

	sbuff.sunpos = _scene->sunpos;
	sbuff.suncolor = _scene->suncolor;
	sbuff.iblColor = _scene->iblcolor;
	sbuff.iblIntensity = _scene->iblIntensity;
	sbuff.ambientcolor = DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f };
	sbuff.preciseShadows = _scene->moreShadowSamples;

	// Start the pipeline
	_mpso->Prepare(&cbuff, &sbuff);
	_drawmodels.clear();
	_modelcount = 0;
}

void SceneRenderer::EndStage()
{
	// Finish up pipeline
	if (_scene->fxaa)
	{
		FxaaBuffer fxaabuff = {};
		fxaabuff.textureSize = DirectX::XMINT2{ 
			static_cast<int>(_device->GetDeviceResources()->GetLogicalSize().width),
			static_cast<int>(_device->GetDeviceResources()->GetLogicalSize().height)
		};
		fxaabuff.bias = _scene->bias;
		fxaabuff.biasMin = _scene->biasMin;
		fxaabuff.spanMax = _scene->spanMax;

		_mpso->Finish(_scene->gaussianShadowBlur, &fxaabuff);
	}
	else
	{
		_mpso->Finish(_scene->gaussianShadowBlur, nullptr);
	}

	// Set rendering to back buffer for ImGui
	// As it is rendered on top
	_device->SetRenderTargetBackbuffer();
}

