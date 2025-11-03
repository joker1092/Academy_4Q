#pragma once
#include "Object.h"
class Drum : public Object
{
public:
	Drum() : Object("Drum") {};
	Drum(const std::string& name) : Object(name) {};
	virtual ~Drum() = default;
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
		union Value {
			int count;
			float time;
		};
		Value value;
		bool use = false;	// 해당 플레이어가 사용중인 상태.
	};
	playerTime players[2];
	float maxTimer = 1.3f;
};

template<>
struct MetaType<Drum>
{
	static constexpr std::string_view type{ "Drum" };
};
