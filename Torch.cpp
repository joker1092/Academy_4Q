#include "Torch.h"
#include "Physics/Common.h"
#include "../InputManager.h"
#include "Player.h"
#include "RigidbodyComponent.h"
#include "SoundManager.h"

void Torch::Initialize()
{
	Object::Initialize();
}

void Torch::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Torch::Update(float tick)
{
	Object::Update(tick);
}

void Torch::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Torch::Destroy()
{
	Object::Destroy();
}

void Torch::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Torch>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void Torch::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void Torch::OnTriggerEnter(ICollider* other)
{
}

void Torch::OnTriggerStay(ICollider* other)
{
}

void Torch::OnTriggerExit(ICollider* other)
{
}

void Torch::OnCollisionEnter(ICollider* other)
{
	PxQueryFilterData filterData;
	filterData.data.word0 = 2; //item -> 2  여기에 이거 하는거 맞나?
}

void Torch::OnCollisionStay(ICollider* other)
{
}

void Torch::OnCollisionExit(ICollider* other)
{
}

bool Torch::IsIgnoreInteract()
{
	return false;
}

void Torch::InteractEnter(Object* thisPlayer, int playerID)
{
	HighLightOn();
}

void Torch::Interact(Object* thisPlayer, int playerID, float tick)
{
	if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::X)|| InputManagement->IsKeyPressed('X')) {
		Sound->playSound("Intractn_PickUp", ChannelType::SFX);
		static_cast<Player*>(thisPlayer)->SetHeldObject(this);
		// action trigger
	}
}

void Torch::InteractExit(Object* thisPlayer, int playerID)
{
	HighLightOff();
}

void Torch::BornItem()
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
	act->setGlobalPose(physx::PxTransform(PxIdentity));

	Event->Publish("TorchReturn" + GetName(), this);
}

void Torch::EditorContext()
{
	ImGui::Text("Torch");
	ImGui::Spacing();
}

std::string_view Torch::GetType()
{
	return MetaType<Torch>::type;
}

void Torch::SetFire(bool fire)
{
	isFire = fire;

	// 홰의 fire컴포넌트 추가 시 작업.
	std::cout << "Torch Fire : " << (fire ? "on" : "off") << std::endl;
}

