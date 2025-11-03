#include "Monster.h"
#include "Player.h"
#include "CharacterController.h"
#include "Physics/Common.h"
#include "SoundManager.h"

void Monster::Initialize()
{
	Object::Initialize();

	_btComponent = AddComponent<BehaviorTreeComponent>(std::make_unique<MonsterBT>(this));

	player1 = GameManagement->GetPlayer1();
	player2 = GameManagement->GetPlayer2();


	//pathfinding initialize
	if (_targetPlayer!=nullptr)
	{
		GameManagement->GetCurrWorld()->GetGrid(x, z, mapsize);
		Mathf::Vector3 targetPosition = _targetPlayer->GetWorldMatrix().Translation();
		//targetPosition /= 10;
		std::pair<int, int> tarpos = std::make_pair((int)targetPosition.x, (int)targetPosition.z);
		//Mathf::Vector3 pos = { _position.x, _position.y, _position.z };
		Mathf::Vector3 pos = this->GetWorldMatrix().Translation();
		std::pair<int, int> monpos = std::make_pair((int)pos.x, (int)pos.z);
		_pathFinding = new PathFinding(mapsize,x,z, monpos, tarpos);
		std::pair<int, int> res = _pathFinding->update();
		updateTime = 0.0f;
		direction = Vector2{ (float)(res.first - monpos.first),(float)(res.second - monpos.second) };
		direction.Normalize();
	}
	else {
		_targetPlayer=SuchTargetPlayer();
	}

}

void Monster::FixedUpdate(float fixedTick)
{
	//todo : Monster FixedUpdate
	Object::FixedUpdate(fixedTick);
}

void Monster::Update(float tick)
{
	//todo : Monster Update
	Object::Update(tick);
	updateTime += tick;


	if (controller == nullptr) {
		controller = GetComponent<CharacterController>();
		if (controller == nullptr)
			return;
	}
	IgnoreSelfQueryFilterCallback ignore(controller->controller->getActor());
	PxQueryFilterData filterData;
	filterData.data.word0 = PhysFilter::WORD1;

	controller->moveSpeed = _speed;

	//1초마다 타겟을 찾아서 경로를 갱신
	if (updateTime >= updateLimit )
	{
		_targetPlayer = SuchTargetPlayer();
		PathFindingToTarget();
	}
	//경로가 있으면 이동
	MoveToTarget(tick);
	
	
	//레이케스트로 타겟이 걸리면 공격
	auto curdir = controller->currDirection;
	Vector3 dir = { curdir.x, 0.f, curdir.y };
	dir.Normalize();
	float radius = controller->attribute.radius * 1.2f;
	auto position = GetLocalPosition() * 10.f;
	auto originPos = curdir * radius;
	position.x += originPos.x;
	position.z += originPos.y;
	auto coll = raycast(position, dir, 1.f, filterData, &ignore);
	Object* curInteractObject = nullptr;
	if (coll != nullptr) {
		//coll->GetOwner()->Interact(playerID, tick);
		curInteractObject = coll->GetOwner();
	}

	if (curInteractObject != nullptr && (curInteractObject->GetType()==MetaType<Player>::type)) {
		//todo : monster attack
		if (_monsterState.isAttacking) {
			AttackToTarget(curInteractObject);
		}
	}



	if (GetComponent<RenderComponent>() != nullptr) {
		if (!_btComponent) {
			_btComponent = AddComponent<BehaviorTreeComponent>(std::make_unique<MonsterBT>(this));
		}
		else {
			_btComponent->updateState<MonsterBT>(_monsterState);
			_btComponent->Update(tick);
		}
	}

}

void Monster::LateUpdate(float tick)
{
	//todo : Monster LateUpdate
	Object::LateUpdate(tick);
}



void Monster::Destroy()
{
	//todo : Monster Destroy
	Object::Destroy();
}

