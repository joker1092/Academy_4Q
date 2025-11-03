#pragma once
#include "../Utility_Framework/Core.Minimal.h"
#include "D2DBitmapScene.h"

class UIEventManager final : public Singleton<UIEventManager>
{
private:
	friend class Singleton;

	UIEventManager() = default;
	~UIEventManager() = default;

public:
	using UIEventCallback = std::function<void(int)>;

	void Subscribe(const std::string& eventName, UIEventCallback callback) 
	{
		eventListners[eventName].push_back(callback);
	}

	void Publish(const std::string& eventName, int data) 
	{
		if (eventListners.find(eventName) != eventListners.end())
		{
			for (const auto& callback : eventListners[eventName])
			{
				streamExecuter.push_back(std::move([data, &callback]() { callback(data); }));
			}
		}
	}

	void Execute() 
	{
		for (const auto& callback : streamExecuter)
		{
			callback();
		}
		streamExecuter.clear();
	}

private:
	friend class World;
	std::unordered_map<std::string, std::vector<UIEventCallback>> eventListners;
	std::vector<std::function<void()>> streamExecuter;
};

inline static auto& UIEvent = UIEventManager::GetInstance();