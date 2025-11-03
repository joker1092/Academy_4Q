#pragma once
#include "GameManager.h"
#include "EventSystem.h"
#include "Component.h"
#include "ObjectFactory.h"
#include <vector>
class SpawnerComponent : public Component
{
public:
	SpawnerComponent() : nameBuffer(255, 0) {};
	SpawnerComponent(Object* owner) : nameBuffer(255, 0), Component() {
		_owner = owner;
	}
	~SpawnerComponent();

	// Component을(를) 통해 상속됨
	void Initialize() override;
	void FixedUpdate(float fixedTick) override;
	void Update(float tick) override;
	void LateUpdate(float tick) override;
	void EditorContext() override;
	virtual void Serialize(_inout json& out) override;
	virtual void DeSerialize(_in json& in) override;

	void SetFlagObject(FlagObject* flagObject) { _flagObject = flagObject; }
	void SetMaxSpawnCount(int maxSpawnCount) { _maxSpawnCount = maxSpawnCount; }
	void SetSpawnTime(float spawnTime) { _spawnTime = spawnTime; }
	void SetSpawnObjectName(const std::string& spawnObjectName) { _spawnObjectName = spawnObjectName; }
	
	Object* SpawnObject(); //스포너 위치에 오브젝트 생성
	void ReturnSpawnObject(Object* object); //오브젝트 반환
	uint32 ID() override { return _ID; }
	Vector3 _spawnOffset = Vector3(0, 0, 0);
private:
	static constexpr uint32 _ID{ 5005 };
	int _maxSpawnCount = 10; //최대 몬스터 생성 수
	std::queue<Object*> _object; // 스폰할 오브젝트들을 담을 벡터
	float _spawnTimer = 0.0f;    //생성 시간
	float _spawnTime = 50.0f; //생성 간격
	float _initSpawnTime = 50.0f; //초기 생성 간격
	int _spawnCount = 0; //생성된 몬스터 수 //최대 몬스터 10마리만 생성 필요시 사용
	bool _isSpawning = false; //생성중인지 여부
	FlagObject* _flagObject = nullptr; //생성 중 판별용 오브젝트 //todo : FlagObject : Object 클래스로 변경 세부 구현 필요
	std::string _spawnObjectName = ""; //생성할 오브젝트 이름
	std::string _prefabName = "";

	
private:
	std::vector<char> nameBuffer;
};

template<>
struct MetaType<SpawnerComponent>
{
	static constexpr std::string_view type{ "SpawnerComponent" };
};