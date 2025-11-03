#include "Stairs.h"

void Stairs::Initialize()
{
	Object::Initialize();
}

void Stairs::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Stairs::Update(float tick)
{
	Object::Update(tick);
}

void Stairs::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void Stairs::Destroy()
{
	Object::Destroy();
}

void Stairs::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Stairs>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);

}

void Stairs::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

void Stairs::OnTriggerEnter(ICollider* other)
{
}

void Stairs::OnTriggerStay(ICollider* other)
{
}

void Stairs::OnTriggerExit(ICollider* other)
{
}

void Stairs::OnCollisionEnter(ICollider* other)
{
}

void Stairs::OnCollisionStay(ICollider* other)
{
}

void Stairs::OnCollisionExit(ICollider* other)
{
}

void Stairs::EditorContext()
{
	ImGui::Text("Stair");
	ImGui::Spacing();
}

std::string_view Stairs::GetType()
{
	return MetaType<Stairs>::type;
}
