#include "SceneRenderer.h"
#include "Banchmark.hpp"

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
	Texture2D tex = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_EnvHDR.dds"));
	Texture2D tex2 = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_DiffuseHDR.dds"));
	Texture2D tex3 = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_SpecularHDR.dds"));
	Texture2D tex4 = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_Brdf.dds"));
	// Create and set cubemap to that state object
	_mpso->CreateCubeMap(tex);
	_mpso->CreateIBL(tex2, tex3, tex4);
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
	//여기서 컬링하고, 렌더링할 모델을 정합니다.
	_device->ClearBackbuffer();

	// Craete camera and scene buffers
	DirectX::XMMATRIX proj = _camera->GetProjectionMatrix();
	DirectX::XMMATRIX view = _camera->GetViewMatrix();

	float3 pos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMStoreFloat3(&pos, _camera->GetPosition());

	CameraBuffer cbuff
	{
		DirectX::XMMatrixMultiplyTranspose(view, proj),
		pos,
		0,
		float3{ 0.0, 0.0f, 0.0f },
		0
	};
	SceneBuffer sbuff{};

	sbuff.sunpos = _scene->sunpos;
	sbuff.suncolor = _scene->suncolor;
	sbuff.iblColor = _scene->iblcolor;
	sbuff.iblIntensity = _scene->iblIntensity;
	sbuff.ambientcolor = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	sbuff.preciseShadows = _scene->moreShadowSamples;

	// Start the pipeline
	_mpso->Prepare(&cbuff, &sbuff);
	_drawmodels.clear();
	_modelcount = 0;
}

void SceneRenderer::StageDrawModels()
{
	// Compute shadow map from Sun view
	foreach(drop(_drawmodels, 1), [this](auto&& ins_model) 
	{
		if(ins_model.model)
		{
			ModelBuffer modelBuff{};
			modelBuff.modelmatrix = XMMatrixTranspose(ins_model.GetMatrix());
			_mpso->SetModelConstants(&modelBuff);

			for (auto&& mesh : ins_model.model->meshes)
			{
				_mpso->DrawMeshShadows(mesh.bindex, mesh.bvertex);
			}
		}
	});

	_mpso->SetAnimeShadowsShader();

	foreach(drop(_drawmodels, 1), [this](auto&& ins_model)
	{
		if (ins_model.animModel)
		{
			ModelBuffer modelBuff{};
			modelBuff.modelmatrix = XMMatrixTranspose(ins_model.GetMatrix());
			_mpso->SetModelConstants(&modelBuff);

			for (auto&& mesh : ins_model.animModel->meshes)
			{
				_mpso->DrawMeshShadows(mesh.bindex, mesh.bvertex);
			}
		}
	});

	_mpso->FinishShadows();

	foreach(_drawmodels, [this](auto&& ins_model)
	{
		if (ins_model.model)
		{
			ModelBuffer modelBuff{};
			modelBuff.modelmatrix = XMMatrixTranspose(ins_model.GetMatrix());
			_mpso->SetModelConstants(&modelBuff);

			for (auto&& mesh : ins_model.model->meshes)
			{
				_mpso->DrawMesh(mesh.bindex, mesh.bvertex, mesh.material);
			}
		}
	});

	_mpso->SetAnimeShader();

	foreach(_drawmodels, [this](auto&& ins_model)
		{
			if (ins_model.animModel)
			{
				ModelBuffer modelBuff{};
				modelBuff.modelmatrix = XMMatrixTranspose(ins_model.GetMatrix());
				_mpso->SetModelConstants(&modelBuff);

				for (auto&& mesh : ins_model.animModel->meshes)
				{
					_mpso->DrawMesh(mesh.bindex, mesh.bvertex, mesh.material);
				}
			}
		});
}

void SceneRenderer::EndStage()
{
	// Finish up pipeline
	if (_scene->fxaa)
	{
		FxaaBuffer fxaabuff = {};
		fxaabuff.textureSize = int2
		{ 
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

void SceneRenderer::AddDrawModel(const InstancedModel& model)
{
	_drawmodels.push_back(model);
	_modelcount += 1;
}

void SceneRenderer::UpdateDrawModel()
{
	foreach(_scene->ModelsData, [this](auto&& ins_model)
	{
		//TODO : 시간이 된다면 여기서 컬링을 하고, 렌더링할 모델을 정합니다.(안될거 같습니다.)
		_drawmodels.push_back(ins_model);
		_modelcount += 1;
	});
}

