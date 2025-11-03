#include "Drum.h"
#include "Player.h"
#include "../InputManager.h"
#include "../Physics/Common.h"
#include "../SoundManager.h"
#include "../GameManager.h"

void Drum::Initialize()
{
	Object::Initialize();
}

void Drum::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Drum::Update(float tick)
{
	Object::Update(tick);
}

void Drum::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Drum::Destroy()
{
	Object::Destroy();
}

void Drum::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Drum>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void Drum::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);
}

void Drum::OnTriggerEnter(ICollider* other)
{
}

void Drum::OnTriggerStay(ICollider* other)
{
}

void Drum::OnTriggerExit(ICollider* other)
{
}

void Drum::OnCollisionEnter(ICollider* other)
{
}

void Drum::OnCollisionStay(ICollider* other)
{
}

void Drum::OnCollisionExit(ICollider* other)
{
}

bool Drum::IsIgnoreInteract()
{
	return false;
}

void Drum::InteractEnter(Object* thisPlayer, int playerID)
{
	players[playerID].playerID = playerID;
	players[playerID].value.time = 0.f;
	players[playerID].use = true;

	HighLightOn();
}

void Drum::Interact(Object* thisPlayer, int playerID, float tick)
{
	// 둘중 하나만 작동하게 걸러야하는데...

	if (InputManagement->IsControllerButtonPressed(playerID, ControllerButton::X) || InputManagement->IsKeyPressed('X')) {
		static_cast<Player*>(thisPlayer)->GetPlayerState().isDrum = true;
		float prevTime = players[playerID].value.time;

		players[playerID].value.time += tick;

		if (players[playerID].value.time > maxTimer)
		{
			Sound->playSound("Intractn_BeatDrum",ChannelType::SFX);
			GameManagement->PlayDrumBeat();
			players[playerID].value.time = 0.f;
		}
	}
}

void Drum::InteractExit(Object* thisPlayer, int playerID)
{
	players[playerID].value.time = 0.f;
	players[playerID].use = false;

	HighLightOff();
}

void Drum::EditorContext()
{
	ImGui::Text("Drum");
	ImGui::Spacing();
}

std::string_view Drum::GetType()
{
	return MetaType<Drum>::type; 
}

