#include "pch.h"
#include "UIController.h"
#include "../EventSystem.h"
#include "../InputManager.h"
#include "../GameManager.h"

void UIController::EventInitialize()
{
	for (auto& vec : _selectEventUI)
	{
		vec.resize(15);
	}

	UIEventManager::GetInstance()->Subscribe("Opacity", [&](int data)
	{
		for (auto& obj : _opacityEventUI[data])
		{
			if(!obj->m_IsLerpOpacity)
			{
				if (obj->m_Opacity == 0.0f)
				{
					obj->m_TargetOpacity = obj->m_OriginOpacity;
				}
				else if (obj->m_Opacity == obj->m_OriginOpacity)
				{
					obj->m_TargetOpacity = 0.0f;
				}
			}
		}

		for (auto& obj : _opacityEventUI[data])
		{
			obj->m_IsLerpOpacity = true;
		}
	});

	UIEventManager::GetInstance()->Subscribe("Popup Open", [&](int data)
	{
		for (auto& obj : _opacityEventUI[data])
		{
			if(0.f != obj->m_OriginOpacity)
			{
				continue;
			}

			if (!obj->m_IsLerpOpacity)
			{
				if (obj->m_Opacity == obj->m_OriginOpacity)
				{
					obj->m_OpacityLerpSpeed = 1.8f;
					obj->m_TargetOpacity = 1.f;
				}
			}
		}

		for (auto& obj : _opacityEventUI[data])
		{
			obj->m_IsLerpOpacity = true;
		}
	});

	UIEventManager::GetInstance()->Subscribe("Popup Close", [&](int data)
	{
		for (auto& obj : _opacityEventUI[data])
		{
			if (0.f != obj->m_OriginOpacity)
			{
				continue;
			}
			if (obj->m_Opacity >= 0.9f)
			{
				obj->m_OpacityLerpSpeed = 1.8f;
				obj->m_TargetOpacity = obj->m_OriginOpacity;
			}
		}
		for (auto& obj : _opacityEventUI[data])
		{
			obj->m_IsLerpOpacity = true;
		}
	});

	UIEventManager::GetInstance()->Subscribe("Select", [&](int data)
	{
		// 컨트롤러 자체 적으로 처리
	});

	UIEventManager::GetInstance()->Subscribe("Selected", [&](int data)
	{
		if (_selectedEventUI[_currentLayer].size() == 0)
		{
			return;
		}

		if (_prevLayer != _currentLayer)
		{
			for (auto& obj : _selectedEventUI[_currentLayer])
			{
				if (obj)
				{
					obj->m_OpacityLerpSpeed = 0.8f;
					obj->m_TargetOpacity = 0.0f;
					obj->m_IsLerpOpacity = true;
				}
			}
		}

		auto obj = _selectedEventUI[_currentLayer][data];
		if (!obj->m_IsLerpOpacity)
		{
			obj->m_TargetOpacity = obj->m_OriginOpacity;
			obj->m_IsLerpOpacity = true;
		}

		for (auto& obj : _selectedEventUI[_currentLayer])
		{
			if (obj && obj != _selectedEventUI[_currentLayer][data] && !obj->m_IsLerpOpacity)
			{
				obj->m_OpacityLerpSpeed = 0.8f;
				obj->m_TargetOpacity = 0.0f;
				obj->m_IsLerpOpacity = true;
			}
		}
	});

	Event->Subscribe("Layer Change 0 -> 1", [&](void* data)
	{
		//int layer = *(int*)data;
		if (_currentLayer == 0)
		{
			_prevLayer = 0;
			_currentLayer = 1;
			_selectIndex = 0;
		}

		if (_prevLayer != _currentLayer)
		{
			UIEventManager::GetInstance()->Publish("Opacity", 0);
		}
	});

	Event->Subscribe("Layer Change 1 -> 0", [&](void* data)
	{
		//int layer = *(int*)data;
		if (_currentLayer == 1)
		{
			_prevLayer = 1;
			_currentLayer = 0;
			_selectIndex = 0;
		}

		if (_prevLayer != _currentLayer)
		{
			UIEventManager::GetInstance()->Publish("Opacity", 0);
		}
	});

	Event->Subscribe("How To Play PopUp Open", [&](void* data) 
	{
		UIEventManager::GetInstance()->Publish("Popup Open", 2);
		_currentLayer = 2;
	});

	Event->Subscribe("How To Play PopUp Close", [&](void* data)
	{
		UIEventManager::GetInstance()->Publish("Popup Close", 2);
		_currentLayer = 0;

	});

	Event->Subscribe("Pause PopUp Open", [&](void* data)
	{
		UIEventManager::GetInstance()->Publish("Popup Open", 3);
		_currentLayer = 3;
	});

	Event->Subscribe("Pause PopUp Close", [&](void* data)
	{
		GameManagement->SetGameState(GameManager::GameState::Play);
		GameManagement->Resume();
		UIEventManager::GetInstance()->Publish("Popup Close", 3);
		_currentLayer = 0;

	});

	Event->Subscribe("Result PopUp Open", [&](void* data)
	{
		UIEventManager::GetInstance()->Publish("Popup Open", 4);
		if (_currentLayer != 4)
		{
			UIEventManager::GetInstance()->Publish("Score", 0);
		}
		_currentLayer = 4;
	});

	UIEventManager::GetInstance()->Subscribe("Stamp", [&](int data)
		{
			if (_stageTotalScore >= 25)
			{
				_StampEventUI->m_Index = 0;
			}
			else if (_stageTotalScore >= 20)
			{
				_StampEventUI->m_Index = 1;
			}
			else if (_stageTotalScore >= 15)
			{
				_StampEventUI->m_Index = 2;
			}
			else if (_stageTotalScore >= 10)
			{
				_StampEventUI->m_Index = 3;
			}
			else
			{
				_StampEventUI->m_Index = 4;
			}
			_StampEventUI->m_Visible = true;
		});

	UIEventManager::GetInstance()->Subscribe("Manduk", [&](int data)
		{
			_MandukEventUI->m_Visible = true;
			if (_stageTotalScore >= 25)
			{
				_MandukEventUI->m_Index = 0;
			}
			else if (_stageTotalScore >= 20)
			{
				_MandukEventUI->m_Index = 1;
			}
			else if (_stageTotalScore >= 15)
			{
				_MandukEventUI->m_Index = 2;
			}
			else if (_stageTotalScore >= 10)
			{
				_MandukEventUI->m_Index = 3;
			}
			else
			{
				_MandukEventUI->m_Index = 4;
			}
		});

	UIEventManager::GetInstance()->Subscribe("Score Init", [&](int data)
	{
		for (auto& obj : _scoreEventUI[data])
		{
			obj->m_Visible = false;
			obj->m_Score = 0;
			obj->m_targetScore = 0;
		}

		if(_MandukEventUI && _StampEventUI)
		{
			_isScoreEnd = false;
		}
	});

	Event->Subscribe("Result PopUp Close", [&](void* data)
	{
		UIEventManager::GetInstance()->Publish("Popup Close", 4);
		UIEventManager::GetInstance()->Publish("Score Init", 0);
		GameManagement->SetGameOver(false);
		_currentLayer = 0;
	});

	Event->Subscribe("Bungsu System", [&](void* data) 
	{
		//봉수대 시스템
		std::array<int, 5> farBongsu = *(std::array<int, 5>*)data;
		for (int i = 0; i < 5; i++)
		{
			if (_FarBongsuEventUI[i])
			{
				_FarBongsuEventUI[i]->m_Visible = farBongsu[i];
			}
		}
	});

	UIEventManager::GetInstance()->Subscribe("Click", [&](int data)
	{
		//UI Object 내부로 이동
	});

	UIEventManager::GetInstance()->Subscribe("StateChange", [&](int data)
	{
		//아무 것도 안하는 키값만 가지고 있음.
	});

	UIEventManager::GetInstance()->Subscribe("IdleState", [&](int data) 
	{
		if(nullptr != _playerStateEventUI[data])
		{
			_playerStateEventUI[data]->m_Index = 0;
		}
	});

	UIEventManager::GetInstance()->Subscribe("StunState", [&](int data)
	{
		_playerStateEventUI[data]->m_Index = 4;
	});

	UIEventManager::GetInstance()->Subscribe("ColdState", [&](int data)
	{
		if(!_playerStateEventUI[data])
		{
			return;
		}

		if(1 == _playerStateEventUI[data]->m_Index)
		{
			_playerStateEventUI[data]->m_Index = 1;
		}
		else
		{
			_playerStateEventUI[data]->m_Index = 2;
		}
	});

	UIEventManager::GetInstance()->Subscribe("FrozenState", [&](int data)
	{
		if (!_playerStateEventUI[data])
		{
			return;
		}

		_playerStateEventUI[data]->m_Index = 3;
	});

	UIEventManager::GetInstance()->Subscribe("Clipping", [&](int data)
	{
		std::unordered_map<int, std::function<float()>> clippingMap = {
			{0, [&]() { return GameManagement->GetWaveClippingPercentage(); }},
			{1, [&]() { return GameManagement->GetStageClippingPercentage(); }}
		};

		for (auto& obj : _clippingEventUI)
		{
			auto it = clippingMap.find(obj->m_ClippingID);
			if (it != clippingMap.end())
			{
				obj->m_ViewPercentage = it->second();
			}
		}
	});

	UIEventManager::GetInstance()->Subscribe("CheckList", [&](int data)
	{
		_checkListEventUI->m_Visible = true;
	});

	UIEventManager::GetInstance()->Subscribe("StageImg", [&](int data)
	{
		_stageImgEventUI->m_Visible = true;
	});

	UIEventManager::GetInstance()->Subscribe("Desc", [&](int data)
	{
		_descEventUI->m_Visible = true;
	});

	UIEventManager::GetInstance()->Subscribe("FarBongsu", [&](int data)
	{
	});

	UIEventManager::GetInstance()->Subscribe("Interact", [&](int data)
	{
	});

	UIEventManager::GetInstance()->Subscribe("Score", [&](int data)
	{
		for (auto& obj : _scoreEventUI[data])
		{
			obj->m_Visible = true;
			switch (obj->m_ScoreID)
			{
			case 0:
			{
				obj->m_targetScore = GameManagement->GetCorrectScore();
				break;
			}
			case 1:
			{
				obj->m_targetScore = GameManagement->GetSpeedScore();
				break;
			}
			case 2:
			{
				obj->m_targetScore = GameManagement->GetEmergencyScore();
				break;
			}
			case 3:
			{
				obj->m_targetScore = GameManagement->GetFrozenScore();
				break;
			}
			case 4:
			{
				obj->m_targetScore = GameManagement->GetCleanScore();
				break;
			}
			}
		}
	});

	UIEventManager::GetInstance()->Subscribe("Dialog", [&](int data)
	{
		_StageDialogEventUI->m_Visible = true;
		_StageDialogEventUI->m_Index++;
	});

	UIEventManager::GetInstance()->Subscribe("Dialog Init", [&](int data)
	{
		_StageDialogEventUI->m_Visible = false;
		_StageDialogEventUI = nullptr;
	});

	Event->Subscribe("Manduk unvisible", [&](void* data) 
	{
		_MandukEventUI->m_Visible = false;
	});

	Event->Subscribe("Stamp unvisible", [&](void* data)
	{
		_StampEventUI->m_Visible = false;
		_stageTotalScore = 0;
	});

	Event->Subscribe("Stage Img Change", [&](void* data)
	{
		int index = *(int*)data;
		_stageImgEventUI->m_Index = index;
		_descEventUI->m_Index = index;
		_checkListEventUI->m_Index = index;
	});

	Event->Subscribe("P1_ColdStateText", [&](void* data)
	{
		std::wstring* dataStr = (std::wstring*)data;
		(*dataStr) = std::to_wstring(GameManagement->GetPlayerCold(0)) + L"%";
	});

	Event->Subscribe("P2_ColdStateText", [&](void* data)
	{
		std::wstring* dataStr = (std::wstring*)data;
		(*dataStr) = std::to_wstring(GameManagement->GetPlayerCold(1)) + L"%";
	});

	Event->Subscribe("InteractExtinguish", [&](void* data) 
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 0;
	});

	Event->Subscribe("InteractTorch", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 1;
	});

	Event->Subscribe("InteractBurn", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 2;
	});

	Event->Subscribe("InteracDrum", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 3;
	});

	Event->Subscribe("InteractPick", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 4;
	});

	Event->Subscribe("InteractSlash", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 5;
	});

	Event->Subscribe("InteractWater", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 6;
	});

	Event->Subscribe("InteractWood", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = true;
		_interactEventUI[dataInt]->m_Index = 7;
	});

	Event->Subscribe("InteractNone", [&](void* data)
	{
		int dataInt = *(int*)data;
		_interactEventUI[dataInt]->m_Visible = false;
	});

}

