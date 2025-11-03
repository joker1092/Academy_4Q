#include "Loot.h"
#include "../InputManager.h"
#include "Player.h"
#include "RigidbodyComponent.h"

void Loot::Initialize()
{
	Object::Initialize();
}

void Loot::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Loot::Update(float tick)
{
	Object::Update(tick);
}

void Loot::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Loot::Destroy()
{
	Object::Destroy();
}

void Loot::Serialize(_inout json& out)
{
	json data;
	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Loot>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}
	out["World"]["objects"].push_back(data);
}

void Loot::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);
}

void Loot::OnTriggerEnter(ICollider* other)
{
}

void Loot::OnTriggerStay(ICollider* other)
{
}

void Loot::OnTriggerExit(ICollider* other)
{
}

void Loot::OnCollisionEnter(ICollider* other)
{
}

void Loot::OnCollisionStay(ICollider* other)
{
}

void Loot::OnCollisionExit(ICollider* other)
{
}

bool Loot::IsIgnoreInteract()
{
	return false;
}

void Loot::InteractEnter(Object* thisPlayer, int playerID)
{
	HighLightOn();
}

void Loot::Interact(Object* thisPlayer, int playerID, float tick)
{
	if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::X) || InputManagement->IsKeyPressed('X')) {
		static_cast<Player*>(thisPlayer)->SetHeldObject(this);
		// action trigger
	}
}

void Loot::InteractExit(Object* thisPlayer, int playerID)
{
	HighLightOff();
}

void Loot::BornItem()
{
	// action trigger

	auto renders = GetComponents<RenderComponent>();
	for (auto& r : renders) {
		r->SetVisiable(false);
	}

	auto colliders = GetComponents<ColliderComponent>();
	for (auto& c : colliders) {
		c->SetEnable(false);
	}

	auto rb = GetComponent<RigidbodyComponent>();
	auto act = static_cast<physx::PxRigidDynamic*>(rb->rbActor);
	rb->info.isKinematic = true;
	act->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rb->info.isKinematic);
	act->setGlobalPose(physx::PxTransform(physx::PxIdentity));

	Event->Publish("LootReturn" + GetName(), this);
}

void Loot::EditorContext()
{
	ImGui::Text("Loot");
	ImGui::Spacing();
}

std::string_view Loot::GetType()
{
	return MetaType<Loot>::type;
}
