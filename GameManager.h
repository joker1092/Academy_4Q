//전역 게임 관리자 클래스
#pragma once
#include "Utility_Framework\Core.Definition.h"
#include "Utility_Framework\Core.Minimal.h"
#include "EventSystem.h"
#include "World.h"
#include "Player.h"

class GameManager : public Singleton<GameManager>
{
private:
	friend class Singleton;
private:
	GameManager() = default;
	~GameManager() = default;

public:
	enum class GameState
	{
		InitState,
		Loading,
		Play,
		Pause,
		Clear,
		GameOver
	};

	enum class GameLevel
	{
		MainMenu = 0,
		Stage1,
		Stage2,
		Stage3,
		Stage4,
		Stage5,
		MaxGameLevel
	};

public:
	void Initialize();
	void Loading();
	void Update(float tick);
	void PlayUpdate(float tick);
	bool EnemyRespawn(Object* target);
public:
	void SetGameState(GameState state) { _gameState = state; }
	GameState GetGameState() const { return _gameState; }

	void SetGameLevel(GameLevel level) { _gameLevel = level; }
	GameLevel GetGameLevel() const { return _gameLevel; }

	void SetWorld(World* world) { currWorld = world; }
	World* GetCurrWorld() const { return currWorld; }

	void SetPlayer1(Player* player) { _player1 = player; }
	Player* GetPlayer1() const { return _player1; }

	int GetPlayerCold(int index) const 
	{
		switch (index)
		{
		case 0:
		{
			if (_player1 != nullptr)
			{
				return _player1->_currentCold;
			}
		}
		case 1:
		{
			if (_player2 != nullptr)
			{
				return _player2->_currentCold;
			}
		}
		default:
			return 0;
		}
	}

	void SetPlayer2(Player* player) { _player2 = player; }
	Player* GetPlayer2() const { return _player2; }

	std::string GetStageName(GameLevel level) const { return _stageList[(unsigned long long)level]; }

	void SetBongsu(int index, int value) { InGameBongsu[index] = value; } //인게임 봉수대 정보 설정
	float GetWaveClippingPercentage() const { return _waveLeftTime / _waveTime; } //웨이브 시간 클리핑 비율
	float GetStageClippingPercentage() const { return _waveCount / 12.f; } //스테이지 시간 클리핑 비율

	float GetTimeScale() const { return _timeScale; }

	void Pause() { _timeScale = 0.0f; }
	void Resume() { _timeScale = 1.0f; }

	void AddMonsterLiveTime(float time) { _monsterLiveTime += time; }
	void AddFrozenTime() { _playerFrozen ++; }
	void AddSleepTime(float time) { _farSleepTime += time; }


	void SceneChange(int level);

	int GetCorrectScore() const;
	int GetSpeedScore() const;
	int GetEmergencyScore() const;
	int GetFrozenScore() const;
	int GetCleanScore() const;

	void SetWaveStart(bool start) { _waveStart = start; }
	bool IsGameOver() const { return _isGameOver; }
	void SetGameOver(bool over) { _isGameOver = over; }

	//점수 관련 변수 초기화
	void InitScore();


	void PlayDrumBeat();

	void EnemyDiscount() { _enemyCount--; };

	int _enemyCount = 0; // Enemy_Count
	int _mexEnemyCount = 3; // Enemy_MaxCount
	float _enemyRespawnTime = 30.0f; // Enemy_RespawnTime
private:
	GameState _gameState = GameState::InitState; // Gmanager_GameState
	GameLevel _gameLevel = GameLevel::MainMenu; // Gmanager_GameLevel
	World* currWorld = nullptr; // Gmanager_currWorld

	float _timeScale = 1.0f; // inGameTimeScale

	Player* _player1 = nullptr; // Gmanager_Player
	Player* _player2 = nullptr; // Gmanager_Player

	float _gravity = 9.8f;  // Gmanager_Gravity
	float _waveTime = 30.0f; // Gmanager_waveTime
	float _waveLeftTime = _waveTime; // Gmanager_waveLeftTime
	float _stageTime = _waveTime * 12; // Gmanager_stageTime ==> offset 필요할듯
	float _time = 0.0f; // Gmanager_Time
	int _waveCount = 0; // Gmanager_waveCount
	bool _waveClear = false; // waveClear
	bool _isGameOver = false; // Gmanager_isGameOver
	bool _checkStatus = false; // Gmanager_checkStatus
	bool _waveStart = false;
	int _correctNumber = 0; // Score_Number
	int _correctAgility = 0; // Score_Agility
	int _playerFrozen = 0; // Score_Frozen
	int _filedClean = 0; // Score_Clean
	float _farSleepTime = 0.0f; // Sleeping_time
	float _monsterLiveTime = 0.0f; // Enemy_Rest

	float _bongsuUpdateTime = 0.0f; // 봉수대 업데이트 시키는 변수

	//봉수대 정보
	//UI로 보여줄 봉수대 5개의 정보 -> 예를 어디서 가져올지  --> 파일로 가져올게 아니라면 내부에 박아넣을지
	bool Far_isSleep = false; // 
	std::array<int, 5> FarBongsu = { 0,0,0,0,0 };
	std::vector<std::vector<int>> FarBongsuList;
	//인게임 봉수대 5개의 정보  -> 예를 언제마다 가져올지? -> 매 초마다//
	std::vector<int> InGameBongsu = { 0,0,0,0,0 };

	//봉수대 비교 함수 --> 매 초마다 비교
	int farflag = 0; // 비교를 위한 farBongsu 비트플래그
	int ingameflag = 0; // 비교를 위한 ingameBongsu 비트플래그
	void CompareBongsu();


	

	std::vector<std::string> _stageList; // Gmanager_stageList
};

inline static auto& GameManagement = GameManager::GetInstance();