void UIController::AddOpacityEventUI(int layer, UIObject* obj)
{
	_opacityEventUI[layer].push_back(obj);
	obj->m_EventType.push_back(UIEventType::OnOpacity);
}

void UIController::ClearOpacityEventUI()
{
	for (auto& obj : _opacityEventUI)
	{
		obj.clear();
	}
}

void UIController::AddSelectEventUI(int layer, UIObject* obj)
{
	int currentindex = _cur_layer_size[layer] - 1;
	_selectEventUI[layer][currentindex] = obj;
	obj->m_SelectID = currentindex;
	_cur_layer_size[layer]++;
	obj->m_EventType.push_back(UIEventType::OnSelect);
}

void UIController::DeSerializeOpacityEventUI(int layer, UIObject* obj)
{
	_opacityEventUI[layer].push_back(obj);
}

void UIController::DeSerializeSelectEventUI(int layer, UIObject* obj)
{
	_selectEventUI[layer][obj->m_SelectID] = obj;
	_cur_layer_size[layer]++;
}

void UIController::ClearSelectEventUI()
{
	for (auto& obj : _selectEventUI)
	{
		obj.clear();
	}
}

void UIController::AddClickEventUI(UIObject* obj, int value)
{

}

void UIController::AddClippingEventUI(UIObject* obj)
{
	_clippingEventUI.push_back(obj);
	obj->m_EventType.push_back(UIEventType::OnClipping);
}

