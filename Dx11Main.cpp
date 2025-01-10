#include "Dx11Main.h"
#include "Utility_Framework/CoreWindow.h"
#include "InputManager.h"
#include "RenderEngine/ImGuiRegister.h"

DirectX11::Dx11Main::Dx11Main(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
{
	m_deviceResources->RegisterDeviceNotify(this);

	//�Ʒ� ������	�ʱ�ȭ �ڵ带 ���⿡ �߰��մϴ�.
	m_sceneRenderer = std::make_unique<SceneRenderer>(m_deviceResources);
	m_imguiRenderer = std::make_unique<ImGuiRenderer>(m_deviceResources);
    AssetsSystem->LoadModels();
    AssetsSystem->LoadShaders();

	m_sceneRenderer->Initialize();

	SceneInitialize();
}

DirectX11::Dx11Main::~Dx11Main()
{
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

void DirectX11::Dx11Main::SceneInitialize()
{
	m_camera = std::make_unique<Camera>(m_deviceResources);
	m_camera->SetPosition(-4.0f, 4.0f, 4.0f);
	m_camera->pitch = -30.f;
	m_camera->yaw = 320.f;

	m_sceneRenderer->SetCamera(m_camera.get());

	m_model = AssetsSystem->Models["sphere"];
	m_model->position = { 0.f, 3.f, 0.f, 0.f };
	m_model->scale = { 0.02f, 0.02f, 0.02f, 0.f };

	m_ground = AssetsSystem->Models["plane"];
	m_ground->scale = { 20.f, 1.f, 20.f, 0.f };
	m_ground->meshes[0].material->properties.roughness = 1.0f;
	ImGui::ContextRegister("Ground material", [&]()
		{
			ImGui::SliderFloat("Ground metalness", &m_ground->meshes[0].material->properties.metalness, 0.01f, 1.f);
			ImGui::SliderFloat("Ground roughnees", &m_ground->meshes[0].material->properties.roughness, 0.01f, 1.f);
		});

	m_scene = std::make_unique<Scene>();
	m_sceneRenderer->SetScene(m_scene.get());
}

void DirectX11::Dx11Main::CreateWindowSizeDependentResources()
{
	//�������� â ũ�⿡ ���� ���ҽ��� �ٽ� ����� �ڵ带 ���⿡ �߰��մϴ�.
	m_deviceResources->ResizeResources();
}

void DirectX11::Dx11Main::Update()
{
	//�������� ������Ʈ �ڵ带 ���⿡ �߰��մϴ�.
	m_timeSystem.Tick([&]
	{
		//�������� ������Ʈ �ڵ带 ���⿡ �߰��մϴ�.
		std::wostringstream woss;
		woss.precision(6);
		woss << L"4Q Graphics Application"
			<< L"Width: "
			<< m_deviceResources->GetOutputSize().width
			<< L" Height: "
			<< m_deviceResources->GetOutputSize().height
			<< L" FPS: "
			<< m_timeSystem.GetFramesPerSecond()
			<< L" FrameCount: "
			<< m_timeSystem.GetFrameCount();

		SetWindowText(m_deviceResources->GetWindow()->GetHandle(), woss.str().c_str());
		//�������� ������Ʈ �ڵ带 ���⿡ �߰��մϴ�.
        m_camera->Update(m_timeSystem.GetElapsedSeconds());
		InputManagement->Update();
	});
}

bool DirectX11::Dx11Main::Render()
{
	// ó�� ������Ʈ�ϱ� ���� �ƹ� �͵� ���������� ������.
	if (m_timeSystem.GetFrameCount() == 0)
	{
		return false;
	}

	m_sceneRenderer->StagePrepare();

	m_sceneRenderer->AddDrawModel(m_ground);
	m_sceneRenderer->AddDrawModel(m_model);

	m_sceneRenderer->StageDrawModels();
    m_sceneRenderer->EndStage();

	m_imguiRenderer->Render();

	return true;
}
// �������� �ʿ��� ����̽� ���ҽ��� �������� �˸��ϴ�.
void DirectX11::Dx11Main::OnDeviceLost()
{

}

// ����̽� ���ҽ��� ���� �ٽ� ������� �� ������ �������� �˸��ϴ�.
void DirectX11::Dx11Main::OnDeviceRestored()
{
	CreateWindowSizeDependentResources();
}
