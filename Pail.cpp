#include "Pail.h"
#include "Physics/Common.h"
#include "../InputManager.h"
#include "Player.h"
#include "RenderComponent.h"
#include "SoundManager.h"
void Pail::Initialize()
{
	Object::Initialize();
}

void Pail::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Pail::Update(float tick)
{
	Object::Update(tick);

	// render를 init단계에서 확인하려면 프리펩뿐이므로 update에서 체크
	if (waterPail == nullptr) {
		auto renders = GetComponents<RenderComponent>();

		for (auto r : renders) {
			auto model = r->GetInstancedModel();
			if (model != nullptr) {
				if (model->instancedName.find("item_bucketWater") != std::string::npos) {
					waterPail = r;
					waterPail->SetVisiable(false);
					isEmpty = true;
				}
			}
		}
	}
}

void Pail::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Pail::Destroy()
{
	Object::Destroy();
}

void Pail::Serialize(_inout json& out)
{
	json data;
	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Pail>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}
	out["World"]["objects"].push_back(data);
}

void Pail::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);
}

void Pail::OnTriggerEnter(ICollider* other)
{
}

void Pail::OnTriggerStay(ICollider* other)
{
}

void Pail::OnTriggerExit(ICollider* other)
{
}

void Pail::OnCollisionEnter(ICollider* other)
{
	PxQueryFilterData filterData;
	filterData.data.word0 = 2; //item -> 2  여기에 이거 하는거 맞나?
}

void Pail::OnCollisionStay(ICollider* other)
{
}

void Pail::OnCollisionExit(ICollider* other)
{
}

bool Pail::IsIgnoreInteract()
{
	return false;
}

void Pail::InteractEnter(Object* thisPlayer, int playerID)
{
	HighLightOn();
}

void Pail::Interact(Object* thisPlayer, int playerID, float tick)
{
	if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::X) || InputManagement->IsKeyPressed('X')) {
		Sound->playSound("Intractn_PickUp", ChannelType::SFX);
		static_cast<Player*>(thisPlayer)->SetHeldObject(this);
		// action trigger
	}
}

void Pail::InteractExit(Object* thisPlayer, int playerID)
{
	HighLightOff();
}

void Pail::BornItem()
{
	SetWater(false);
}

void Pail::EditorContext()
{
	ImGui::Text("Pail");
	ImGui::Spacing();
}

std::string_view Pail::GetType()
{
	return MetaType<Pail>::type;
}

void Pail::SetWater(bool full)
{
	// action trigger 이건 bool값에 따라 소리 다르게 가능합니다.

	waterPail->SetVisiable(full);
	isEmpty = !full;

	// 물 가득차면 렌더컴포넌트 중에 빈 양동이 있으면 true, false
	std::cout << "pail : " << (full ? "full" : "empty") << std::endl;
	std::cout << "This message indicates that the function has not been added. : Pail.h" << std::endl;
}
