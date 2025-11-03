#include "WoodPile.h"
#include "InputManager.h"
#include "SpawnerComponent.h"
#include "BoxColliderComponent.h"
#include "RigidbodyComponent.h"
#include "../Physics/Common.h"
#include "../SoundManager.h"
#include "Core.Random.h"

void WoodPile::Initialize()
{
	Object::Initialize();
}

void WoodPile::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void WoodPile::Update(float tick)
{
	Object::Update(tick);

	if(timer > 0.f)
		timer -= tick;
}

void WoodPile::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void WoodPile::Destroy()
{
	Object::Destroy();
}

void WoodPile::Serialize(_inout json& out)
{
	json data;
	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<WoodPile>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}
	out["World"]["objects"].push_back(data);
}

void WoodPile::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void WoodPile::OnTriggerEnter(ICollider* other)
{
}

void WoodPile::OnTriggerStay(ICollider* other)
{
}

void WoodPile::OnTriggerExit(ICollider* other)
{
}

void WoodPile::OnCollisionEnter(ICollider* other)
{
}

void WoodPile::OnCollisionStay(ICollider* other)
{
}

void WoodPile::OnCollisionExit(ICollider* other)
{
}

bool WoodPile::IsIgnoreInteract()
{
	return false;
}

void WoodPile::InteractEnter(Object* thisPlayer, int playerID)
{
	HighLightOn();
}

void WoodPile::Interact(Object* thisPlayer, int playerID, float tick)
{
	if ((InputManagement->IsControllerConnected(playerID) && 
		InputManagement->IsControllerButtonDown(playerID, ControllerButton::X) &&
		timer <= 0.f) || (InputManagement->IsKeyDown('X') && timer <= 0.f)) {
		interactCount++;
		timer = maxTimer;

		if (playerID == 0)
		{
			GameManagement->GetPlayer1()->GetPlayerState().isUseAxe = true;
			Sound->playSound("Intractn_ChopWood", ChannelType::SFX);
		}
		else if (playerID == 1)
		{
			GameManagement->GetPlayer2()->GetPlayerState().isUseAxe = true;
			Sound->playSound("Intractn_ChopWood", ChannelType::SFX);
		}

		
		// 클릭에 대한 효과도 이부분에서 처리하면 될거같습니다.
		if (interactCount >= maxInteractCount) {
			// 장작을 다 패면.
			// 장작 스폰.
			// action trigger

			std::cout << "spawn firewood." << std::endl;
			interactCount = 0;

			auto rad = Random<float>(0.f, 3.141592f).Generate();
			auto dirX = std::cosf(rad);
			auto dirY = std::sinf(rad);

			auto spawn = GetComponent<SpawnerComponent>();
			if (spawn == nullptr) return;
			auto obj = spawn->SpawnObject();
			if (obj == nullptr) return;
			obj->SetPosition(_position);
			auto rigidCom = obj->GetComponent<RigidbodyComponent>();
			//auto boxCom = obj->AddComponent<BoxColliderComponent>();
			//auto rigidCom = obj->AddComponent<RigidbodyComponent>();
			//
			rigidCom->info.isKinematic = false;
			rigidCom->info.useGravity = true;
			//
			auto act = static_cast<PxRigidDynamic*>(rigidCom->rbActor);
			
			auto tr = act->getGlobalPose();
			tr.p.x = _position.x * 10.f;
			tr.p.y = _position.y * 10.f + 1.f;
			tr.p.z = _position.z * 10.f;
			act->setGlobalPose(tr);
			
			rigidCom->rbActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !rigidCom->info.useGravity);
			act->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, rigidCom->info.isKinematic);
			float zero[3] = { 0.f,0.f,0.f };
			rigidCom->AddForce(zero, ForceMode::VELOCITY_CHANGE);
			float vel[3] = { dirX, 5.f, dirY };
			rigidCom->AddForce(vel, ForceMode::IMPULSE);

		}
	}
}

void WoodPile::InteractExit(Object* thisPlayer, int playerID)
{
	HighLightOff();
}

void WoodPile::EditorContext()
{
	ImGui::Text("WoodPile");
	ImGui::Spacing();
}

std::string_view WoodPile::GetType()
{
	return MetaType<WoodPile>::type;
}


