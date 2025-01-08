#pragma once
#include "DeviceResources.h"
#include "LogicalDevice.h"
#include "Buffers.h"
#include "MPSO.h"
#include "Scene.h"
#include "Primitives.h"

class SceneRenderer
{
public:
	SceneRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
	~SceneRenderer();

	void Initialize();
	void SetCamera(Camera* camera);
	inline void SetScene(Scene* scene) { _scene = scene; };
	// Stages
	void StagePrepare();
    void EndStage();

	inline void PoolModel(const std::shared_ptr<Model>& model)
	{
		_drawmodels.push_back(model);
		_modelcount += 1;
	}
	inline void StageDrawModels()
	{
		// Compute shadow map from Sun view
		for (UINT i = 1; i < _modelcount; i++)
		{
			ModelBuffer modelbuff;
			modelbuff.modelmatrix = DirectX::XMMatrixTranspose(_drawmodels[i]->GetMatrix());

			_mpso->SetModelConstants(&modelbuff);

			for (size_t u = 0; u < _drawmodels[i]->meshes.size(); u++)
			{
				_mpso->DrawMeshShadows(
					_drawmodels[i]->meshes[u].bindex,
					_drawmodels[i]->meshes[u].bvertex
				);
			}
		}

		// Perpare for main rendering
		_mpso->FinishShadows();

		// Draw 
		for (UINT i = 0; i < _modelcount; i++)
		{
			ModelBuffer modelbuff;
			modelbuff.modelmatrix = DirectX::XMMatrixTranspose(_drawmodels[i]->GetMatrix());

			_mpso->SetModelConstants(&modelbuff);

			for (UINT u = 0; u < _drawmodels[i]->meshes.size(); u++)
			{
				_mpso->DrawMesh(
					_drawmodels[i]->meshes[u].bindex,
					_drawmodels[i]->meshes[u].bvertex,
					_drawmodels[i]->meshes[u].material
				);
			}
		}
	}

private:
	Camera*							_camera;
    std::shared_ptr<DirectX11::DeviceResources> deviceResources;
	Scene*							_scene;

	std::vector<std::shared_ptr<Model>> _drawmodels;
	UINT								_modelcount;

	std::unique_ptr<LogicalDevice>	_device;
	std::unique_ptr<MPSO>			_mpso;

};
