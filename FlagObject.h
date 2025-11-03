#pragma once
#include "Object.h"


//MonsterSpawner에서 사용할 FlagObject 클래스 이외에도 플레그 사용이 필요한 경우 사용
//ActionComponent 생성 되서 안 쓸듯 이미 만들어 놨으니 추후에 봅시다.
class TorchComponet;
class SpawnerComponent;
class FlagObject : public Object
{
public:
	FlagObject() : Object("FlagObject") {};
	FlagObject(const std::string& name) : Object(name) {};
	~FlagObject() = default;
	void Initialize() override;
	void FixedUpdate(float fixedTick) override;
	void Update(float tick) override;
	void LateUpdate(float tick) override;
	void Destroy() override;
	virtual void Serialize(_inout json& out) override;
	virtual void DeSerialize(_in json& in) override;
	//void InterAction(); // 상호작용 상위로 올릴까 고민중
	//bool GetFlag() const { return _flag; }
	//void SetFlag(bool flag) { _flag = flag; }
	void SetTimer(float time);// 자동으로 플레그가 거짓으로 바뀌는 시간설정 -1이면 비활성화 무제한 고려X

	// ITriggerNotify을(를) 통해 상속됨
	virtual void OnTriggerEnter(ICollider* other);
	virtual void OnTriggerStay(ICollider* other);
	virtual void OnTriggerExit(ICollider* other);

	virtual std::string_view GetType() override;

	virtual void EditorContext() override;

	// IsFire()가 true면 플래그 활성화.
	bool IsFire() { return _flagTime > 0.f; }

private:
	//bool _flag = false;
	float _flagTime = -1;// -1 이면 플레그 비활성화
	//float _timer = 0; 

	TorchComponet* torchComponent = nullptr;
	SpawnerComponent* spawnComponent = nullptr;


	float _spawnTimer = 0.0f;    //생성 시간
	float _spawnTime = 50.0f; //생성 간격
	float _initSpawnTime = 50.0f; //초기 생성 간격


protected:
	playerTime triggerPlayers[2];
};

template<>
struct MetaType<FlagObject>
{
	static constexpr std::string_view type{ "FlagObject" };
};
