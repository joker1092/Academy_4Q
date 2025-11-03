#include "GameManager.h"
#include "SoundManager.h"
#include "RenderEngine/ImGuiRegister.h"
#include "Utility_Framework/SimpleIniFile.h"
#include "D2DLib/UIController.h"
#include "FlagObject.h"

void GameManager::Initialize()
{
	//todo : 게임 초기화 코드를 여기에 추가합니다.
	//전체초기화 일지 부분초기화일지는 나중에 결정
	//필요 JSON 파일명 : "MainMenu.json", "SelectStage.json", "Stage1.json", "Stage2.json", "Stage3.json", "Stage4.json"
	//아니라면 일일이 박아놓을것
	_stageList.reserve((int)GameLevel::MaxGameLevel);
	_stageList.push_back("MainMenu_V8.json");
	_stageList.push_back("stage01_v057.json");
	_stageList.push_back("stage02_v054.json");
	_stageList.push_back("stage03_v054.json");
	_stageList.push_back("stage04_v055.json");
	////==============================================================================================================
	////월드 전환시 해당 스테이지 웨이브정보를 저장한 파일을 로드 --> 씬전환으로 뺄것

	file::path path = PathFinder::Relative("\\Setting.ini");
	SimpleIniFile iniFile;
	try
	{
		iniFile.Load(path.string());
	}
	catch (const std::exception& e)
	{
		Log::Error(e.what());
	}
	//일단 뭘 빼드려야 할지 모르겠음.
	_waveTime		  = std::stof(iniFile.GetValue("GameManager", "WaveTime", "30.0f"));
	_stageTime		  = std::stof(iniFile.GetValue("GameManager", "StageTime", "300.0f"));
	_farSleepTime	  = std::stof(iniFile.GetValue("GameManager", "FarSleepTime", "0.0f"));

	ImGui::ContextRegister("GameManager Inspactor", [&] 
	{
		ImGui::Text("GameManager Inspactor");
		ImGui::Text("GameState : %d", (int)_gameState);
		ImGui::Text("GameLevel : %d", (int)_gameLevel);
		ImGui::Text("TimeScale : %f", _timeScale);
		ImGui::Text("WaveTime : %f", _waveTime);
		ImGui::DragFloat("WaveLeftTime", &_waveLeftTime, 0.1f, 0.0f, 100.0f);
		ImGui::Checkbox("WaveStart", &_waveStart);
		ImGui::Text("StageTime : %f", _stageTime);
		ImGui::Text("Time : %f", _time);
		ImGui::InputInt("WaveCount : %d", &_waveCount);
		ImGui::Text("WaveClear : %d", _waveClear);
		ImGui::Text("IsGameOver : %d", _isGameOver);
		ImGui::Text("CheckStatus : %d", _checkStatus);
		ImGui::Text("CorrectNumber : %d", _correctNumber);
		ImGui::Text("CorrectAgility : %d", _correctAgility);
		ImGui::Text("PlayerFrozen : %d", _playerFrozen);
		ImGui::Text("FiledClean : %d", _filedClean);
		ImGui::Text("FarSleepTime : %f", _farSleepTime);
		ImGui::Text("MonsterLiveTime : %f", _monsterLiveTime);
		ImGui::Text("BongsuUpdateTime : %f", _bongsuUpdateTime);
		ImGui::Text("Far_isSleep : %d", Far_isSleep);
		ImGui::Text("InGameBongsu : %d %d %d %d %d", InGameBongsu[0], InGameBongsu[1], InGameBongsu[2], InGameBongsu[3], InGameBongsu[4]);
		ImGui::Text("FarFlag : %d", farflag);
		ImGui::Text("IngameFlag : %d", ingameflag);
		ImGui::Text("StageList : %d", _stageList.size());
		float _waveClippingPercentage = (float)_waveLeftTime / _waveTime;
		ImGui::Text("WaveClippingPercentage : %f", _waveClippingPercentage);

		ImGui::Separator();
		if (_player1)
		{
			ImGui::PushID(0);
			ImGui::Text("Player1");
			ImGui::Checkbox("Stun : %d", &_player1->GetPlayerState().isStun);
			ImGui::DragFloat("CurrentCold : %f", &_player1->_currentCold);
			ImGui::PopID();
		}

		if (_player2)
		{
			ImGui::PushID(1);
			ImGui::Text("Player2");
			ImGui::Checkbox("Stun : %d", &_player2->GetPlayerState().isStun);
			ImGui::DragFloat("CurrentCold : %f", &_player2->_currentCold);
			ImGui::PopID();
		}

		ImGui::Separator();
		ImGui::Text("FarBongsu");
		ImGui::InputInt("1", &FarBongsu[0]);
		ImGui::InputInt("2", &FarBongsu[1]);
		ImGui::InputInt("3", &FarBongsu[2]);
		ImGui::InputInt("4", &FarBongsu[3]);
		ImGui::InputInt("5", &FarBongsu[4]);

		ImGui::Separator();
		ImGui::Text("Result Test Instector");
		if (ImGui::Button("Test Result"))
		{
			_isGameOver = !_isGameOver;
		}
		ImGui::Text("Result : %d", _isGameOver);
		ImGui::InputInt("CorrectNumber", &_correctNumber);
		ImGui::InputInt("CorrectAgility", &_correctAgility);
		ImGui::InputInt("PlayerFrozen", &_playerFrozen);
		ImGui::InputInt("FiledClean", &_filedClean);
		//_stageTime - (_monsterLiveTime+ _farSleepTime)
		ImGui::InputFloat("StageTime", &_stageTime);
		ImGui::InputFloat("MonsterLiveTime", &_monsterLiveTime);
		ImGui::InputFloat("FarSleepTime", &_farSleepTime);
		int emergency = _stageTime - (_monsterLiveTime + _farSleepTime);
		ImGui::Text("Emergency : %d", emergency);
	});

	//Sound->playSound("Test_IngameMusic", ChannelType::BGM);
	
}

