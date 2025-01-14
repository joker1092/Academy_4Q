#pragma once
#include "ImGUiRegisterClass.h"

namespace ImGui
{
    static inline void ContextRegister(const std::string& name, std::function<void()> function)
    {
        ImGuiRegister::GetInstance()->Register(name, function);
    }

	static inline void ContextRegister(const std::string& name, std::function<void()> function, ImGuiWindowFlags flags)
	{
		ImGuiRegister::GetInstance()->Register(name, function, flags);
	}

    static inline void ContextRegister(const std::string& name, const std::string& subName, std::function<void()> function)
    {
        ImGuiRegister::GetInstance()->Register(name, subName, function);
    }

	static inline void ContextRegister(const std::string& name, const std::string& subName, std::function<void()> function, ImGuiWindowFlags flags)
	{
		ImGuiRegister::GetInstance()->Register(name, subName, function, flags);
	}

    static inline void ContextUnregister(const std::string& name)
    {
        ImGuiRegister::GetInstance()->Unregister(name);
    }

    static inline void ContextUnregister(const std::string& name, const std::string& subName)
    {
        ImGuiRegister::GetInstance()->Unregister(name, subName);
    }
}
