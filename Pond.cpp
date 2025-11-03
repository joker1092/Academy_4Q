#include "Pond.h"
#include "Player.h"
#include "Pail.h"
#include "../InputManager.h"
#include "SpawnerComponent.h"
#include "RigidbodyComponent.h"
#include "../Physics/Common.h"
#include "../SoundManager.h"

void Pond::Initialize()
{
	Object::Initialize();
}

void Pond::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Pond::Update(float tick)
{
	Object::Update(tick);
}

void Pond::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Pond::Destroy()
{
	Object::Destroy();
}

void Pond::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Pond>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void Pond::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void Pond::OnTriggerEnter(ICollider* other)
{
}

void Pond::OnTriggerStay(ICollider* other)
{
}

void Pond::OnTriggerExit(ICollider* other)
{
}

void Pond::OnCollisionEnter(ICollider* other)
{
}

void Pond::OnCollisionStay(ICollider* other)
{
}

void Pond::OnCollisionExit(ICollider* other)
{
}

bool Pond::IsIgnoreInteract()
{
	return false;
}

void Pond::InteractEnter(Object* thisPlayer, int playerID)
{
	players[playerID].playerID = playerID;
	players[playerID].time = 0.f;

	HighLightOn();
}

void Pond::Interact(Object* thisPlayer, int playerID, float tick)
{
	auto player = static_cast<Player*>(thisPlayer);
	if (player->IsHeldObject() == false) return; // 들고 있는 객체가 없다면 리턴.(추가 처리시 수정)
	
	auto heldObj = player->GetHeldObject();
	if (heldObj->GetType() != MetaType<Pail>::type) return; // 들고 있는 객체가 양동이가 아니라면 리턴.

	if (InputManagement->IsControllerButtonPressed(playerID, ControllerButton::X) || InputManagement->IsKeyPressed('X')) {
		players[playerID].time += tick;
		static_cast<Player*>(thisPlayer)->GetPlayerState().isInterAction = true;
		if (players[playerID].time > maxTimer)
		{
			// action trigger
			Pail* pail = static_cast<Pail*>(heldObj);
			pail->SetWater(true);
			Sound->playSound("Intractn_DrawWater", ChannelType::SFX);
			players[playerID].time = 0.f;
		}
	}
}

void Pond::InteractExit(Object* thisPlayer, int playerID)
{
	players[playerID].time = 0.f;

	HighLightOff();
}

void Pond::EditorContext()
{
	ImGui::Text("Pond");
	ImGui::Spacing();
}

std::string_view Pond::GetType()
{
	return MetaType<Pond>::type;
}
