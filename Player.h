#pragma once
#include "Object.h"
#include "../InputManager.h"
#include "BehaviorTreeComponent.h"
#include "Torch.h"

class CharacterController;
class Player : public Object
{
	class PlayerBT : public BaseBehaviorTree
	{
	public:
		struct PlayerState {
			bool isStun = false;
			bool isAttacking = false;
			bool isJumping = false;
			bool isMoveing = false;
			bool isUseAxe = false;
			bool isInterAction = false;
			bool isDrum = false;
		} _playerState;

		Player* _player = nullptr;
	private:

		bool isStun(float tick) {
			return _playerState.isStun;
		}
		bool isAttacking(float tick) {
			return _playerState.isAttacking;
		}
		bool isJumping(float tick) {
			return _playerState.isJumping;
		}
		bool isMoveing(float tick) {
			return _playerState.isMoveing;
		}
		bool isUseAxe(float tick) {
			return _playerState.isUseAxe;
		}
		bool isInterAction(float tick) {
			return _playerState.isInterAction;
		}
		bool isDrum(float tick) {
			return _playerState.isDrum;
		}
		

		void StunAction(float tick) {
			//todo : 에니메이션 처리
			auto render = _player->GetComponent<RenderComponent>();
			render->SetVisiable(true);
			render->SetSubVisible(false);
			render->SetAxeVisible(false);

			if (render->GetInstancedModel() != nullptr) {
				render->GetInstancedModel()->animModel->animator->PlayAnimation(0);
			}

			_player->_stunTimer += tick;
			if (_player->_stunTimer >= _player->_stunTime) {
				_player->_isStun = false;
				_player->_stunTimer = 0;
			}
		}

		void AttackingAction(float tick) {
			auto render = _player->GetComponent<RenderComponent>();
			render->SetVisiable(false);
			render->SetSubVisible(true);
			render->SetAxeVisible(false);

			//여기서는 에니메이션만 나머지는 다른곳에서 처리
			if (render->
				GetSubInstancedModel() != nullptr) {
				render->
					GetSubInstancedModel()->
					animModel->animator->PlayAnimation(0);
			}
		}


		void UseDrum(float tick) {
			auto render = _player->GetComponent<RenderComponent>();
			render->SetVisiable(true);
			render->SetSubVisible(false);
			render->SetAxeVisible(false);

			if (render->
				GetInstancedModel() != nullptr) {
				render->
					GetInstancedModel()->
					animModel->animator->PlayAnimation(5);
			}

			_player->GetPlayerState().isDrum = false;
		}

		void InterAction(float tick) {
			auto render = _player->GetComponent<RenderComponent>();
			render->SetVisiable(true);
			render->SetSubVisible(false);
			render->SetAxeVisible(false);

			if (render->
				GetInstancedModel() != nullptr) {
				render->
					GetInstancedModel()->
					animModel->animator->PlayAnimation(6);
			}
			_player->GetPlayerState().isInterAction = false;
		}

		void UseAxeAction(float tick) {
			//여기서는 에니메이션만 나머지는 다른곳에서 처리
			if (_playerState.isUseAxe)
			{
				auto render = _player->GetComponent<RenderComponent>();
				render->SetVisiable(false);
				render->SetSubVisible(false);
				render->SetAxeVisible(true);
				if (render->GetAxeInstancedModel() != nullptr) {
					render->GetAxeInstancedModel()->animModel->animator->PlayAnimation(0);

					/*auto instancemodel = render->GetAxeInstancedModel();
					auto animmodel = instancemodel->animModel;
					auto anim = animmodel->animator;
					anim->PlayAnimation(0);*/

					_player->_animeTimer += tick;
					float duration = render->GetAxeInstancedModel()->animModel->animator->
						_currentAnimation->GetDurationSeconds();
					if (_player->_animeTimer > duration) {
						_player->GetPlayerState().isUseAxe = false;
						_player->_animeTimer = 0.0f;
					}
					
				}
			}
		}

