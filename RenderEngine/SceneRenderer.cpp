#include "SceneRenderer.h"
#include "Banchmark.hpp"
#include "../MeshEditor.h"
#include "PXDebugSystem.h"

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
	Texture2D tex = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_Brdf.dds"));
	Texture2D tex2 = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_DiffuseHDR.dds"));
	Texture2D tex3 = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_EnvHDR.dds"));
	Texture2D tex4 = TextureLoader::LoadCubemapFromFile(PathFinder::Relative("IBL\\puresky_SpecularHDR.dds"));

	// Create and set cubemap to that state object
	_mpso->CreateCubeMap(tex2);
	_mpso->CreateIBL(tex2, tex3, tex);
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
	float3 up{};
	DirectX::XMStoreFloat3(&up, _camera->GetUp());
	float3 right{};
	DirectX::XMStoreFloat3(&right, _camera->GetRight());
	float3 viewDir{};
	DirectX::XMStoreFloat3(&viewDir, _camera->GetViewPosition());
	float3 pos{};
	DirectX::XMStoreFloat3(&pos, _camera->GetPosition());

	CameraBuffer cbuff
	{
		DirectX::XMMatrixMultiplyTranspose(view, proj),
		pos,
		_camera->exposure,
		viewDir,
		0,
		up,
		0,
		right,
		0
	};
	SceneBuffer sbuff{};

	sbuff.sunpos = _scene->sunpos;
	sbuff.suncolor = _scene->suncolor;
	sbuff.iblColor = _scene->iblcolor;
	sbuff.iblIntensity = _scene->iblIntensity;
	sbuff.ambientcolor = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	sbuff.preciseShadows = _scene->moreShadowSamples;
	sbuff.numPointLights = _scene->numPointLights;
	for (uint32 i = 0; i < _scene->numPointLights; i++)
	{
		sbuff.pointLights[i] = _scene->pointLights[i].buffer;
	}

	// Start the pipeline
	_mpso->Prepare(&cbuff, &sbuff);
	_drawmodels.clear();
	_billboards.clear();
	_modelcount = 0;
	_billboardcount = 0;
}

