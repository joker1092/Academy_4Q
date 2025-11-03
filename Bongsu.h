#pragma once
#include "Object.h"
#include "GameManager.h"

class FireComponent;
class Bongsu : public Object
{
public:
	Bongsu() : Object("Bongsu") {};
	Bongsu(const std::string& name) : Object(name) {};
	virtual ~Bongsu() = default;
	virtual void Initialize() override;
	virtual void FixedUpdate(float fixedTick) override;
	virtual void Update(float tick) override;
	virtual void LateUpdate(float tick) override;
	virtual void Destroy() override;
	virtual void Serialize(_inout json& out) override;
	virtual void DeSerialize(_in json& in) override;


	void AddMaterial(); // 장작 추가
	void OutFire() { _time = 0; } // 불끄기
	void SetFire() { _time = _maxTime; } // 불켜기

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

public:
	int BongsuID = 0; //봉수 ID

private:
	friend class FireComponent;
	int _isFire = 0;	 //0:불꺼짐 1:불켜짐
	float _time = 0;	//켜져있을 남은 시간
	float _maxTime = 60.0f; //최대 시간
	float _materialTime = 20.0f; //임의 변수 추가 장작 하나당 추가되는 시간

protected:
	playerTime triggerPlayers[2];
};

template<>
struct MetaType<Bongsu>
{
	static constexpr std::string_view type{ "Bongsu" };
};

