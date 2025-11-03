#pragma once
#include "Object.h"

class RenderComponent;
class Pail : public Object
{
public:
	Pail() : Object("Pail") {};
	Pail(const std::string& name) : Object(name) {};
	virtual ~Pail() = default;
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
	virtual void BornItem() override;
	virtual void EditorContext() override;

	virtual std::string_view GetType() override;

	// true면 양동이 가득참. false면 소각이나 양동이 물을 버린경우에 호출.
	void SetWater(bool full);

	bool IsEmpty() { return isEmpty; }
private:
	RenderComponent* waterPail = nullptr;
	bool isEmpty = false;
};

template<>
struct MetaType<Pail>
{
	static constexpr std::string_view type{ "Pail" };
};