void GameManager::Update(float tick)
{

}

void GameManager::PlayUpdate(float tick)
{
	//debug
	_enemyRespawnTime -= tick; //몬스터 리스폰 시간 감소


	if(!_waveStart || _gameLevel == GameManager::GameLevel::MainMenu)
	{
		return;
	}

	Event->Publish("Bungsu System", &FarBongsu); //FarBongsu 이벤트 발생
	//_enemyRespawnTime -= tick; //몬스터 리스폰 시간 감소

	if (_player1)
	{
		if (_player1->GetPlayerState().isStun)
		{
			UIEvent->Publish("StunState", 0);
		}
		else if (_player1->_isDesease || 100.f <= _player1->_currentCold)
		{
			UIEvent->Publish("FrozenState", 0);
		}
		else if (50.0f <= _player1->_currentCold)
		{
			UIEvent->Publish("ColdState", 0);
		}
		else
		{
			UIEvent->Publish("IdleState", 0);
		}
	}

	if (_player2)
	{
		if (_player2->GetPlayerState().isStun)
		{
			UIEvent->Publish("StunState", 1);
		}
		else if (_player2->_isDesease || 100.f <= _player2->_currentCold)
		{
			UIEvent->Publish("FrozenState", 1);
		}
		else if (50.0f <= _player2->_currentCold)
		{
			UIEvent->Publish("ColdState", 1);
		}
		else
		{
			UIEvent->Publish("IdleState", 1);
		}
	}

	_waveLeftTime -= tick; // 웨이브 시간 감소
	_bongsuUpdateTime += tick; // 봉수대 업데이트 시간 증가

	if (_waveLeftTime < 0) // 웨이브 시간이 지나면
	{
		if (FarBongsuList.size() == 0) { return; }
		//todo : 웨이브 종료
		_waveCount++; // 웨이브 카운트 증가
		auto& next = FarBongsuList[_waveCount]; // 다음 웨이브 정보
		if (next[5] > 0) //6번째 Far_isSleep 값이 1이면
		{
			Far_isSleep = true; //Far_isSleep를 true로 설정
		}
		else
		{
			Far_isSleep = false; //Far_isSleep를 false로 설정
		}

		if (Far_isSleep)
		{
			FarBongsu = { 0,0,0,0,0 };
		}
		else 
		{
			FarBongsu = { next[0],next[1],next[2],next[3],next[4] }; //FarBongsu에 다음 웨이브 정보 설정
		}
		
		_waveLeftTime = _waveTime; //시간 초기화
		_waveClear = false; //웨이브 클리어 초기화
		//Event->Publish("FarBongsu", &FarBongsu); //FarBongsu 이벤트 발생
		Sound->playSound("OtherBongsu_On", ChannelType::SFX); //건너편 봉수대 신호 온
	}

	if (_bongsuUpdateTime > 1.0f)
	{
		CompareBongsu(); //봉수대 비교
	}

	if (_waveCount > 12) 
	{
		//todo : 게임 종료
		_filedClean = currWorld->tempObjectCount(); //임시 오브젝트 겟수 가져오기
		_isGameOver = true;
	}

	
}

bool GameManager::EnemyRespawn(Object* target)
{
	if (target->GetType() == MetaType<FlagObject>::type) {
		FlagObject* flag = static_cast<FlagObject*>(target);

		if ((_enemyCount < _mexEnemyCount) && _enemyRespawnTime <= 0) {
		
			return true;
		}
		else {
			return false;
		}
		return false;
	}

}

