#pragma once
#include "Utility_Framework\Core.Mathf.h"
#include "EventSystem.h"
#include "Object.h"
#include "BehaviorTreeComponent.h"
#include "PathFinding.h"
#include "GameManager.h"




class Monster : public Object
{
	class MonsterBT : public BaseBehaviorTree
	{
	public:
		struct MonsterState {
			bool isDead = false;
			bool isStun = false;
			bool isAttacking = false;
			bool isMoveing = false;
			bool isIdle = true;
		}_monsterState;

	private:
		Monster* _monster;
		bool isDead(float tick) {
			
			return _monsterState.isDead;
		}
		bool isStun(float tick) {
			return _monsterState.isStun;
		}
		bool isAttacking(float tick) {
			return _monsterState.isAttacking;
		}
		bool isMoveing(float tick) {
			return _monsterState.isMoveing;
		}
		bool isIdle(float tick) {
			return _monsterState.isIdle;
		}


		bool CanFindPath() {
			
			return (_monster->direction.Length() > 0)&& (_monster->_attackTimer == 0);
		}


		std::shared_ptr<Action> dead = std::make_shared<Action>([this](float tick) {
			//todo : 죽음
			//행동에 대한 처리
			//에니메이션 처리
			std::cout << "Dead" << std::endl;
			_monster->GetComponent<RenderComponent>()->
			GetInstancedModel()->
				animModel->animator->PlayAnimation(7);
			Vector3 pos = _monster->GetWorldMatrix().Translation();

			if (pos.y < 0){
				Event->Publish("MonsterDead" + _monster->GetName(), this);
			}
			

			});
		std::shared_ptr<Action> stun = std::make_shared<Action>([this](float tick) {
			//todo : 스턴
			//행동에 대한 처리
			//에니메이션 처리
			std::cout << "Stun" << std::endl;
			_monster->GetComponent<RenderComponent>()->
			GetInstancedModel()->
				animModel->animator->PlayAnimation(0);
			//스턴시간이 지나면 스턴해제
			_monster->_stunTimer += tick;
			if (_monster->_stunTimer >= 3.0f) {
				_monster->_monsterState.isStun = false;
				_monster->_stunTimer = 0;
			}

			});
		std::shared_ptr<Action> move = std::make_shared<Action>([this](float tick) { 
			//todo : 이동
			//행동에 대한 처리
			std::cout << "Move" << std::endl;
			_monster->GetComponent<RenderComponent>()->
				GetInstancedModel()->
				animModel->animator->PlayAnimation(10);
			_monster->MoveToTarget(tick);
			//에니메이션 처리
			
			});
		std::shared_ptr<Action> attack = std::make_shared<Action>([this](float tick) { 
			//todo : 공격
			_monster->animeTimer += tick;
			std::cout << "Attack" << std::endl;
			//에니메이션 처리
			_monster->GetComponent<RenderComponent>()->
				GetInstancedModel()->
				animModel->animator->PlayAnimation(2);
			float dur = _monster->GetComponent<RenderComponent>()->GetInstancedModel()->animModel->animator->_animations[2]->GetDurationSeconds();
			if (_monster->animeTimer >= dur) {
				_monster->animeTimer = 0;
				_monster->_monsterState.isAttacking = false;
			}
			//공격 성공 실패 판단 필요
			
			});
		std::shared_ptr<Action> idle = std::make_shared<Action>([this](float tick) {
			//todo : 대기
			std::cout << "Idle" << std::endl;
			_monster->GetComponent<RenderComponent>()->
				GetInstancedModel()->
				animModel->animator->PlayAnimation(3);
			//idle 에니메이션 처리
			});

		std::shared_ptr<Condition> deadCondition = std::make_shared<Condition>([this](float tick) { return isDead(tick); }); //죽음조건
		std::shared_ptr<Condition> stunCondition = std::make_shared<Condition>([this](float tick) { return isStun(tick); }); //스턴조건
		std::shared_ptr<Condition> attackCondition = std::make_shared<Condition>([this](float tick) { 
			bool dist  = _monster->distance > -1 && _monster->distance <= 1; 
			bool time = (_monster->_attackTimer == 0);
			return dist && time;  //공격 가능거리 10					//공격조건
			});
		std::shared_ptr<Condition> moveCondition = std::make_shared<Condition>([this](float tick) { 
			return CanFindPath(); //이동조건
			});


