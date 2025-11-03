#include "Wood.h"
#include "RenderComponent.h"
#include "Physics/Common.h"
#include "../InputManager.h"
#include "Player.h"
#include "SoundManager.h"
#include "RigidbodyComponent.h"

void Wood::Initialize()
{
	Object::Initialize();
}

void Wood::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Wood::Update(float tick)
{
	Object::Update(tick);
	//temp code
	/*_inTime += tick;
	if (_inTime> _lifeTime)
	{
		Burn();
	}*/
	//temp code end
}

void Wood::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Wood::Destroy()
{
	Object::Destroy();
}

void Wood::Serialize(_inout json& out)
{
	json data;
	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Wood>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}
	out["World"]["objects"].push_back(data);
}

void Wood::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void Wood::OnTriggerEnter(ICollider* other)
{
}

void Wood::OnTriggerStay(ICollider* other)
{
}

void Wood::OnTriggerExit(ICollider* other)
{
}

void Wood::OnCollisionEnter(ICollider* other)
{
	PxQueryFilterData filterData;	
	filterData.data.word0 = 2; //item -> 2  여기에 이거 하는거 맞나?
}

void Wood::OnCollisionStay(ICollider* other)
{
}

void Wood::OnCollisionExit(ICollider* other)
{
}

bool Wood::IsIgnoreInteract()
{
	return false;
}

void Wood::InteractEnter(Object* thisPlayer, int playerID)
{
	HighLightOn();
}

void Wood::Interact(Object* thisPlayer, int playerID, float tick)
{
	if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::X) || InputManagement->IsKeyPressed('X')) {
		Sound->playSound("Intractn_PickUp", ChannelType::SFX);
		static_cast<Player*>(thisPlayer)->SetHeldObject(this);
		// action trigger
	}
}

void Wood::InteractExit(Object* thisPlayer, int playerID)
{
	HighLightOff();
}

void Wood::BornItem()
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

	SetPosition({ 0.f,0.f,0.f });
	auto rb = GetComponent<RigidbodyComponent>();
	auto act = static_cast<physx::PxRigidDynamic*>(rb->rbActor);
	rb->info.isKinematic = true;
	act->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rb->info.isKinematic);
	act->setGlobalPose(physx::PxTransform(PxIdentity));

	Event->Publish("WoodReturn" + GetName(), this);
}

void Wood::EditorContext()
{
	ImGui::Text("Wood");
	ImGui::Spacing();
}

std::string_view Wood::GetType()
{
	return MetaType<Wood>::type;
}

