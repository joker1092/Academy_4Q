#pragma once
#include "Object.h"

//하이에나 스폰 포탈 -> 스포너가 여기 붙을 예정
class EnemyPotal : public Object
{
public:
	EnemyPotal() : Object("EnemyPotal") {};
	EnemyPotal(const std::string& name) : Object(name) {};
	virtual ~EnemyPotal() = default;
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
struct MetaType<EnemyPotal>
{
	static constexpr std::string_view type{ "EnemyPotal" };
};