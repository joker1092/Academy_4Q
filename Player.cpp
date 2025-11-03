#include "Player.h"
#include "CharacterController.h"
#include "Physics/Common.h"
#include "GameManager.h"
#include "Pail.h"
#include "SoundManager.h"

void Player::Initialize()
{
	Object::Initialize();
	//_btComponent = AddComponent<BehaviorTreeComponent>(std::make_unique<PlayerBT>(this));
}

void Player::FixedUpdate(float fixedTick)
{
	Object::FixedUpdate(fixedTick);
}

void Player::Update(float tick)
{
	Object::Update(tick);
	if (controller == nullptr) {
		controller = GetComponent<CharacterController>();
		if (controller == nullptr)
			return;
	}

	AddCold(tick * _coldIncreaseRate);

	controller->moveSpeed = _isDesease ? _deseaseSpeed : _initSpeed;

#ifdef Keyboard
	Vector2 dir;
	dir.x = 0.f;
	dir.y = 0.f;
	dir.x += InputManagement->IsKeyPressed('J') ? -1.f : 0.f;
	dir.x += InputManagement->IsKeyPressed('L') ? 1.f : 0.f;
	dir.y += InputManagement->IsKeyPressed('I') ? 1.f : 0.f;
	dir.y += InputManagement->IsKeyPressed('K') ? -1.f : 0.f;

	if (pow(dir.x, 2) + pow(dir.y, 2) < 0.04f) {
		dir.x = 0.f;
		dir.y = 0.f;
	}
	else {
		int a = 1;
	}
	dir.y = -dir.y;
	controller->direction = dir;
	if (dir.Length() > 0) {
		_state.isMoveing = true;
	}
	else {
		_state.isMoveing = false;
	}

	if (controller->contactCallback.isGround) {
		_state.isJumping = false;
	}
	else {
		_state.isJumping = true;
	}

	if ((InputManagement->IsControllerButtonDown(playerID, ControllerButton::A) || InputManagement->IsKeyPressed(' '))) {
		//controller->Jump();
		if (controller->contactCallback.isGround) {
			controller->contactCallback.isGround = false;
			float vel[3] = { 0.f, controller->jumpVelocity, 0.f };
			controller->AddForceChangeVelocity(vel);
		}
		/*std::vector<BoxShape> boxshapes;
		Physics->GetShapes(boxshapes);
		for (auto& b : boxshapes) {
			std::cout <<"worldpos : " << b.worldPosition[0]<< ", " << b.worldPosition[1] << ", " << b.worldPosition[2] << std::endl;
			std::cout <<"worldrot : " << b.worldRotation[0]<< ", " << b.worldRotation[1] << ", " << b.worldRotation[2] << ", " << b.worldRotation[3] << std::endl;
			std::cout << "halfsize : " << b.halfSize[0] << ", " << b.halfSize[1] << ", " << b.halfSize[2] << std::endl;
		}*/
	}
	//if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::X)) {
	//	if (controller->otherhit != nullptr) {
	//		controller->otherhit->Jump();
	//	}
	//}

	IgnoreSelfQueryFilterCallback ignore(controller->controller->getActor());
	PxQueryFilterData filterData;
	//filterData.flags
	filterData.data.word0 = PhysFilter::WORD1;

	// 레이 쏠 때 얘 써서 무시하기.
	if (IsHeldObject()) {


		auto pos = _position;
		pos.y += 0.2f;
		heldObject->SetPosition(pos);

		auto otherControl = heldObject->GetComponent<CharacterController>();
		if (otherControl != nullptr) {
			pos *= 10.f;
			otherControl->controller->setPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
		}
	}
	if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::B) || InputManagement->IsKeyPressed('B')) {
		// 놓기 버튼이 될 예정.
		if (IsHeldObject()) {
			auto rb = heldObject->GetComponent<RigidbodyComponent>();
			if (rb) {
				// 리지드바디 있으면
				float zero[3] = { 0.f, 0.f, 0.f };
				rb->AddForce(zero, ForceMode::VELOCITY_CHANGE);


				Vector2 forceDirection = controller->currDirection * _throwVelocity;
				float velocity[3] = { forceDirection.x, 0.f, forceDirection.y };
				rb->AddForce(velocity, ForceMode::IMPULSE);
				if(heldObject->GetType() != MetaType<Pail>::type)
					heldObject = nullptr;
			}
			else {
				auto otherControl = heldObject->GetComponent<CharacterController>();
				if (otherControl) {
					// 컨트롤러 있으면
					//otherControl->Jump();
					Vector2 forceDirection = controller->currDirection * _throwVelocity * 5.f;
					float velocity[3] = { forceDirection.x, 20.f, forceDirection.y };
					otherControl->AddForceChangeVelocityWhenisGround(velocity);
					heldObject = nullptr;
				}
			}
		}
		/*
		auto curdir = controller->currDirection;
		Vector3 dir = { curdir.x, 0.f, curdir.y };
		dir.Normalize();
		float radius = controller->attribute.radius * 1.2f;
		auto position = GetLocalPosition() * 10.f;
		auto originPos = curdir * radius;
		position.x += originPos.x;
		position.z += originPos.y;
		auto coll = raycast(position, dir, 1.f, filterData, &ignore);
		if (coll != nullptr) {
			std::cout << coll->GetOwner()->GetName() << std::endl;
		}
		*/
	}
	{
		// 상시 탐색 상태.

		auto curdir = controller->currDirection;
		Vector3 dir = { curdir.x, 0.f, curdir.y };
		dir.Normalize();
		float radius = controller->attribute.radius * 1.2f;
		//auto position = GetLocalPosition() * 10.f;
		auto test = GetLocalPosition() * 10.f;



		auto position = controller->controller->getFootPosition();
		position.y += 0.1f;	// 레이 위치를 컨트롤러 바닥에서 처리하도록 수정.
		Vector3 toDxVec = { (float)position.x, (float)position.y, (float)position.z };

		auto originPos = curdir * radius;
		position.x += originPos.x;
		position.z += originPos.y;

		toDxVec.x += originPos.x;
		toDxVec.z += originPos.y;

		//auto coll = raycast(position, dir, 1.f, filterData, &ignore);
		auto coll = raycast(toDxVec, dir, 1.f, filterData, &ignore);
		Object* curInteractObject = nullptr;
		if (coll != nullptr) {
			//coll->GetOwner()->Interact(playerID, tick);
			curInteractObject = coll->GetOwner();
		}

		if (prevInteractObject == nullptr && curInteractObject != nullptr) {
			curInteractObject->InteractEnter(this, playerID);
		}
		else if (prevInteractObject != nullptr && curInteractObject != nullptr && prevInteractObject != curInteractObject) {
			prevInteractObject->InteractExit(this, playerID);
			curInteractObject->InteractEnter(this, playerID);
		}
		else if (prevInteractObject != nullptr && curInteractObject != nullptr && prevInteractObject == curInteractObject) {
			curInteractObject->Interact(this, playerID, tick);
		}
		else if (prevInteractObject != nullptr && curInteractObject == nullptr) {
			prevInteractObject->InteractExit(this, playerID);
		}

		prevInteractObject = curInteractObject;
	}

