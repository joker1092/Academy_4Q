#pragma once
#include "InteractableComponent.h"
#include "Physics/Common.h"
class InteractFirewood : public InteractableComponent
{
public:
	// InteractableComponent을(를) 통해 상속됨
	bool IsInteractable() override;
	void Interact() override;
	void Interact(Object* obj) override;
	void Initialize() override;
	void FixedUpdate(float fixedTick) override;
	void Update(float tick) override;
	void LateUpdate(float tick) override;
	void EditorContext() override;
	void Serialize(_inout json& out) override;
	void DeSerialize(_in json& in) override;
	void OnTriggerEnter(ICollider* other) override;
	void OnTriggerStay(ICollider* other) override;
	void OnTriggerExit(ICollider* other) override;
	void OnCollisionEnter(ICollider* other) override;
	void OnCollisionStay(ICollider* other) override;
	void OnCollisionExit(ICollider* other) override;
	void BurnOut() override;
	uint32 ID() override { return 0; }

private:
	Object* interactingObject = nullptr;	// 상호작용 중인 오브젝트
	int interactCount = 0;
	int maxInteractCount = 5;
	float timer = 0.f;
	float maxTimer = 0.2f;
};

