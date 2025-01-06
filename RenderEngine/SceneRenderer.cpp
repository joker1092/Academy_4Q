#include "SceneRenderer.h"
#include "CoreWindow.h"
#include "TextureLoader.h"

SceneRenderer::SceneRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
}

SceneRenderer::~SceneRenderer()
{
	m_camera = nullptr;
	m_scene = nullptr;
	m_drawModels.clear();
	m_drawModelCount = 0;
	m_pso.reset();
}

void SceneRenderer::Initialize()
{
	TextureLoader::Initialize(m_deviceResources);

	m_pso = std::make_unique<MeshBasedPSO>(m_deviceResources);

	Texture2D texture = TextureLoader::LoadTextureFromFile("Assets/IBL/cubemap.dds");

	//m_pso->CreateCubeMap(texture);
}

void SceneRenderer::SetCamera(Camera* camera)
{
	m_camera = camera;
}

void SceneRenderer::SetScene(Scene* scene)
{
	m_scene = scene;
}

void SceneRenderer::StagePrepare()
{
#if defined(DEBUG) || defined(_DEBUG)
	CORE_ASSERT(m_camera, "Camera is not set");
	CORE_ASSERT(m_scene, "Scene is not set");
#endif



}

void SceneRenderer::StageSubmit()
{
}

void SceneRenderer::AddDrawModel(Model* model)
{
}

void SceneRenderer::StageDrawModels()
{
}