		void JumpingAction(float tick) {
			auto render = _player->GetComponent<RenderComponent>();
			
			_player->JumpSound();
			//여기서는 에니메이션만 나머지는 다른곳에서 처리
			if (_player->IsHeldObject())
			{
				if (_player->heldObject->GetType() == MetaType<Torch>::type) {
					render->SetVisiable(false);
					render->SetSubVisible(true);
					render->SetAxeVisible(false);

					if (render->GetSubInstancedModel() != nullptr) {
						render->GetSubInstancedModel()->animModel->animator->PlayAnimation(3);
					}
				}
				else {
					render->SetVisiable(true);
					render->SetSubVisible(false);
					render->SetAxeVisible(false);

					if (render->
						GetInstancedModel() != nullptr) {
						render->
							GetInstancedModel()->
							animModel->animator->PlayAnimation(9);
					}
				}
			}
			else
			{
				render->SetVisiable(true);
				render->SetSubVisible(false);
				render->SetAxeVisible(false);

				if (render->
					GetInstancedModel() != nullptr) {
					render->
						GetInstancedModel()->
						animModel->animator->PlayAnimation(13);
				}
				
			}
		}


		void MoveingAction(float tick) {
			auto render = _player->GetComponent<RenderComponent>();
			_player->MoveSound();
			//여기서는 에니메이션만 나머지는 다른곳에서 처리
			if (_player->IsHeldObject())
			{
				if (_player->heldObject->GetType() == MetaType<Torch>::type) {
					render->SetVisiable(false);
					render->SetSubVisible(true);
					render->SetAxeVisible(false);

					if (render->GetSubInstancedModel() != nullptr) {
						render->GetSubInstancedModel()->animModel->animator->PlayAnimation(2);
					}
				}
				else {
					render->SetVisiable(true);
					render->SetSubVisible(false);
					render->SetAxeVisible(false);

					if (render->
						GetInstancedModel() != nullptr) {
						render->
							GetInstancedModel()->
							animModel->animator->PlayAnimation(10);
					}
				}
			}
			else
			{
				render->SetVisiable(true);
				render->SetSubVisible(false);
				render->SetAxeVisible(false);

				if (render->
					GetInstancedModel() != nullptr) {
					render->
						GetInstancedModel()->
						animModel->animator->PlayAnimation(12);
				}
			}
		}


		void Idle(float tick) {
			auto render = _player->GetComponent<RenderComponent>();

			if (_player->IsHeldObject())
			{
				if (_player->heldObject->GetType() == MetaType<Torch>::type) {
					render->SetVisiable(false);
					render->SetSubVisible(true);
					render->SetAxeVisible(false);

					if (render-> GetSubInstancedModel() != nullptr) {
						render->GetSubInstancedModel()->animModel->animator->PlayAnimation(1);
					}
				}
				else {
					render->SetVisiable(true);
					render->SetSubVisible(false);
					render->SetAxeVisible(false);

					if (render->
						GetInstancedModel() != nullptr) {
						render->
							GetInstancedModel()->
							animModel->animator->PlayAnimation(2);
					}
				}
			}
			else {
				render->SetVisiable(true);
				render->SetSubVisible(false);
				render->SetAxeVisible(false);

				if (render->
					GetInstancedModel() != nullptr) {
					render->
						GetInstancedModel()->
						animModel->animator->PlayAnimation(0);
				}
			}
		}

	public:

		explicit PlayerBT(Player* player) : _player(player) {}

		void buildTree() override {
			root = std::make_shared<Selector>(std::vector<std::shared_ptr<Node>>{
				std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
					std::make_shared<Condition>([&](float tick) { return isStun(tick); }),
						std::make_shared<Action>([&](float tick) { StunAction(tick); }),
				}),
				std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
					std::make_shared<Condition>([&](float tick) { return isAttacking(tick); }),
						std::make_shared<Action>([&](float tick) { /*std::cout << "Attacking" << std::endl;*/ AttackingAction(tick); }),
				}),
				std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
					std::make_shared<Condition>([&](float tick) { return isJumping(tick); }),
						std::make_shared<Action>([&](float tick) { /*std::cout << "Jumping" << std::endl;*/ JumpingAction(tick); }),
				}),
				std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
					std::make_shared<Condition>([&](float tick) { return isMoveing(tick); }),
						std::make_shared<Action>([&](float tick) {/*std::cout << "Moveing" << std::endl;*/ MoveingAction(tick); }),
				}),
				std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
					std::make_shared<Condition>([&](float tick) { return isUseAxe(tick); }),
						std::make_shared<Action>([&](float tick) {/*std::cout << "UseAxe" << std::endl;*/ UseAxeAction(tick); }),
				}),
				std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
					std::make_shared<Condition>([&](float tick) { return isInterAction(tick); }),
						std::make_shared<Action>([&](float tick) {/*std::cout << "UseAxe" << std::endl;*/ InterAction(tick); }),
				}),
				std::make_shared<Sequence>(std::vector<std::shared_ptr<Node>>{
					std::make_shared<Condition>([&](float tick) { return isDrum(tick); }),
						std::make_shared<Action>([&](float tick) {/*std::cout << "UseAxe" << std::endl;*/ UseDrum(tick); }),
				}),std::make_shared<Action>([&](float tick) {/*std::cout << "Idle" << std::endl;*/ Idle(tick); }),
			});
		}

		void updateState(const PlayerBT::PlayerState& state) 
		{
			_playerState = state;
		}

	};