void UIController::DeSerializeClippingEventUI(UIObject* obj)
{
	_clippingEventUI.push_back(obj);
}

void UIController::ClearClippingEventUI()
{
	_clippingEventUI.clear();
}

void UIController::AddSelectedEventUI(int layer, UIObject* obj)
{
	_selectedEventUI[layer].push_back(obj);
	obj->m_EventType.push_back(UIEventType::OnSelected);
}

void UIController::DeSerializeSelectedEventUI(int layer, UIObject* obj)
{
	_selectedEventUI[layer].push_front(obj);
}

void UIController::AddScoreEventUI(int layer, UIObject* obj)
{
	_scoreEventUI[layer].push_back(obj);
	obj->m_EventType.push_back(UIEventType::OnScore);
}

void UIController::DeSerializeScoreEventUI(int layer, UIObject* obj)
{
	_scoreEventUI[layer].push_back(obj);
}

void UIController::AddStateEventUI(UIObject* obj, int value)
{
	_playerStateEventUI[value] = obj;
	obj->m_PlayerID = value;
	obj->m_EventType.push_back(UIEventType::OnStateChange);
}

void UIController::AddFarBongsuEventUI(UIObject* obj, int value)
{
	_FarBongsuEventUI[value] = obj;
	obj->m_FarBongsuID = value;
	obj->m_EventType.push_back(UIEventType::OnFarBongsu);
}

