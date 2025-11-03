#include "FlagObject.h"
#include "SoundManager.h"
#include "Player.h"
#include "Torch.h"
#include "TorchComponet.h"
#include "SpawnerComponent.h"
#include "CharacterController.h"

void FlagObject::Initialize()
{
	//todo : Initialize
	//이곳에서 시간설정 해줘도 상관은 없음 오브젝트별 다른시간이 필요하면 X;
	//SetTimer(30.0f);
	Object::Initialize();
}

void FlagObject::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
	//todo : FixedUpdates
}

void FlagObject::Update(float tick)
{
	Object::Update(tick);

	if (torchComponent == nullptr) {
		auto torchCom = GetComponent<TorchComponet>();
		if (torchCom != nullptr) {
			torchComponent = torchCom;
		}
	}
	if (spawnComponent == nullptr) {
		auto spawner = GetComponent<SpawnerComponent>();
		if (spawner != nullptr) {
			spawnComponent = spawner;
		}
	}

	//todo : Update
	if (_flagTime > 0)
	{
		_flagTime -= tick;
		if (_flagTime < 0.f) {
			// 불이 꺼지는 트리거.
			// 이때 사운드나 몬스터 포탈의 무언가를 활성화.
			if (torchComponent) {
				torchComponent->FireOff();
			}
		}
	}
	else {
		//불이 꺼진 상태에서 몬스터 스폰까지의 시간 체크
		if (spawnComponent) {
			_spawnTimer += tick;
			if (_spawnTimer > _spawnTime) {
				if (GameManagement->EnemyRespawn(this)) {
					auto obj = spawnComponent->SpawnObject();
					Vector3 pos = GetWorldMatrix().Translation() + spawnComponent->_spawnOffset;
					if (obj)
					{
						//todo : 몬스터 리스폰
						GameManagement->_enemyCount++;
						GameManagement->_enemyRespawnTime = 30.0f;
						static_cast<Monster*>(obj)->GetComponent<CharacterController>()->controller->setPosition(PxExtendedVec3((double)pos.x, (double)pos.y + 5.f, (double)pos.z));
					}
					_spawnTimer = 0;
				}
				else {
					_spawnTimer -= 1.0f; // 1초후 다시 시도
				}
			}
		}
	}
		
}

void FlagObject::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
	//todo : LateUpdate
}

void FlagObject::Destroy()
{
	Object::Destroy();
}

void FlagObject::Serialize(_inout json& out)
{
	json data;
	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<FlagObject>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}
	out["World"]["objects"].push_back(data);
}

void FlagObject::DeSerialize(_in json& in)
{
	Object::DeSerialize(in);
}

//void FlagObject::InterAction()
//{
//	//todo : InterAction
//	//플레그를 변경하는 상호작용을 하는 경우
//	if (_flagTime > 0) {
//		if (!_flag)
//		{
//			Sound->playSound("Intractn_IgniteWalltorch", ChannelType::SFX);
//			SetFlag(true);
//		}
//	}
//}

void FlagObject::SetTimer(float time)
{
	_flagTime = time;

	if (torchComponent) {
		if(time > 0.f)
			torchComponent->FireOn();
	}

	if (_flagTime <= 0.f) {
		// 짧은 시간 내에 트리거 왔다갔다 하면 처리는 어떻게할까요. 물어보고 조건넣기.
		Sound->playSound("Intractn_IgniteWalltorch", ChannelType::SFX);
	}
}

void FlagObject::OnTriggerEnter(ICollider* other)
{
	auto otherOwner = other->GetOwner();
	if (otherOwner->GetType() == MetaType<Player>::type) {
		auto player = static_cast<Player*>(otherOwner);
		int playerID = player->playerID;

		triggerPlayers[playerID].playerID = playerID;
		triggerPlayers[playerID].use = true;

		HighLightOn();

		if (player->IsHeldObject()) {
			auto held = player->GetHeldObject();
			if (held->GetType() == MetaType<Torch>::type) {
				auto torch = static_cast<Torch*>(held);
				if (torch->IsFire()) {
					SetTimer(30.f); // 불타고 있을 때 벽걸이 횃대 활성화.

				}
			}
		}
	}
}

void FlagObject::OnTriggerStay(ICollider* other)
{
}

void FlagObject::OnTriggerExit(ICollider* other)
{
	auto otherOwner = other->GetOwner();
	if (otherOwner->GetType() == MetaType<Player>::type) {
		HighLightOff();
	}
}

std::string_view FlagObject::GetType()
{
	return MetaType<FlagObject>::type;
}

void FlagObject::EditorContext()
{
	ImGui::Text("Flag");
	ImGui::Spacing();
}
