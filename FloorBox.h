#pragma once
#include "Object.h"
class FloorBox : public Object
{
public:
	FloorBox() : Object("FloorBox") {};
	FloorBox(const std::string& name) : Object(name) {};
	virtual ~FloorBox() = default;
	virtual void Initialize() override;
	virtual void FixedUpdate(float fixedTick) override;
	virtual void Update(float tick) override;
	virtual void LateUpdate(float tick) override;
	virtual void Destroy() override;
	virtual void Serialize(_inout json& out) override;
	virtual void DeSerialize(_in json& in) override;
	virtual std::string_view GetType() override;

	// ITriggerNotify을(를) 통해 상속됨
	virtual void OnTriggerEnter(ICollider* other);
	virtual void OnTriggerStay(ICollider* other);
	virtual void OnTriggerExit(ICollider* other);

	// ICollisionNotify을(를) 통해 상속됨
	virtual void OnCollisionEnter(ICollider* other);
	virtual void OnCollisionStay(ICollider* other);
	virtual void OnCollisionExit(ICollider* other);

	virtual void EditorContext() override;
};

template<>
struct MetaType<FloorBox>
{
	static constexpr std::string_view type{ "FloorBox" };
};