#endif 

	if (InputManagement->IsControllerConnected(playerID)) {
		Vector2 dir = InputManagement->GetControllerThumbL(playerID);
		
		if (pow(dir.x, 2) + pow(dir.y, 2) < 0.04f) {
			dir.x = 0.f;
			dir.y = 0.f;
		}
		dir.y = -dir.y;
		controller->direction = dir;
		if (dir.Length() > 0) {
			_state.isMoveing = true;
		}
		else {
			_state.isMoveing = false;
		}

		if (controller->contactCallback.isGround) {
			_state.isJumping = false;
		}
		else {
			_state.isJumping = true;
		}

		if ((InputManagement->IsControllerButtonDown(playerID, ControllerButton::A) || InputManagement->IsKeyPressed(' '))) {
			//controller->Jump();
			if (controller->contactCallback.isGround) {
				controller->contactCallback.isGround = false;
				float vel[3] = { 0.f, controller->jumpVelocity, 0.f };
				controller->AddForceChangeVelocity(vel);
			}
			/*std::vector<BoxShape> boxshapes;
			Physics->GetShapes(boxshapes);
			for (auto& b : boxshapes) {
				std::cout <<"worldpos : " << b.worldPosition[0]<< ", " << b.worldPosition[1] << ", " << b.worldPosition[2] << std::endl;
				std::cout <<"worldrot : " << b.worldRotation[0]<< ", " << b.worldRotation[1] << ", " << b.worldRotation[2] << ", " << b.worldRotation[3] << std::endl;
				std::cout << "halfsize : " << b.halfSize[0] << ", " << b.halfSize[1] << ", " << b.halfSize[2] << std::endl;
			}*/
		}
		//if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::X)) {
		//	if (controller->otherhit != nullptr) {
		//		controller->otherhit->Jump();
		//	}
		//}

		IgnoreSelfQueryFilterCallback ignore(controller->controller->getActor());
		PxQueryFilterData filterData;
		//filterData.flags
		filterData.data.word0 = PhysFilter::WORD1;

		// 레이 쏠 때 얘 써서 무시하기.
		if (IsHeldObject()) {
			
			auto pos = _position;
			pos.y += 0.1f;
			heldObject->SetPosition(pos);

			auto heldrb = heldObject->GetComponent<RigidbodyComponent>();
			if (heldrb) {
				static_cast<physx::PxRigidDynamic*>(heldrb->rbActor)->setLinearVelocity({ 0.f,0.f,0.f });
			}
			
			auto otherControl = heldObject->GetComponent<CharacterController>();
			if (otherControl != nullptr) {
				pos *= 10.f;
				pos.y += 1.f;
				otherControl->controller->setPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
			}
			else {
				
			}
		}
		if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::B)) {
			// 놓기 버튼이 될 예정.
			if (IsHeldObject()) {
				Sound->playSound("Intractn_PutDown", ChannelType::SFX);
				auto rb = heldObject->GetComponent<RigidbodyComponent>();
				if (rb) {
					// 리지드바디 있으면
					float zero[3] = { 0.f, 0.f, 0.f };
					rb->AddForce(zero, ForceMode::VELOCITY_CHANGE);

					if (heldObject->GetType() == MetaType<Torch>::type) {
						static_cast<Torch*>(heldObject)->SetFire(false);
						auto render = heldObject->GetComponent<RenderComponent>();
						render->SetVisiable(true);
					}

					Vector2 forceDirection = controller->currDirection * _throwVelocity;
					float velocity[3] = { forceDirection.x, 0.f, forceDirection.y };
					rb->AddForce(velocity, ForceMode::IMPULSE);
					heldObject = nullptr;
				}
				else {
					auto otherControl = heldObject->GetComponent<CharacterController>();
					if (otherControl) {
						// 컨트롤러 있으면
						//otherControl->Jump();
						Vector2 forceDirection = controller->currDirection * _throwVelocity;
						float velocity[3] = { forceDirection.x, 7.f, forceDirection.y };
						otherControl->AddForceChangeVelocityWhenisGround(velocity, 1.f);
						heldObject = nullptr;
					}
				}
			}
			/*
			auto curdir = controller->currDirection;
			Vector3 dir = { curdir.x, 0.f, curdir.y };
			dir.Normalize();
			float radius = controller->attribute.radius * 1.2f;
			auto position = GetLocalPosition() * 10.f;
			auto originPos = curdir * radius;
			position.x += originPos.x;
			position.z += originPos.y;
			auto coll = raycast(position, dir, 1.f, filterData, &ignore);
			if (coll != nullptr) {
				std::cout << coll->GetOwner()->GetName() << std::endl;
			}
			*/
		}
		{
			// 상시 탐색 상태.
			
			auto curdir = controller->currDirection;
			Vector3 dir = { curdir.x, 0.f, curdir.y };
			dir.Normalize();
			float radius = controller->attribute.radius * 1.1f;
			//auto position = GetLocalPosition() * 10.f;
			auto test = GetLocalPosition() * 10.f;



			auto position = controller->controller->getFootPosition();
			position.y += 0.1f;	// 레이 위치를 컨트롤러 바닥에서 처리하도록 수정.
			Vector3 toDxVec = { (float)position.x, (float)position.y, (float)position.z };

			auto originPos = curdir * radius;
			//position.x += originPos.x;
			//position.z += originPos.y;

			toDxVec.x += originPos.x;
			toDxVec.z += originPos.y;

			//auto coll = raycast(position, dir, 1.f, filterData, &ignore);
			auto coll = raycast(toDxVec, dir, 1.f, filterData, &ignore);
			Object* curInteractObject = nullptr;
			if (coll != nullptr) {
				//coll->GetOwner()->Interact(playerID, tick);
				curInteractObject = coll->GetOwner();
			}

			if (prevInteractObject == nullptr && curInteractObject != nullptr) {
				curInteractObject->InteractEnter(this, playerID);
			}
			else if (prevInteractObject != nullptr && curInteractObject != nullptr && prevInteractObject != curInteractObject) {
				prevInteractObject->InteractExit(this, playerID);
				curInteractObject->InteractEnter(this, playerID);
			}
			else if (prevInteractObject != nullptr && curInteractObject != nullptr && prevInteractObject == curInteractObject) {
				curInteractObject->Interact(this, playerID, tick);
			}
			else if (prevInteractObject != nullptr && curInteractObject == nullptr) {
				prevInteractObject->InteractExit(this, playerID);
			}

			prevInteractObject = curInteractObject;
		}
	}

	
	if (_btComponent != nullptr) {
		_btComponent->updateState<PlayerBT>(_state);
		//_btComponent->Update(tick);
	}
	else {
		//std::cout << "btComponent is nullptr. bt is not Deserialize. when you last build, check this comment" << std::endl;
		_btComponent = AddComponent<BehaviorTreeComponent>(std::make_unique<PlayerBT>(this));

		// bt를 기본적으로 생성 가능하게 하고 등록을 따로 가능하도록 추가해야 할듯합니다.
	}
}

