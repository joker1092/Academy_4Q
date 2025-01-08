#include "SceneRenderer.h"
#include "CoreWindow.h"
#include "TextureLoader.h"

SceneRenderer::SceneRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources) :
    m_stateDevice(std::make_unique<StateDevice>(deviceResources))
{
}

SceneRenderer::~SceneRenderer()
{
	m_camera = nullptr;
	m_scene = nullptr;
	m_drawModels.clear();
	m_pso.reset();
}

void SceneRenderer::Initialize()
{
    m_pso = std::make_unique<MeshBasedPSO>(m_stateDevice.get());

	file::path path = PathFinder::Relative("IBL\\");

    path /= "cubemap.dds";

	Texture2D texture = TextureLoader::LoadTextureFromFile(path.string());

	m_pso->CreateCubeMap(texture);
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
	CORE_ASSERT(m_camera);
	CORE_ASSERT(m_scene);
#endif

    m_stateDevice->ClearRenderTargetBackBuffer(m_clearColor);

    Mathf::xMatrix proj = m_camera->GetProjectionMatrix();
    Mathf::xMatrix view = m_camera->GetViewMatrix();

    Mathf::Vector3 pos = { 0.f, 0.f, 0.f };
    pos = m_camera->GetPosition();

    CameraBuffer cameraBuff{
        XMMatrixMultiplyTranspose(view, proj),
        pos,
        0,
        float3(0.f, 0.f, 0.f),
        0,
    };

    SceneBuffer sceneBuff{};
    sceneBuff.SunPos = m_scene->SunPos;
    sceneBuff.SunColor = m_scene->SunColor;
    sceneBuff.IBLColor = m_scene->IBLColor;
    sceneBuff.IBLIntensity = m_scene->IBLIntensity;
    sceneBuff.ambientColor = float3(0.0f, 0.0f, 0.0f);
    sceneBuff.preciseShadow = m_scene->bMoreShadowSamplers;

	m_pso->Prepare(&cameraBuff, &sceneBuff);
	m_drawModels.clear();
}

void SceneRenderer::AddDrawModel(Model* model)
{
	m_drawModels.push_back(model);
}

void SceneRenderer::StageDrawModels()
{
    for (auto&& model : drop(m_drawModels, 1))
    {
        ModelBuffer modelBuff{};
        modelBuff.modelMatrix = XMMatrixTranspose(model->GetMatrix());

        m_pso->SetModelConstants(&modelBuff);

        for (auto&& mesh : model->meshes)
        {
            m_pso->DrawMeshShadows(mesh.m_IndexBuffer, mesh.m_VertexBuffer);
        }
    }

	m_pso->FinishShadows();

    for (auto&& model : m_drawModels)
    {
		ModelBuffer modelBuff{};
		modelBuff.modelMatrix = XMMatrixTranspose(model->GetMatrix());
		m_pso->SetModelConstants(&modelBuff);
		for (auto&& mesh : model->meshes)
		{
			m_pso->DrawMesh(mesh.m_IndexBuffer, mesh.m_VertexBuffer, mesh.m_Material);
		}
    }
	m_pso->Finish();
    m_stateDevice->SetRenderTargetBackBuffer();
}
