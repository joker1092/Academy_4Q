//횃대상자 오브젝트
#pragma once
#include "Object.h"
#include <list>

class TorchBox : public Object
{
public:
	TorchBox() : Object("TorchBox") {};
	TorchBox(const std::string& name) : Object(name) {};
	virtual ~TorchBox() = default;
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
	struct playerTime {
		int playerID;
		float time;
	};
	float maxTimer = 1.3f;
	playerTime players[2];
};

template<>
struct MetaType<TorchBox>
{
	static constexpr std::string_view type{ "TorchBox" };
};