void Player::LateUpdate(float tick)
{
	Object::LateUpdate(tick);
}


void Player::Destroy()
{
	//todo : Player Destroy
}

void Player::Serialize(_inout json& out)
{
	json data;

	data["components"] = json::array();
	data["componentCount"] = _components.size();
	data["scale"] = { _scale.x, _scale.y, _scale.z };
	data["rotation"] = { _rotation.x, _rotation.y, _rotation.z };
	data["position"] = { _position.x, _position.y, _position.z };
	data["player"]["playerID"] = playerID;
	data["player"]["speed"] = _initSpeed;
	data["player"]["deseaseSpeed"] = _deseaseSpeed;

	data["name"] = _name;
	data["type"] = MetaType<Player>::type;

	for (auto& component : _components)
	{
		component->Serialize(data);
	}

	out["World"]["objects"].push_back(data);
}

void Player::DeSerialize(_in json& in)
{
	_position = Mathf::jsonToVector3(in["position"]);
	_rotation = Mathf::jsonToVector3(in["rotation"]);
	_scale = Mathf::jsonToVector3(in["scale"]);

	if (!in["player"]["playerID"].empty())
	{
		playerID = in["player"]["playerID"].get<int>();
		if(0 == playerID)
		{
			GameManagement->SetPlayer1(this);
		}
		else if (1 == playerID)
		{
			GameManagement->SetPlayer2(this);
		}
	}

	if(!in["player"]["speed"].empty())
		_initSpeed = in["player"]["speed"].get<float>();
	if(!in["player"]["deseaseSpeed"].empty())
		_deseaseSpeed = in["player"]["deseaseSpeed"].get<float>();
}

