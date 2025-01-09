#include "ImGuiRenderer.h"
#include "CoreWindow.h"

ImGuiRenderer::ImGuiRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources)
{
    //아래 렌더러	초기화 코드를 여기에 추가합니다.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_deviceResources->GetWindow()->GetHandle());
    ID3D11Device* device = m_deviceResources->GetD3DDevice();
    ID3D11DeviceContext* deviceContext = m_deviceResources->GetD3DDeviceContext();
    ImGui_ImplDX11_Init(device, deviceContext);
}

ImGuiRenderer::~ImGuiRenderer()
{
    Shutdown();
}

void ImGuiRenderer::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    ImGui::NewFrame();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    auto& container = ImGuiRegister::GetInstance()->m_contexts;

    for (auto& [name, context] : container)
    {
        context.Render();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRenderer::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