void UIController::AddInteractEventUI(UIObject* obj, int value)
{
	_interactEventUI[value] = obj;
	obj->m_PlayerID = value;
	obj->m_EventType.push_back(UIEventType::OnInteract);
}

void UIController::SetCheckListEventUI(UIObject* obj)
{
	_checkListEventUI = obj;
	obj->m_EventType.push_back(UIEventType::OnCheckList);
}

void UIController::DeSerializeCheckListEventUI(UIObject* obj)
{
	_checkListEventUI = obj;
}

void UIController::SetStageImgEventUI(UIObject* obj)
{
	_stageImgEventUI = obj;
	obj->m_EventType.push_back(UIEventType::OnStageImage);
}

void UIController::DeSerializeStageImgEventUI(UIObject* obj)
{
	_stageImgEventUI = obj;
}

void UIController::SetDescEventUI(UIObject* obj)
{
	_descEventUI = obj;
	obj->m_EventType.push_back(UIEventType::OnDescription);
}

void UIController::DeSerializeDescEventUI(UIObject* obj)
{
	_descEventUI = obj;
}

void UIController::SetStampEventUI(UIObject* obj)
{
	_StampEventUI = obj;
	obj->m_EventType.push_back(UIEventType::OnStamp);
}

void UIController::SetMandukEventUI(UIObject* obj)
{
	_MandukEventUI = obj;
	obj->m_EventType.push_back(UIEventType::OnManduk);
}