void SceneRenderer::StageDrawModels()
{
	// Compute shadow map from Sun view
	if(isEditor = _scene->isEditorMode, isEditor)
	{
		foreach(drop(_drawmodels, 1), [this](auto&& ins_model)
			{
				if (ins_model.model)
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
					if (ins_model.animModel->animator->_currentAnimation)
					{
						JointBuffer jointBuff{ ins_model.animModel->GetFinalBoneTransforms() };
						_mpso->SetAnimeConstants(&jointBuff);

						for (auto&& mesh : ins_model.animModel->meshes)
						{
							_mpso->DrawMeshShadows(mesh.bindex, mesh.bvertex);
						}
					}
					else
					{
						Log::Error("No animation is playing");
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

		StageEditModel();

		_mpso->SetAnimeShader();

		foreach(_drawmodels, [this](auto&& ins_model)
			{
				if (ins_model.animModel)
				{
					if (ins_model.animModel->animator->_currentAnimation)
					{
						JointBuffer jointBuff{ ins_model.animModel->GetFinalBoneTransforms() };
						_mpso->SetAnimeConstants(&jointBuff);

						for (auto&& mesh : ins_model.animModel->meshes)
						{
							_mpso->DrawMesh(mesh.bindex, mesh.bvertex, mesh.material);
						}
					}
					else
					{
						Log::Error("No animation is playing");
					}
				}
			});

		_mpso->SetOutlineShader();

		foreach(_drawmodels, [this](auto&& ins_model)
			{
				if (ins_model.model)
				{
					ModelBuffer modelBuff{};
					modelBuff.modelmatrix = XMMatrixTranspose(ins_model.GetMatrix());
					_mpso->SetModelConstants(&modelBuff);
					for (auto&& mesh : ins_model.model->meshes)
					{
						_mpso->DrawOutline(mesh.bindex, mesh.bvertex, ins_model.outline);
					}
				}
			});

		_mpso->SetOutlineAnimShader();

		foreach(_drawmodels, [this](auto&& ins_model)
			{
				if (ins_model.animModel)
				{
					if (ins_model.animModel->animator->_currentAnimation)
					{
						JointBuffer jointBuff{ ins_model.animModel->GetFinalBoneTransforms() };
						_mpso->SetAnimeOutlineConstants(&jointBuff);
						for (auto&& mesh : ins_model.animModel->meshes)
						{
							_mpso->DrawOutline(mesh.bindex, mesh.bvertex, ins_model.outline);
						}
					}
					else
					{
						Log::Error("No animation is playing");
					}
				}
			});

		_scene->EditorOutline();
	}
	else
	{
		foreach(_drawmodels, [this](auto&& ins_model)
			{
				if (ins_model.model)
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

		foreach(_drawmodels, [this](auto&& ins_model)
			{
				if (ins_model.animModel)
				{
					if (ins_model.animModel->animator->_currentAnimation)
					{
						JointBuffer jointBuff{ ins_model.animModel->GetFinalBoneTransforms() };
						_mpso->SetAnimeConstants(&jointBuff);

						for (auto&& mesh : ins_model.animModel->meshes)
						{
							_mpso->DrawMeshShadows(mesh.bindex, mesh.bvertex);
						}
					}
					else
					{
						Log::Error("No animation is playing");
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

		//StageEditModel();

		_mpso->SetAnimeShader();

		foreach(_drawmodels, [this](auto&& ins_model)
			{
				if (ins_model.animModel)
				{
					if (ins_model.animModel->animator->_currentAnimation)
					{
						JointBuffer jointBuff{ ins_model.animModel->GetFinalBoneTransforms() };
						_mpso->SetAnimeConstants(&jointBuff);

						for (auto&& mesh : ins_model.animModel->meshes)
						{
							_mpso->DrawMesh(mesh.bindex, mesh.bvertex, mesh.material);
						}
					}
					else
					{
						Log::Error("No animation is playing");
					}
				}
			});

		_mpso->SetOutlineShader();

		foreach(_drawmodels, [this](auto&& ins_model)
			{
				if (ins_model.model)
				{
					ModelBuffer modelBuff{};
					modelBuff.modelmatrix = XMMatrixTranspose(ins_model.GetMatrix());
					_mpso->SetModelConstants(&modelBuff);
					for (auto&& mesh : ins_model.model->meshes)
					{
						_mpso->DrawOutline(mesh.bindex, mesh.bvertex, ins_model.outline);
					}
				}
			});

		_mpso->SetOutlineAnimShader();

		foreach(_drawmodels, [this](auto&& ins_model)
		{
			if (ins_model.animModel)
			{
				if (ins_model.animModel->animator->_currentAnimation)
				{
					JointBuffer jointBuff{ ins_model.animModel->GetFinalBoneTransforms() };
					_mpso->SetAnimeOutlineConstants(&jointBuff);
					for (auto&& mesh : ins_model.animModel->meshes)
					{
						_mpso->DrawOutline(mesh.bindex, mesh.bvertex, ins_model.outline);
					}
				}
				else
				{
					Log::Error("No animation is playing");
				}
			}
		});
	}
}

void SceneRenderer::SetEditModel(const std::shared_ptr<Model>& model)
{
	if (model && _meshEditModel != model && model->isLoaded)
	{
		_meshEditModel = model;
	}
}

const std::unique_ptr<RenderTarget>& SceneRenderer::GetMeshEditorTarget() const
{
	return _mpso->GetMeshEditorTarget();
}

void SceneRenderer::StageEditModel()
{
	if (MeshEditorSystem->show)
	{
		_mpso->SetMeshEditorTarget();
		if (_meshEditModel)
		{
			ModelBuffer modelBuff{};
			Mathf::Vector3 scale = { 1.0f, 1.0f, 1.0f };

			modelBuff.modelmatrix = XMMatrixTranspose(XMMatrixScaling(scale.x, scale.y, scale.z));
			_mpso->SetModelConstants(&modelBuff);
			for (auto&& mesh : _meshEditModel->meshes)
			{
				_mpso->DrawMesh(mesh.bindex, mesh.bvertex, mesh.material);
			}
		}
		_mpso->EndMeshEditorTarget();
	}
}

void SceneRenderer::AddBillboard(const InstancedBillboard& billboard)
{
	_billboards.push_back(billboard);
	_billboardcount += 1;
}

void SceneRenderer::UpdateDrawBillboards()
{
	foreach(_scene->billboards, [this](auto&& billboard)
	{
		if (billboard._isVisible)
		{
			_billboards.push_back(billboard);
			_billboardcount += 1;
		}
	});
}

void SceneRenderer::StageBillboardsPrepare()
{
	DirectX::XMMATRIX proj = _camera->GetProjectionMatrix();
	DirectX::XMMATRIX view = _camera->GetViewMatrix();

	float3 pos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMStoreFloat3(&pos, _camera->GetPosition());
	float3 viewDir{};
	DirectX::XMStoreFloat3(&viewDir, _camera->GetViewPosition());
	float3 up{};
	DirectX::XMStoreFloat3(&up, _camera->GetUp());
	float3 right{};
	DirectX::XMStoreFloat3(&right, _camera->GetRight());

	CameraBuffer cbuff
	{
		DirectX::XMMatrixMultiplyTranspose(view, proj),
		pos,
		_camera->exposure,
		viewDir,
		0,
		up,
		0,
		right,
		0
	};

	_mpso->SetBillboardShader(&cbuff);
}

void SceneRenderer::StageDrawBillboards()
{
	foreach(_billboards, [this](auto&& billboard)
	{
		_mpso->DrawBillboard(billboard);
	});

	_mpso->FinishBillboardsShader();
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
		if (ins_model.isVisiable)
		{
			_drawmodels.push_back(ins_model);
			_modelcount += 1;
		}
	});
}