public:
	Player() : Object("Player") {};
	Player(const std::string& name) : Object(name) {};
	~Player() = default;
public:
	void Initialize() override;
	void FixedUpdate(float fixedTick) override;
	void Update(float tick) override;
	void LateUpdate(float tick) override;
	void Destroy() override;
	virtual void Serialize(_inout json& out) override;
	virtual void DeSerialize(_in json& in) override;

	virtual void EditorContext() override;

	// ITriggerNotify을(를) 통해 상속됨
	virtual void OnTriggerEnter(ICollider* other);
	virtual void OnTriggerStay(ICollider* other);
	virtual void OnTriggerExit(ICollider* other);

	// ICollisionNotify을(를) 통해 상속됨
	virtual void OnCollisionEnter(ICollider* other);
	virtual void OnCollisionStay(ICollider* other);
	virtual void OnCollisionExit(ICollider* other);

	virtual std::string_view GetType() override;

	virtual bool IsIgnoreInteract()	override;
	virtual void InteractEnter(Object* thisPlayer, int playerID) override;
	virtual void Interact(Object* thisPlayer, int playerID, float tick) override;
	virtual void InteractExit(Object* thisPlayer, int playerID) override;

	void MoveSound();
	void JumpSound();

	void BongsuHeat(int trigger);

	PlayerBT::PlayerState& GetPlayerState() { return _state; }
public:
	bool _isStun = false; //기절여부
	float _stunTime = 3.0f; //기절시간
	float _stunTimer = 0; //기절타이머 Player1_StunTime
	bool _isJump = false; //점프여부 Player1_isJumping
	bool _isDesease = false; //약화여부 Player1_debuff_cold
	float _currentCold = 0.0f; //현재추위 Player1_currentCold
	float _coldIncreaseRate = 1.7f; //추위증가량 Player1_coldIncreaseRate
	float _coldDecreaseRate = 17.0f; //추위감소량 Player1_coldDecreaseRate
	float _initSpeed = 1.0f; //초기속도 Player1_Speed
	float _deseaseSpeed = 0.5f; //약화시 속도 Player1_ColdSpeed
	float _throwVelocity = 10.f;

	int _isHeat = 0; //
	bool _active = false; //플레이어 활성화 여부

	float _animeTimer = 0.0f; //에니메이션 관리시간

	// 추위 관련.
	bool IsDesease() { return _currentCold >= 100.f; }
	constexpr static float _maxCold = 100.f; // 최대 추위 수치. 퍼센테이지 이므로 0~1인듯.
	void AddCold(float value) { 
		_currentCold += value; 
		if (_currentCold > _maxCold) {
			_currentCold = _maxCold;
			_isDesease = true;
		}
		if (_currentCold < 0.f) {
			_currentCold = 0.f;
			_isDesease = false;
		}
	}

private:
	bool IsHeat() { return _isHeat > 0; }

private:

	PlayerBT::PlayerState _state;
	BehaviorTreeComponent* _btComponent;

public:
	int playerID = 0;
	
private:
	Object* heldObject = nullptr;
	Object* prevInteractObject = nullptr; // 이전에 상호작용했던 오브젝트 
	CharacterController* controller = nullptr;
public:
	bool IsHeldObject() { return heldObject != nullptr; }
	void SetHeldObject(Object* heldObj);// 테스트용. 들고 있던 오브젝트를 떨군다는 기능 필요.
	Object* GetHeldObject() { return heldObject; }

	void Damage(Object* hiyna, float power = 1.f);
};

template<>
struct MetaType<Player>
{
	static constexpr std::string_view type{ "Player" };
};