void GameManager::SceneChange(int level)
{
	Sound->stopSound(ChannelType::BGM);
	FarBongsuList.clear();
	_gameLevel = (GameLevel)level;

	std::string scenecsv = "";

	switch (_gameLevel)
	{
	case GameManager::GameLevel::MainMenu:
		break;
	case GameManager::GameLevel::Stage1:
		scenecsv = "FarBongsuList_01.csv";
		break;
	case GameManager::GameLevel::Stage2:
		scenecsv = "FarBongsuList_02.csv";
		break;
	case GameManager::GameLevel::Stage3:
		scenecsv = "FarBongsuList_03.csv";
		break;
	case GameManager::GameLevel::Stage4:
		scenecsv = "FarBongsuList_04.csv";
		break;
	case GameManager::GameLevel::Stage5:
		scenecsv = "FarBongsuList_05.csv";
		break;
	default:
		break;
	}

	try
	{
		auto read = CSVReader<int, int, int, int, int, int>(PathFinder::Relative("csv\\").string() + scenecsv);
		read.forEach([&](int a, int b, int c, int d, int e, int f) {
			std::vector<int> temp = { a,b,c,d,e,f };
			FarBongsuList.push_back(temp);
			});
		assert(FarBongsuList.size() == 24);
		auto init = FarBongsuList[0];
		FarBongsu = { init[0], init[1], init[2], init[3], init[4] };
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	
	Sound->playSound("BGM_IngameStage", ChannelType::BGM);

}

int GameManager::GetCorrectScore() const
{
	if (_correctNumber >= 10) {
		return 5;
	}
	if (_correctNumber >= 8)
	{
		return 4;
	}
	if (_correctNumber >= 6)
	{
		return 3;
	}
	if (_correctNumber >= 4)
	{
		return 2;
	}
	if (_correctNumber >= 2)
	{
		return 1;
	}
	return 0;
}

int GameManager::GetSpeedScore() const
{
	if (_correctAgility >= 200)
	{
		return 5;
	}
	if (_correctAgility >= 160)
	{
		return 4;
	}
	if (_correctAgility >= 120)
	{
		return 3;
	}
	if (_correctAgility >= 90)
	{
		return 2;
	}
	if (_correctAgility >= 45)
	{
		return 1;
	}
	return 0;
}


int GameManager::GetEmergencyScore() const
{
	int score = _stageTime - (_monsterLiveTime+ _farSleepTime);

	if (score >= 240)
	{
		return 5;
	}
	if (score >= 180)
	{
		return 4;
	}
	if (score >= 120)
	{
		return 3;
	}
	if (score >= 90)
	{
		return 2;
	}
	if (score >= 45)
	{
		return 1;
	}
	return 0;
}

int GameManager::GetFrozenScore() const
{
	if (_playerFrozen < 1)
	{
		return 5;
	}
	if (_playerFrozen < 3)
	{
		return 4;
	}
	if (_playerFrozen < 5)
	{
		return 3;
	}
	if (_playerFrozen < 7)
	{
		return 2;
	}
	if (_playerFrozen < 9)
	{
		return 1;
	}
	return 0;
}

int GameManager::GetCleanScore() const
{
	//todo : world 종료시 스폰되어 있는 오브젝트 겟수 가져오는 거 필요
	//문서에 양동이 겟수 제외라고 함 -> 임시 겍체 겟수만 세면 줄여야 하나 이건 물어 봅시다
	if (_filedClean < 4)
	{
		return 5;
	}
	if (_filedClean < 5)
	{
		return 4;
	}
	if (_filedClean < 6)
	{
		return 3;
	}
	if (_filedClean < 7)
	{
		return 2;
	}
	if (_filedClean < 8)
	{
		return 1;
	}
	return 0;
}

//점수 관련 변수 초기화
void GameManager::InitScore()
{
	_correctNumber = 0;
	_correctAgility = 0;
	_monsterLiveTime = 0;
	_farSleepTime = 0;
	_playerFrozen = 0;
	_filedClean = 0;
}

void GameManager::PlayDrumBeat()
{
	if (Far_isSleep) 
	{
		Far_isSleep = false;
	}
	else {
		return;
	}
}

//봉수대 비교
void GameManager::CompareBongsu()
{
	//todo : ingame 봉수대 값 가져오기
	//world에 봉수대 값 5개 박기 -> evnet에서 각 봉수대가 변경될때마다 갱신
	//그에 따라 
	
	if (!Far_isSleep)
	{
		//ingamebongsu와 farbongsu 를 비트플래그로 만들어서 비교
		
		for (int i = 0; i < 5; i++)
		{
			//farbongsu 비트플래그;
			farflag |= (1 << FarBongsu[i]);
			//ingamebongsu 비트플래그
			ingameflag |= (1 << InGameBongsu[i]);
		} 
		
		//비교
		if (farflag == ingameflag)
		{
			//todo : 둘이 같을때 
			//점수 추가
			// 웨이브 클리어 체크
			if (!_waveClear)
			{
				_waveClear = true; //waveClear는 웨이브 시작시 false로 초기화
				_correctNumber++;
				Sound->playSound("Bongsu_Correct", ChannelType::SFX); //봉수대 정답 //이건 불이 꺼졌다 다시 켜졌을때 재생시킬지 낮에 확인 필요
			}
			else 
			{
				//todo : 이미 클리어 했을때
				//맞춘 시간을 늘려준다
				_correctAgility++;
			}
		}	
	}
}



