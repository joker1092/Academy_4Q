#pragma once
#include "Object.h"

class Wall : public Object
{
public:
	Wall() : Object("Wall") {};
	Wall(const std::string& name) : Object(name) {};
	virtual ~Wall() = default;
	virtual void Initialize() override;
	virtual void FixedUpdate(float fixedTick) override;
	virtual void Update(float tick) override;
	virtual void LateUpdate(float tick) override;
	virtual void Destroy() override;
	virtual void Serialize(_inout json& out) override;
	virtual void DeSerialize(_in json& in) override;

	// ITriggerNotify을(를) 통해 상속됨
	virtual void OnTriggerEnter(ICollider* other);
	virtual void OnTriggerStay(ICollider* other);
	virtual void OnTriggerExit(ICollider* other);

	// ICollisionNotify을(를) 통해 상속됨
	virtual void OnCollisionEnter(ICollider* other);
	virtual void OnCollisionStay(ICollider* other);
	virtual void OnCollisionExit(ICollider* other);

	virtual std::string_view GetType() override;

	virtual void EditorContext() override;
};

template<>
struct MetaType<Wall>
{
	static constexpr std::string_view type{ "Wall" };
};