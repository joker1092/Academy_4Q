#pragma once
#include "ImGuiContext.h"
#include "DeviceResources.h"
#include "ClassProperty.h"
#include "ImGUiRegisterClass.h"

class ImGuiRenderer
{
public:
    ImGuiRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
    ~ImGuiRenderer();
    void Render();
    void Shutdown();

private:
    std::shared_ptr<DirectX11::DeviceResources> m_deviceResources;
};