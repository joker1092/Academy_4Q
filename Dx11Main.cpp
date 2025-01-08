#include "Dx11Main.h"
#include "Utility_Framework/CoreWindow.h"
#include "InputManager.h"

DirectX11::Dx11Main::Dx11Main(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
{
	m_deviceResources->RegisterDeviceNotify(this);


	//아래 렌더러	초기화 코드를 여기에 추가합니다.
	m_sceneRenderer = std::make_unique<SceneRenderer>(m_deviceResources);
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
	m_model->scale = { 0.02f };

	m_ground = AssetsSystem->Models["plane"];

	m_scene = std::make_unique<Scene>();

	m_scene->suncolor = { 1.f, 1.f, 0.96f };
	m_scene->sunpos = { 3.f, 10.f, 3.f };
	m_scene->iblIntensity = 0.1f;
	m_scene->iblcolor = { 1.f, 1.f, 1.f };
    m_scene->moreShadowSamples = true;
    m_scene->gaussianShadowBlur = false;

	m_sceneRenderer->SetScene(m_scene.get());
}

void DirectX11::Dx11Main::CreateWindowSizeDependentResources()
{
	//렌더러의 창 크기에 따라 리소스를 다시 만드는 코드를 여기에 추가합니다.
}

void DirectX11::Dx11Main::Update()
{
	//렌더러의 업데이트 코드를 여기에 추가합니다.
	m_timeSystem.Tick([&]
	{
		//렌더러의 업데이트 코드를 여기에 추가합니다.
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
		//렌더러의 업데이트 코드를 여기에 추가합니다.
        m_camera->Update(m_timeSystem.GetElapsedSeconds());
	});
}

bool DirectX11::Dx11Main::Render()
{
	// 처음 업데이트하기 전에 아무 것도 렌더링하지 마세요.
	if (m_timeSystem.GetFrameCount() == 0)
	{
		return false;
	}

	m_sceneRenderer->StagePrepare();

	m_sceneRenderer->PoolModel(m_ground);
	m_sceneRenderer->PoolModel(m_model);

	m_sceneRenderer->StageDrawModels();
    m_sceneRenderer->EndStage();

	return true;
}
// 릴리스가 필요한 디바이스 리소스를 렌더러에 알립니다.
void DirectX11::Dx11Main::OnDeviceLost()
{
}

// 디바이스 리소스가 이제 다시 만들어질 수 있음을 렌더러에 알립니다.
void DirectX11::Dx11Main::OnDeviceRestored()
{
	CreateWindowSizeDependentResources();
}
