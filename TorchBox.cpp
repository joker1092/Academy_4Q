#include "TorchBox.h"
#include "Player.h"
#include "../InputManager.h"
#include "SpawnerComponent.h"
#include "RigidbodyComponent.h"
#include "../Physics/Common.h"
#include "Core.Random.h"
#include "SoundManager.h"
void TorchBox::Initialize()
{
	Object::Initialize();
}

void TorchBox::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void TorchBox::Update(float tick)
{
	Object::Update(tick);
}

void TorchBox::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void TorchBox::Destroy()
{
	Object::Destroy();
}

void TorchBox::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<TorchBox>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void TorchBox::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void TorchBox::OnTriggerEnter(ICollider* other)
{
}

void TorchBox::OnTriggerStay(ICollider* other)
{
}

void TorchBox::OnTriggerExit(ICollider* other)
{
}

void TorchBox::OnCollisionEnter(ICollider* other)
{
}

void TorchBox::OnCollisionStay(ICollider* other)
{
}

void TorchBox::OnCollisionExit(ICollider* other)
{
}

bool TorchBox::IsIgnoreInteract()
{
	return false;
}

void TorchBox::InteractEnter(Object* thisPlayer, int playerID)
{
	players[playerID].playerID = playerID;
	players[playerID].time = 0.f;

	HighLightOn();
}

void TorchBox::Interact(Object* thisPlayer, int playerID, float tick)
{
	if (InputManagement->IsControllerButtonPressed(playerID, ControllerButton::X) || InputManagement->IsKeyPressed('X')) {
		players[playerID].time += tick;
		static_cast<Player*>(thisPlayer)->GetPlayerState().isInterAction = true;

		if (players[playerID].time > maxTimer) 
		{
			Sound->playSound("Intractn_DrawTorch", ChannelType::SFX);
			// action trigger

			auto rad = Random<float>(0.f, 3.141592f).Generate();
			auto dirX = std::cosf(rad);
			auto dirY = std::sinf(rad);

			auto spawn = GetComponent<SpawnerComponent>();
			if (spawn == nullptr) return;
			auto obj = spawn->SpawnObject();
			if (obj == nullptr) return;
			obj->SetPosition(_position);
			auto rigidCom = obj->GetComponent<RigidbodyComponent>();

			rigidCom->info.isKinematic = false;
			rigidCom->info.useGravity = true;

			auto act = static_cast<PxRigidDynamic*>(rigidCom->rbActor);

			auto tr = act->getGlobalPose();
			tr.p.x = _position.x * 10.f;
			tr.p.y = _position.y * 10.f + 1.f;
			tr.p.z = _position.z * 10.f;
			act->setGlobalPose(tr);

			rigidCom->rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !rigidCom->info.useGravity);
			act->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidCom->info.isKinematic);

			float vel[3] = { dirX, 5.f, dirY };
			rigidCom->AddForce(vel, ForceMode::IMPULSE);
			players[playerID].time = 0.f;
		}
	}
}

void TorchBox::InteractExit(Object* thisPlayer, int playerID)
{
	players[playerID].time = 0.f;

	HighLightOff();
}

void TorchBox::EditorContext()
{
	ImGui::Text("TorchBox");
	ImGui::Spacing();
}

std::string_view TorchBox::GetType()
{
	return MetaType<TorchBox>::type;
}


