#pragma once
#include "Component.h"
#include "RenderEngine/Scene.h"
#include "ObjectTypeMeta.h"


static uint32 f_id;

class TorchComponet final : public Component
{
public:
	TorchComponet(Scene* scene);
	virtual ~TorchComponet() = default;
	virtual void Initialize() override final {};
	virtual void FixedUpdate(float fixedTick) override final {};
	virtual void Update(float tick) override;
	virtual void LateUpdate(float tick) override final {};
	virtual void EditorContext() override;
	virtual void Serialize(_inout json& out) override final;
	virtual void DeSerialize(_in json& in) override final;

	void Start() { _isStart = true; }

	uint32 ID() override final
	{
		return _ID;
	}

	enum StateFire
	{
		None,
		FireIn,
		FireLoop,
		FireOut
	};

	void FireOn();
	void FireOff();

private:
	void FireInUpdate();
	void FireLoopUpdate();
	void FireOutUpdate();

private:
	static constexpr uint32 _ID{ UINT32_MAX - 2 };
	Scene* _targetScene{};
	InstancedBillboard* _instancedFireInBillboard{};
	InstancedBillboard* _instancedFireLoopBillboard{};
	uint32 _instancedID{};
	Mathf::Vector3 _centerOffset{};
	float size = 1.f;
	bool _isDeSerialize{ true };
	bool _isStart{ false };
	float _frameTime{ 0.01f };
	float _time{};
	StateFire _state{ FireOut };
	float _AccumulatedTime{};
	float _StateDuration{ 3.f };

	bool _isSubscribe = false;

	float prevTime = 0.f;
	float outTime = 0.8f;
};

template<>
struct MetaType<TorchComponet>
{
	static constexpr std::string_view type{ "TorchComponet" };
};