void Player::EditorContext()
{
	ImGui::Text("PlayerObject");

	ImGui::Spacing();

	ImGui::DragInt("playerID", &playerID, 1, 0,1);
	ImGui::DragFloat("initSpeed", &_initSpeed);
	ImGui::DragFloat("deseaseSpeed", &_deseaseSpeed);

	if (ImGui::Button("SetPlayer in GameObject")) {
		if (0 == playerID)
		{
			GameManagement->SetPlayer1(this);
		}
		else if (1 == playerID)
		{
			GameManagement->SetPlayer2(this);
		}
	}

	if (ImGui::Button("Delete Controller")) {
		GetComponent<CharacterController>()->SetDestroy();
		controller = nullptr;
	}

	ImGui::Spacing();

	if (ImGui::Button("Active")) {
		_active = !_active;
	}

	if (ImGui::Button("AddBehaviorTree")) {
		_btComponent = AddComponent<BehaviorTreeComponent>(std::make_unique<PlayerBT>(this));
	}
}

void Player::OnTriggerEnter(ICollider* other)
{
}

void Player::OnTriggerStay(ICollider* other)
{
}

void Player::OnTriggerExit(ICollider* other)
{
}

void Player::OnCollisionEnter(ICollider* other)
{
	PxQueryFilterData filterData;
	filterData.data.word0 = 1; //item -> 2  여기에 이거 하는거 맞나?
}

