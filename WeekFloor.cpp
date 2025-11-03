#include "WeekFloor.h"
#include "Player.h"
#include "RenderComponent.h"
#include "ColliderComponent.h"
#include "SoundManager.h"
void WeekFloor::Initialize()
{
	Object::Initialize();
}

void WeekFloor::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void WeekFloor::Update(float tick)
{
	Object::Update(tick);
	if (bridge == nullptr) {
		auto renders = GetComponents<RenderComponent>();

		for (auto r : renders) {
			auto model = r->GetInstancedModel();
			if (model) {
				if (model->instancedName.find("Platform_WeakBridge01") != std::string::npos) {
					bridge = r;
				}
			}
		}
	}
	if (weakBridge == nullptr) {
		auto renders = GetComponents<RenderComponent>();

		for (auto r : renders) {
			auto model = r->GetInstancedModel();
			if (model) {
				if (model->instancedName.find("Platform_WeakBridge02") != std::string::npos) {
					weakBridge = r;
				}
			}
		}
	}


	if (isOverhead > 0) {
		float prevTime = timer;
		float divide = maxTimer * .5f;

		timer += tick;

		if (prevTime <= divide && timer > divide) {
			//render component의 visible을 변경.
			if(bridge)
				bridge->SetVisiable(false);
			Sound->playSound("WeakBridge01", ChannelType::SFX);
		}

		if (timer > maxTimer) {
			// action trigger

			auto renders = GetComponents<RenderComponent>();
			for (auto& r : renders) {
				r->SetVisiable(false);
			}

			auto colliders = GetComponents<ColliderComponent>();
			for (auto& c : colliders) {
				c->SetEnable(false);
			}

			isOverhead = -10; // overhead가 0이상이 될 수 없도록. 야매.

			if (weakBridge)
				weakBridge->SetVisiable(false);

			Sound->playSound("WeakBridge02", ChannelType::SFX);
		}
	}
}

void WeekFloor::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void WeekFloor::Destroy()
{
	Object::Destroy();
}

void WeekFloor::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<WeekFloor>::type;

	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void WeekFloor::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);
}

void WeekFloor::OnTriggerEnter(ICollider* other)
{
}

void WeekFloor::OnTriggerStay(ICollider* other)
{
}

void WeekFloor::OnTriggerExit(ICollider* other)
{
}

void WeekFloor::OnCollisionEnter(ICollider* other)
{
	if (other->GetOwner()->GetType() == MetaType<Player>::type) {
		isOverhead++;
		std::cout << "player enter WeekFloor." << std::endl;
	}
}

void WeekFloor::OnCollisionStay(ICollider* other)
{
}

void WeekFloor::OnCollisionExit(ICollider* other)
{
	if (other->GetOwner()->GetType() == MetaType<Player>::type) {
		isOverhead--;
		std::cout << "player exit WeekFloor." << std::endl;
	}
}

std::string_view WeekFloor::GetType()
{
	return MetaType<WeekFloor>::type;
}

void WeekFloor::EditorContext()
{
	ImGui::Text("WeekFloor");
	ImGui::Spacing();
}
