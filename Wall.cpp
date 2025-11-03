#include "Wall.h"

void Wall::Initialize()
{
	Object::Initialize();
}

void Wall::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Wall::Update(float tick)
{
	Object::Update(tick);
}

void Wall::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Wall::Destroy()
{
	Object::Destroy();
}

void Wall::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Wall>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void Wall::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void Wall::OnTriggerEnter(ICollider* other)
{
}

void Wall::OnTriggerStay(ICollider* other)
{
}

void Wall::OnTriggerExit(ICollider* other)
{
}

void Wall::OnCollisionEnter(ICollider* other)
{
}

void Wall::OnCollisionStay(ICollider* other)
{
}

void Wall::OnCollisionExit(ICollider* other)
{
}

std::string_view Wall::GetType()
{
	return MetaType<Wall>::type;
}

void Wall::EditorContext()
{
	ImGui::Text("Wall");
	ImGui::Spacing();
}
