#pragma once
#include "Component.h"
#include "Monster.h"


class MonsterSpawner : public Component
{
public:
	MonsterSpawner(Object* owner) : Component() {
		_owner = owner;
		_monsters.reserve(10); // 10개의 몬스터를 미리 생성해둠
	}
	// Component을(를) 통해 상속됨
	void Initialize() override { 
		_enemySpawnTimer = _spawnTime = _initSpawnTime; 
		for (int i = 0; i < 10; i++)
		{
			_monsters.emplace_back(new Monster("Monster"));
		}
	};
	void SetFlagObject(Object* flagObject) { _flagObject = flagObject; }
	void FixedUpdate(float fixedTick) override;
	void Update(float tick) override;
	void LateUpdate(float tick) override;
	void EditorContext() override;
	virtual void Serialize(_inout json& out) override {};
	virtual void DeSerialize(_in json& in) override {};

	uint32 ID() override { return _ID; }

private:
	static constexpr uint32 _ID{ 5006 };
	std::vector<Object*> _monsters; // 몬스터들을 담을 벡터
	float _enemySpawnTimer = 0.0f;    //생성 시간
	float _spawnTime = 50.0f; //생성 간격
	float _initSpawnTime = 50.0f; //초기 생성 간격
	int _spawnCount = 0; //생성된 몬스터 수 //최대 몬스터 10마리만 생성 필요시 사용
	bool _isSpawning = false; //생성중인지 여부
	Object* _flagObject = nullptr;
	//생성 중 판별용 오브젝트 //todo : FlagObject : Object 클래스로 변경 세부 구현 필요
};