void UIController::ControlSelectUI(float deltaTime)
{
	int score = GameManagement->GetCorrectScore();
	score += GameManagement->GetEmergencyScore();
	score += GameManagement->GetCleanScore();
	score += GameManagement->GetFrozenScore();
	_stageTotalScore = score;

	if(_MandukEventUI && _StampEventUI)
	{

		if (_isScoreEnd)
		{
			UIEvent->Publish("Stamp", 0);
			UIEvent->Publish("Manduk", 0);
		}
		else
		{
			Event->Publish("Manduk unvisible", nullptr);
			Event->Publish("Stamp unvisible", nullptr);
		}
	}

	if (nullptr == _selectEventUI[_currentLayer][0])
	{
		//잘못 설정된 UI가 있을 경우 정렬
		for (int i = 0; i < _cur_layer_size[_currentLayer]; i++)
		{
			if (1 <= i && _selectEventUI[_currentLayer][i] && !_selectEventUI[_currentLayer][i - 1])
			{
				_selectEventUI[_currentLayer][i - 1] = _selectEventUI[_currentLayer][i];
				_selectEventUI[_currentLayer][i] = nullptr;
				_selectEventUI[_currentLayer][i - 1]->m_SelectID = i - 1;
			}
		}
		return;
	}

	if (_prevLayer != _currentLayer)
	{
		_selectIndex = 0;
		_prevLayer = _currentLayer;
	}

	float2 thumbL{};
	if (_inputDelay > 0.0f)
	{
		_inputDelay -= deltaTime;
		return;
	}
	else
	{
		thumbL = InputManager::GetInstance()->GetControllerThumbL(0);
		_inputDelay = 0.135f;
	}

	if (thumbL.y > 0.23f || thumbL.x > 0.23f)
	{
		_selectIndex++;
		if (_selectIndex >= _cur_layer_size[_currentLayer] - 1)
		{
			_selectIndex = _cur_layer_size[_currentLayer] - 2;
		}
	}
	else if (thumbL.y < -0.23f || thumbL.x < -0.23f)
	{
		_selectIndex--;
		if (_selectIndex < 0)
		{
			_selectIndex = 0;
		}
	}

	if (_selectEventUI[_currentLayer][_selectIndex])
	{
		_selectEventUI[_currentLayer][_selectIndex]->m_IsSelected = true;
	}
	else
	{
		for (int i = 0; i < _cur_layer_size[_currentLayer]; i++)
		{
			if (1 < i && _selectEventUI[_currentLayer][i] && !_selectEventUI[_currentLayer][i - 1])
			{
				_selectEventUI[_currentLayer][i - 1] = _selectEventUI[_currentLayer][i];
				_selectEventUI[_currentLayer][i] = nullptr;
				_selectEventUI[_currentLayer][i - 1]->m_SelectID = i - 1;
			}
		}
	}

	auto view = _selectEventUI[_currentLayer] | take(_cur_layer_size[_currentLayer]);
	foreach(view, [&](UIObject* obj)
	{
		if(!obj)
		{
			return;
		}

		if (obj != _selectEventUI[_currentLayer][_selectIndex])
		{
			obj->m_IsSelected = false;
		}
	});

	UIEventManager::GetInstance()->Publish("Selected", _selectIndex);
	if(_descEventUI)
	{
		_descEventUI->m_Index = _selectIndex;
	}

	if (_stageImgEventUI)
	{
		_stageImgEventUI->m_Index = _selectIndex;
	}

	if (_checkListEventUI)
	{
		_checkListEventUI->m_Index = _selectIndex;
	}

}

void UIController::AllClear()
{
	for (auto& obj : _opacityEventUI)
	{
		obj.clear();
	}
	for (auto& obj : _selectEventUI)
	{
		obj.clear();
	}
	for (auto& obj : _selectedEventUI)
	{
		obj.clear();
	}

	_clickEventUI.clear();

	for (auto& obj : _scoreEventUI)
	{
		obj.clear();
	}
	for (auto& obj : _FarBongsuEventUI)
	{
		obj = nullptr;
	}
	for (auto& obj : _interactEventUI)
	{
		obj = nullptr;
	}
	_checkListEventUI = nullptr;
	_stageImgEventUI = nullptr;
	_descEventUI = nullptr;
	_cursorEventUI = nullptr;
	_StampEventUI = nullptr;
	_MandukEventUI = nullptr;
	_StageDialogEventUI = nullptr;
	_stageTotalScore = 0;
	for (auto& obj : _playerStateEventUI)
	{
		obj = nullptr;
	}
	_clippingEventUI.clear();
	_selectIndex = 0;
	_cur_layer_size = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	_prevLayer = 0;
	_currentLayer = 0;
	_inputDelay = 0.5f;

	for (auto& vec : _selectEventUI)
	{
		vec.resize(15);
	}
}
