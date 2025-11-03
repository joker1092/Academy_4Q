#pragma once
#include "EventSystem.h"
#include "Object.h"

class Torch : public Object
{
public:
	Torch() : Object("Torch") {};
	Torch(const std::string& name) : Object(name) {};
	virtual ~Torch() = default;
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

	void SetFire(bool fire);
	bool IsFire() { return isFire; }
private:
	//temp code
	float _lifeTime = 100.0f;
	float _inTime = 0.0f;
	//temp code end
	bool isFire;
};

template<>
struct MetaType<Torch>
{
	static constexpr std::string_view type{ "Torch" };
};