void Player::OnCollisionStay(ICollider* other)
{
}

void Player::OnCollisionExit(ICollider* other)
{
}

std::string_view Player::GetType()
{
	return MetaType<Player>::type;
}

bool Player::IsIgnoreInteract()
{
	return false;
}

void Player::InteractEnter(Object* thisPlayer, int playerID)
{
	HighLightOn();
}

void Player::Interact(Object* thisPlayer, int playerID, float tick)
{
	if (InputManagement->IsControllerButtonDown(playerID, ControllerButton::X) || InputManagement->IsKeyPressed('X')) {
		static_cast<Player*>(thisPlayer)->SetHeldObject(this);
	}
}

void Player::InteractExit(Object* thisPlayer, int playerID)
{
	HighLightOff();
}

void Player::MoveSound()
{
	Sound->playSound("Player_Footstep", ChannelType::SFX);
}

void Player::JumpSound()
{
	Sound->playSound("Player_Jump", ChannelType::SFX);
}

void Player::BongsuHeat(int trigger)
{
	_isHeat += trigger;
}

void Player::SetHeldObject(Object* heldObj)
{
	if (heldObj == nullptr) {
		if (heldObject != nullptr) {
			if (heldObject->GetType() == MetaType<Torch>::type) {
				auto torch = static_cast<Torch*>(heldObject);
				torch->SetFire(false);
				auto render = torch->GetComponent<RenderComponent>();
				render->SetVisiable(true);
			}
		}
	}
	else {
		if (heldObj->GetType() == MetaType<Torch>::type) {
			auto torch = static_cast<Torch*>(heldObj);
			auto render = torch->GetComponent<RenderComponent>();
			render->SetVisiable(false);
		}
	}

	heldObject = heldObj;
}

void Player::Damage(Object* hyina, float power)
{
	// 손에 있는 오브젝트를 놓침.
	if (heldObject != nullptr) {
		
		SetHeldObject(nullptr);
	}
	auto otherPos = hyina->GetLocalPosition();
	auto dir = _position - otherPos;
	dir.Normalize();

	float velocity[3] = { dir.x * power, 2.f * power, dir.z * power};

	_isStun = true;

	controller->AddForceChangeVelocityWhenisGround(velocity);


	// 받는 데미지 처리
}