		std::shared_ptr<Sequence> DeadSequence = std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
			std::reinterpret_pointer_cast<Node>(deadCondition), //공격조건 true
				std::reinterpret_pointer_cast<Node>(dead) 	//공격
		});

		std::shared_ptr<Sequence> StunSequence = std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
			std::reinterpret_pointer_cast<Node>(stunCondition), //공격조건 true
				std::reinterpret_pointer_cast<Node>(stun) 	//공격
		});


		std::shared_ptr<Sequence> MoveSequence = std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
			std::reinterpret_pointer_cast<Node>(moveCondition), //이동조건 true
				std::reinterpret_pointer_cast<Node>(move)		//이동
		});

		std::shared_ptr<Sequence> AttackSequence = std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
			std::reinterpret_pointer_cast<Node>(attackCondition), //공격조건 true
				std::reinterpret_pointer_cast<Node>(attack) 	//공격
		});

		std::shared_ptr<Selector> chase = std::make_shared<Selector>(std::vector<std::shared_ptr<Node>>{
			std::reinterpret_pointer_cast<Node>(MoveSequence), //이동 or
				std::reinterpret_pointer_cast<Node>(idle)	//대기
		});

		std::shared_ptr<Parallel> attackParallel = std::make_shared<Parallel>(std::vector<std::shared_ptr<Node>>{
			std::reinterpret_pointer_cast<Node>(AttackSequence), //공격
			std::reinterpret_pointer_cast<Node>(chase)	//이동 or 대기
		}, ParallelPolicy::ANY_SUCCESS);



	public:
		explicit MonsterBT(Monster* monster) : _monster(monster) {}

		void buildTree() override {
			root = std::make_shared<Selector>(std::vector<std::shared_ptr<Node>>{
				DeadSequence, //죽음 else
				StunSequence, //스턴 else
				attackParallel //공격 or 이동 or 대기
			});
		}

		void updateState(const MonsterState& state) {
			_monsterState = state;
		}
	};

public:
	Monster() : Object("Monster") {};
	Monster(const std::string& name) : Object(name) {};

	void Initialize() override;
	void FixedUpdate(float fixedTick) override;
	void Update(float tick) override;
	void LateUpdate(float tick) override;
	void Destroy() override;
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

	virtual bool IsIgnoreInteract() override;
	virtual void InteractEnter(Object* thisPlayer, int playerID) override;
	virtual void Interact(Object* thisPlayer, int playerID, float tick) override;
	virtual void InteractExit(Object* thisPlayer, int playerID) override;


	virtual void BornItem() override;

	virtual std::string_view GetType() override;

	virtual void EditorContext() override;

private:
	void SetMonsterState(MonsterBT::MonsterState state) { _monsterState = state; }
	void SetTargetPlayer(Object* target) { _targetPlayer = target; }
	Object* SuchTargetPlayer();
	void PathFindingToTarget();
	void MoveToTarget(float tick);
	float UpdateDistance();
	void AttackToTarget(Object* Player);
	void HitToTarget(Object* Player);
	void DeadToTarget(Object* thisPlayer);
	void Daed() { Event->Publish("MonsterDead" + _name, this); }


private:
	//행동 트리 관리
	MonsterBT::MonsterState _monsterState;
	BehaviorTreeComponent* _btComponent;
	//Loot* _loot = nullptr;
	//타겟 확인 및 길찾기
	PathFinding* _pathFinding = nullptr;
	Object* player1 = nullptr;
	Object* player2 = nullptr;
	Object* _targetPlayer = nullptr;
	float updateTime = 0.0f;
	float updateLimit = 1.0f;
	Mathf::Vector2 direction ;
	float distance;

	//통상 정보
	float _hp = 9;
	float _speed = 0.2f;
	float _attackTimer = 0.0f;
	float _attackFailTimer = 0.0f;
	float _stunTimer = 0.0f;

	//통상 관리 변수
	bool readyAttack = true; //공격준비
	float animeTimer = 0.0f; //에니메이션 관리시간
	CharacterController* controller = nullptr;
	

	//임시 테스트용 변수
	//mapsize 15X15
	int x;
	int z;
	std::vector<std::vector<int>>  mapsize;
	
	private:
		float _velocity[3] = { 0,0,0 };
};

template<>
struct MetaType<Monster>
{
	static constexpr std::string_view type{ "Monster" };
};