#include "Bongsu.h"
#include "Wood.h"
#include "Torch.h"
#include "Pail.h"
#include "Loot.h"
#include "SoundManager.h"
#include "GameManager.h"

void Bongsu::Initialize()
{
	Object::Initialize();
}

void Bongsu::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Bongsu::Update(float tick)
{
	Object::Update(tick);
	_time -= tick;
	if (_time > 0)
	{
		// 불이 켜져 있을 때.
		for (int i = 0; i < 2; i++) {
			if (triggerPlayers[i].use) {
				if (triggerPlayers[i].playerID == 0) {
					auto p1 = GameManagement->GetPlayer1();
					if (p1) {
						p1->AddCold(p1->_coldDecreaseRate * -tick);
					}
				}
				else if (triggerPlayers[i].playerID == 1) {
					auto p2 = GameManagement->GetPlayer2();
					if (p2) {
						p2->AddCold(p2->_coldDecreaseRate * -tick);
					}
				}
			}
		}

		_isFire = 1;
		GameManagement->SetBongsu(BongsuID, 1);
	}
	else 
	{
		_isFire = 0;
		GameManagement->SetBongsu(BongsuID, 0);
		_time = 0;
	}
}

void Bongsu::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Bongsu::Destroy()
{
	Object::Destroy();
}

void Bongsu::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Bongsu>::type;
	data["BongsuID"] = BongsuID;

	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void Bongsu::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);
	if(in.contains("BongsuID"))
	{
		BongsuID = in["BongsuID"].get<int>();
	}
}

void Bongsu::AddMaterial()
{
	_time += _materialTime;
	if (_time > _maxTime)
	{
		_time = _maxTime;
	}
	//
}

void Bongsu::OnTriggerEnter(ICollider* other)
{
	auto otherOwner = other->GetOwner();
	if (otherOwner->GetType() == MetaType<Player>::type) {
		auto player = static_cast<Player*>(otherOwner);
		int playerID = player->playerID;
		player->BongsuHeat(1);
		triggerPlayers[playerID].playerID = playerID;
		triggerPlayers[playerID].use = true;

		if (player->IsHeldObject()) {
			auto held = player->GetHeldObject();
			if (held->GetType() == MetaType<Torch>::type) {
				auto torch = static_cast<Torch*>(held);
				torch->SetFire(true);
			}
		}
	}
}

void Bongsu::OnTriggerStay(ICollider* other)
{
}

void Bongsu::OnTriggerExit(ICollider* other)
{
	auto otherOwner = other->GetOwner();
	if (otherOwner->GetType() == MetaType<Player>::type) {
		auto player = static_cast<Player*>(otherOwner);
		int playerID = player->playerID;
		player->BongsuHeat(-1);
		triggerPlayers[playerID].use = false;
	}
}

void Bongsu::OnCollisionEnter(ICollider* other)
{
}

void Bongsu::OnCollisionStay(ICollider* other)
{
}

void Bongsu::OnCollisionExit(ICollider* other)
{
}
bool Bongsu::IsIgnoreInteract()
{
	return false;
}
void Bongsu::InteractEnter(Object* thisPlayer, int playerID)
{
	HighLightOn();
}
void Bongsu::Interact(Object* thisPlayer, int playerID, float tick)
{
	// 플레이어의 들고있는 타입에 따라 증가량 다르게

	if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::Y) || InputManagement->IsKeyPressed('Y')) {
		auto player = static_cast<Player*>(thisPlayer);
		if (player->IsHeldObject()) {
			auto heldObj = player->GetHeldObject();
			if (heldObj->GetType() == MetaType<Wood>::type) {
				if (_time <= 0.f) {
					Sound->playSound("Bongsu_Ignite", ChannelType::SFX);
				}
				else {
					Sound->playSound("Intractn_Burn", ChannelType::SFX);
				}

				_time += 20.f;
				if (_time > _maxTime) {
					_time = _maxTime;
					// action trigger
				}
				player->SetHeldObject(nullptr);
			}
			else if (heldObj->GetType() == MetaType<Torch>::type) {
				if (_time <= 0.f) {
					Sound->playSound("Bongsu_Ignite", ChannelType::SFX);
				}
				else {
					Sound->playSound("Intractn_Burn", ChannelType::SFX);
				}
				_time += 3.f;
				if (_time > _maxTime) {
					_time = _maxTime;
					// action trigger
				}
				player->SetHeldObject(nullptr);
			}
			else if (heldObj->GetType() == MetaType<Pail>::type) {
				auto held = static_cast<Pail*>(heldObj);
				if (held->IsEmpty() == false) {
					// 물이 있을 때.
					_time = 0.f;
					held->SetWater(false);
					// action trigger.00
					Sound->playSound("Intractn_Extinguish", ChannelType::SFX);

				}
			}
			else if (heldObj->GetType() == MetaType<Loot>::type) {
				// 털뭉치 처리.

				player->SetHeldObject(nullptr);
			}
			else {
				player->SetHeldObject(nullptr);
			}

			heldObj->BornItem();
		}
	}
}
void Bongsu::InteractExit(Object* thisPlayer, int playerID)
{
	HighLightOff();
}
std::string_view Bongsu::GetType()
{
	return MetaType<Bongsu>::type; 
}

void Bongsu::EditorContext()
{
	ImGui::DragInt("Bongsu ID", &BongsuID);
	ImGui::Spacing();
}
