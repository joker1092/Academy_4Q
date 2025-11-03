#include "MonsterSpawner.h"

void MonsterSpawner::Update(float tick)
{
	//_isSpawning = _flagObject->GetComponent<FlagObject>()->GetFlag();
	if (_isSpawning)
	{
		_enemySpawnTimer -= tick;
	}
	if (_enemySpawnTimer <= 0)
	{
		_monsters[_spawnCount]->SetPosition(_owner->GetWorldMatrix().Translation());
		_monsters[_spawnCount]->Initialize();
		_spawnCount++;
		if (_spawnCount >= 10)
		{
			_spawnCount = 0;
		}
		_enemySpawnTimer = _spawnTime;
	}
}

void MonsterSpawner::EditorContext()
{
}

void MonsterSpawner::FixedUpdate(float fixedTick)
{
}

void MonsterSpawner::LateUpdate(float tick)
{
}
