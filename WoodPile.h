//장작더미 클래스
#pragma once
#include "Object.h"
class WoodPile : public Object
{
public:
	WoodPile() : Object("WoodPile") {};
	WoodPile(const std::string& name) : Object(name) {};
	virtual ~WoodPile() = default;
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

	virtual bool IsIgnoreInteract()	override;
	virtual void InteractEnter(Object* thisPlayer, int playerID) override;
	virtual void Interact(Object* thisPlayer, int playerID, float tick) override;
	virtual void InteractExit(Object* thisPlayer, int playerID) override;
	virtual void EditorContext() override;

	virtual std::string_view GetType() override;
private:
	int interactCount = 0;
	int maxInteractCount = 5;
	float timer = 0.f;
	float maxTimer = 0.2f;
};

template<>
struct MetaType<WoodPile>
{
	static constexpr std::string_view type{ "WoodPile" };
};