void Monster::Serialize(_inout json& out)
{
	json data;
	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["name"] = _name;
	data["type"] = MetaType<Monster>::type;
	for (auto& component : _components)
	{
		component->Serialize(data);
	}
	out["World"]["objects"].push_back(data);
}

void Monster::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);
}

void Monster::OnTriggerEnter(ICollider* other)
{
}

void Monster::OnTriggerStay(ICollider* other)
{
}

void Monster::OnTriggerExit(ICollider* other)
{
}

void Monster::OnCollisionEnter(ICollider* other)
{
	PxQueryFilterData filterData;

	filterData.data.word0 = 1; //item -> 2  여기에 이거 하는거 맞나?

}

void Monster::OnCollisionStay(ICollider* other)
{
}

void Monster::OnCollisionExit(ICollider* other)
{
}

bool Monster::IsIgnoreInteract()
{
	return false;
}

void Monster::InteractEnter(Object* thisPlayer, int playerID)
{
}

void Monster::Interact(Object* thisPlayer, int playerID, float tick)
{
}

void Monster::InteractExit(Object* thisPlayer, int playerID)
{
	_hp -= 3;
	if (_hp <= 0)
	{
		_hp = 0;
		
		DeadToTarget(thisPlayer);
	}
	else {
		HitToTarget(thisPlayer);
	}
}

void Monster::BornItem()
{
	Event->Publish("MonsterDead" + GetName(), this);
}

std::string_view Monster::GetType()
{
	return MetaType<Monster>::type;
}

void Monster::EditorContext()
{
	ImGui::Text("Monster");
	ImGui::Spacing();

	ImGui::DragFloat3("_velocity", _velocity);

	if (ImGui::Button("hitTest")) {
		controller->AddForceChangeVelocityWhenisGround(_velocity);
	}
}

Object* Monster::SuchTargetPlayer()
{
	if (!player1)
	{
		player1 = GameManagement->GetPlayer1();
	}
	
	if (!player2)
	{
		player2 = GameManagement->GetPlayer2();
	}


	if (player1 != nullptr && player2 != nullptr)
	{
		Mathf::Vector3 pos = this->GetWorldMatrix().Translation();
		Mathf::Vector3 player1pos = player1->GetWorldMatrix().Translation();
		Mathf::Vector3 player2pos = player2->GetWorldMatrix().Translation();
		float distance1 = (pos - player1pos).Length();
		float distance2 = (pos - player2pos).Length();
		if (distance1 < distance2)
		{
			distance = distance1;
			return player1;
		}
		else {
			distance = distance2;
			return player2;
		}
	}
	else if (player1 != nullptr)
	{
		Mathf::Vector3 pos = this->GetWorldMatrix().Translation();
		Mathf::Vector3 player1pos = player1->GetWorldMatrix().Translation();
		distance = (pos - player1pos).Length();
		return player1;
	}
	else if (player2 != nullptr)
	{
		Mathf::Vector3 pos = this->GetWorldMatrix().Translation();
		Mathf::Vector3 player2pos = player2->GetWorldMatrix().Translation();
		distance = (pos - player2pos).Length();
		return player2;
	}
	else {
		return nullptr;
	}
	
}

