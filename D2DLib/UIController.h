#pragma once
#include "UIEventManager.h"

class UIController final : public Singleton<UIController>
{
private:
	friend class Singleton;

	UIController() = default;
	~UIController() = default;

public:
	void EventInitialize();
	void AddOpacityEventUI(int layer, UIObject* obj);
	void ClearOpacityEventUI();
	void AddSelectEventUI(int layer, UIObject* obj);
	void DeSerializeOpacityEventUI(int layer, UIObject* obj);
	void DeSerializeSelectEventUI(int layer, UIObject* obj);
	void ClearSelectEventUI();
	void AddClickEventUI(UIObject* obj, int value);
	void AddClippingEventUI(UIObject* obj);
	void DeSerializeClippingEventUI(UIObject* obj);
	void ClearClippingEventUI();
	void SetCursorEventUI(UIObject* obj) { _cursorEventUI = obj; }

	void AddSelectedEventUI(int layer, UIObject* obj);
	void DeSerializeSelectedEventUI(int layer, UIObject* obj);

	void AddScoreEventUI(int layer, UIObject* obj);
	void DeSerializeScoreEventUI(int layer, UIObject* obj);

	void AddStateEventUI(UIObject* obj, int value);
	void DeSerializeStateEventUI(UIObject* obj, int value) { _playerStateEventUI[value] = obj; }
	void ClearStateEventUI() { _playerStateEventUI[0] = nullptr; _playerStateEventUI[1] = nullptr; }

	void AddFarBongsuEventUI(UIObject* obj, int value);
	void DeSerializeFarBongsuEventUI(UIObject* obj, int value) { _FarBongsuEventUI[value] = obj; }

	void AddInteractEventUI(UIObject* obj, int value);
	void DeSerializeInteractEventUI(UIObject* obj, int value) { _interactEventUI[value] = obj; }

	void SetCheckListEventUI(UIObject* obj);
	void DeSerializeCheckListEventUI(UIObject* obj);
	void SetStageImgEventUI(UIObject* obj);
	void DeSerializeStageImgEventUI(UIObject* obj);
	void SetDescEventUI(UIObject* obj);
	void DeSerializeDescEventUI(UIObject* obj);

	void SetStampEventUI(UIObject* obj);
	void DeSerializeStampEventUI(UIObject* obj) { _StampEventUI = obj; }

	void SetMandukEventUI(UIObject* obj);
	void DeSerializeMandukEventUI(UIObject* obj) { _MandukEventUI = obj; }

	void ControlSelectUI(float deltaTime);

	int GetCurrentLayer() const { return _currentLayer; }
	float _excuteDelay = 0.f;

	void ResetLayer() { _currentLayer = 0; _prevLayer = 0; _selectIndex = 0; }
	void AllClear();
	int _currentLayer = 0;
	bool _isScoreEnd = false;

private:
	std::array<std::vector<UIObject*>, 10> _opacityEventUI;
	std::array<std::vector<UIObject*>, 10> _selectEventUI;
	std::array<std::deque<UIObject*>, 10> _selectedEventUI;
	std::vector<std::pair<UIObject*, int>> _clickEventUI;
	std::array<std::vector<UIObject*>, 10> _scoreEventUI;
	UIObject* _MandukEventUI{};
	UIObject* _StampEventUI{};
	UIObject* _StageDialogEventUI{};
	std::array<UIObject*, 5>			   _FarBongsuEventUI;
	std::array<UIObject*, 2>			   _interactEventUI;
	UIObject* _checkListEventUI{};
	UIObject* _stageImgEventUI{};
	UIObject* _descEventUI{};
	UIObject* _cursorEventUI{};
	std::array<UIObject*, 2> _playerStateEventUI;
	std::vector<UIObject*> _clippingEventUI;
	int _selectIndex = 0;
	std::array<int, 10> _cur_layer_size{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	int _prevLayer = 0;
	int _stageTotalScore = 0;
	float _inputDelay = 0.35f;
};

inline static auto& UIControl = UIController::GetInstance();