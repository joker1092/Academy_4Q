#include "App.h"
#include "InputManager.h"
#include "Utility_Framework/PathFinder.h"
#include "Utility_Framework/Core.Console.hpp"
#include <concrt.h>

#ifdef _DEBUG
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif

MAIN_ENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	PathFinder::Initialize();
	//시작
	Core::App app;
	app.Initialize(hInstance, L"HeroP Editor", 1280, 720);

	return 0;
}

void Core::App::Initialize(HINSTANCE hInstance, const wchar_t* title, int width, int height)
{
	CoreWindow coreWindow(hInstance, L"HeroP Editor", 1280, 720);

	Concurrency::SchedulerPolicy policy
	{
		3,
		Concurrency::MinConcurrency,
		6,
		Concurrency::MaxConcurrency,
		8,
		Concurrency::ContextPriority,
		THREAD_PRIORITY_HIGHEST
	};

	Concurrency::Scheduler::SetDefaultSchedulerPolicy(policy);
	Concurrency::task init = concurrency::create_task([]()
	{
		return std::string("SUCCESS");
	});
	std::string initResult = init.get();

	Console.WriteLine("Multithreaded initialization: {}", initResult);

	m_deviceResources = std::make_shared<DirectX11::DeviceResources>();
	SetWindow(coreWindow);
	InputManagement->Initialize(coreWindow.GetHandle());
	Load();
	Run();
}

void Core::App::SetWindow(CoreWindow& coreWindow)
{
	m_deviceResources->SetWindow(coreWindow);
	coreWindow.RegisterHandler(WM_INPUT, this, &App::ProcessRawInput);
	coreWindow.RegisterHandler(WM_CLOSE, this, &App::Shutdown);
}

void Core::App::Load()
{
	if (nullptr == m_main)
	{
		m_main = std::make_unique<DirectX11::Dx11Main>(m_deviceResources);
	}
}

void Core::App::Run()
{
	CoreWindow::GetForCurrentInstance()->InitializeTask([&]
	{
		// 초기화 작업
	})
	.Then([&]
	{
		// 메인 루프
		m_main->Update();
		if (m_main->Render())
		{
			m_deviceResources->Present();
		}
	});
}

LRESULT Core::App::Shutdown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_windowClosed = true;
	PostQuitMessage(0);
	return 0;
}

LRESULT Core::App::ProcessRawInput(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	InputManagement->ProcessRawInput(lParam);
	return 0;
}
