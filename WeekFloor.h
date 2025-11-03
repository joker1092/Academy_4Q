#pragma once
#include "Object.h"

class RenderComponent;
class WeekFloor : public Object
{
public:
	WeekFloor() : Object("WeekFloor") {};
	WeekFloor(const std::string& name) : Object(name) {};
	virtual ~WeekFloor() = default;
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

private:
	RenderComponent* bridge = nullptr;
	RenderComponent* weakBridge = nullptr;
	int isOverhead = 0;
	float timer = 0.f;
	float maxTimer = 10.f;
};

template<>
struct MetaType<WeekFloor>
{
	static constexpr std::string_view type{ "WeekFloor" };
};