//Moster A* pathfinding
void Monster::PathFindingToTarget()
{
	//todo : Monster A* pathfinding
	//_pathFinding update
	if (_pathFinding != nullptr)
	{
		
		//Mathf::Vector3 pos = { _position.x, _position.y, _position.z };
		Mathf::Vector3 pos = this->GetWorldMatrix().Translation();
		std::pair<int, int> monpos = std::make_pair((int)pos.x, (int)pos.z);
		
		_pathFinding->updateCurrentPos(monpos);
		if (_targetPlayer != nullptr)
		{
			Mathf::Vector3 targetPosition = _targetPlayer->GetWorldMatrix().Translation();
			//targetPosition /= 10;
			std::pair<int, int> tarpos = std::make_pair((int)targetPosition.x, (int)targetPosition.z);
			_pathFinding->updateTargetPos(tarpos);
		}
		std::pair<int, int> res = _pathFinding->update();

		updateTime = 0.0f;
		direction = Vector2{ (float)(res.first-monpos.first),(float)(res.second - monpos.second)};
		direction.Normalize();
		std::cout << "direction : " << direction.x << " " << 0<< " " << direction.y << std::endl;
	}
	else {
		if (_targetPlayer != nullptr)
		{
			
			GameManagement->GetCurrWorld()->GetGrid(x,z, mapsize);
			Mathf::Vector3 targetPosition = _targetPlayer->GetWorldMatrix().Translation();
			//targetPosition /= 10;
			std::pair<int, int> tarpos = std::make_pair((int)targetPosition.x, (int)targetPosition.z);
			//Mathf::Vector3 pos = { _position.x, _position.y, _position.z };
			Mathf::Vector3 pos = this->GetWorldMatrix().Translation();
			std::pair<int, int> monpos = std::make_pair((int)pos.x, (int)pos.z);
			_pathFinding = new PathFinding(mapsize, x, z, monpos, tarpos);

			std::pair<int, int> res = _pathFinding->update();
			direction = Vector2{ (float)(res.first - monpos.first),(float)(res.second - monpos.second) };
			direction.Normalize();

		}
		else {
			_targetPlayer=SuchTargetPlayer();
		}
	}

}

void Monster::MoveToTarget(float tick)
{
	Mathf::Vector2 direction2D = { direction.x,direction.y };
	direction2D.Normalize();
	Mathf::Vector3 moveDirection = { direction2D.x,0,direction2D.y };
	controller->direction = direction2D;
	//this->translate(moveDirection * _speed * tick);
	//_position += moveDirection * speed * tick;
}

float Monster::UpdateDistance()
{
	//Mathf::Vector3 pos = { _position.x, _position.y, _position.z };
	Mathf::Vector3 pos = this->GetWorldMatrix().Translation();
	if (_targetPlayer!=nullptr)
	{
		return (pos - _targetPlayer->GetWorldMatrix().Translation()).Length();
	}
	else {
		return -1;
	}
}


void Monster::AttackToTarget(Object* player)
{
	
	if (player->GetType()==MetaType<Player>::type)
	{
		auto pplayer = static_cast<Player*>(player);
		if (player != nullptr)
		{
			pplayer->Damage(this);
			_attackTimer = 5.f;
			Sound->playSound("Enemy_Attack", ChannelType::MONSTER);
		}
	}
	//todo : 공격
	//공격 처리를 하고 플레이어의 체력을 감소시킨다.
}

void Monster::HitToTarget(Object* Player)
{
	//todo : 피격 처리
	//피격 처리를 하고 스턴 상태로 전환
	Vector3 atkpos = Player->GetWorldMatrix().Translation();
	Vector3 monpos = this->GetWorldMatrix().Translation();
	Vector2 dir = Vector2{ atkpos.x, atkpos.z } - Vector2{ monpos.x,monpos.z };
	float force[3] = { dir.x ,1.f,dir.y };
	Sound->playSound("Enemy_Hit",ChannelType::MONSTER);
	_monsterState.isStun = true;
	controller->AddForceChangeVelocityWhenisGround(force);
}

void Monster::DeadToTarget(Object* Player)
{
	//todo : 죽음 처리
	//공격한 플레이어와 몬스터의 포지션으로 백터 계산
	Vector3 atkpos= Player->GetWorldMatrix().Translation();
	Vector3 monpos = this->GetWorldMatrix().Translation();

	Vector2 dir = Vector2{ atkpos.x, atkpos.z } - Vector2{ monpos.x,monpos.z };
	float force[3] = { dir.x* 500.f,500.f,dir.y* 500.f };
	Sound->playSound("Enemy_Hit", ChannelType::MONSTER);
	_monsterState.isDead = true;
	controller->AddForceChangeVelocityWhenisGround(force);
}
