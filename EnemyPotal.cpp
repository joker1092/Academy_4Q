#include "EnemyPotal.h"

void EnemyPotal::Initialize()
{
	Object::Initialize();
}

void EnemyPotal::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void EnemyPotal::Update(float tick)
{
	Object::Update(tick);
}

void EnemyPotal::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}

void EnemyPotal::Destroy()
{
	Object::Destroy();
}

void EnemyPotal::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<EnemyPotal>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void EnemyPotal::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);
}

void EnemyPotal::OnTriggerEnter(ICollider* other)
{
}

void EnemyPotal::OnTriggerStay(ICollider* other)
{
}

void EnemyPotal::OnTriggerExit(ICollider* other)
{
}

void EnemyPotal::OnCollisionEnter(ICollider* other)
{
}

void EnemyPotal::OnCollisionStay(ICollider* other)
{
}

void EnemyPotal::OnCollisionExit(ICollider* other)
{
}

std::string_view EnemyPotal::GetType()
{
	return MetaType<EnemyPotal>::type;
}

void EnemyPotal::EditorContext()
{
	ImGui::Text("EnemyPotal");
	ImGui::Spacing();
}
