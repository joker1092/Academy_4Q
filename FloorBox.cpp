#include "FloorBox.h"

void FloorBox::Initialize()
{
	Object::Initialize();
}

void FloorBox::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void FloorBox::Update(float tick)
{
	Object::Update(tick);
}

void FloorBox::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void FloorBox::Destroy()
{
	Object::Destroy();
}

void FloorBox::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<FloorBox>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void FloorBox::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void FloorBox::OnTriggerEnter(ICollider* other)
{
}

void FloorBox::OnTriggerStay(ICollider* other)
{
}

void FloorBox::OnTriggerExit(ICollider* other)
{
}

void FloorBox::OnCollisionEnter(ICollider* other)
{
}

void FloorBox::OnCollisionStay(ICollider* other)
{
}

void FloorBox::OnCollisionExit(ICollider* other)
{
}

void FloorBox::EditorContext()
{
	ImGui::Text("FloorBox");
	ImGui::Spacing();
}

std::string_view FloorBox::GetType()
{
	return MetaType<FloorBox>::type;
}
