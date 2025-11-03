#include "InteractFirewood.h"

bool InteractFirewood::IsInteractable()
{
	return false;
}

void InteractFirewood::Interact()
{
}

void InteractFirewood::Interact(Object* obj)
{
	if (interactingObject == nullptr) {
		interactingObject = obj;
	}
	else {
		if (interactingObject == obj && timer <= 0.f) {
			interactCount++;
			timer = maxTimer;
			// 클릭에 대한 효과도 이부분에서 처리하면 될거같습니다.
			if (interactCount > maxInteractCount) {
				// 장작을 다 패면.
				// 장작 스폰.
				std::cout << "spawn firewood." << std::endl;
				interactCount = 0;
			}
		}
	}
}

void InteractFirewood::Initialize()
{
}

void InteractFirewood::FixedUpdate(float fixedTick)
{
}

void InteractFirewood::Update(float tick)
{
	if (timer > 0.f) {
		timer -= tick;
	}
}

void InteractFirewood::LateUpdate(float tick)
{
}

void InteractFirewood::EditorContext()
{
}

void InteractFirewood::Serialize(_inout json& out)
{
}

void InteractFirewood::DeSerialize(_in json& in)
{
}

void InteractFirewood::OnTriggerEnter(ICollider* other)
{
}

void InteractFirewood::OnTriggerStay(ICollider* other)
{
}

void InteractFirewood::OnTriggerExit(ICollider* other)
{
	if (interactingObject != nullptr) {
		if (interactingObject == other->GetOwner())
		{
			// 상호작용중인 오브젝트가 해당 범위를 벗어남.
			// 현재 진행중인 행동을 종료.
			interactingObject = nullptr;
		}
	}
}

void InteractFirewood::OnCollisionEnter(ICollider* other)
{
}

void InteractFirewood::OnCollisionStay(ICollider* other)
{
}

void InteractFirewood::OnCollisionExit(ICollider* other)
{
}

void InteractFirewood::BurnOut()
{
}
