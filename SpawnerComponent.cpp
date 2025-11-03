#include "SpawnerComponent.h"

SpawnerComponent::~SpawnerComponent()
{
	if (!_object.empty())
	{
		while (!_object.empty())
		{
			auto obj = _object.front();
			_object.pop();

			obj->Destroy();

			delete obj;
		}
	}
}

void SpawnerComponent::Initialize()
{
	if (!_object.empty())
	{
		while (!_object.empty())
		{
			_object.pop();
		}
	}

	_spawnTime = _initSpawnTime;
	//스포너 생성 객체는 objectfactory를 통해 Monster,Wood,Torch 로 제한
	if (_spawnObjectName == MetaType<Monster>::type) //몬스터일 경우
	{
		for (size_t i = 0; i < _maxSpawnCount; i++)
		{
			std::string path = PathFinder::Relative("Prefab\\").string() + _prefabName + ".json";
			std::ifstream input_file(path);
			if (!input_file.is_open())
			{
				Log::Error("Failed to open file : " + path);
				return;
			}
			json input;
			input_file >> input;
			input_file.close();

			if (input.find("PrefebType") == input.end())
			{
				Log::Error("Invalid JSON format");
				return;
			}

			std::string prefabType = input["PrefebType"].get<std::string>();
			//auto obj = CreateObject<Monster>(name);
			auto monster = new Monster(_owner->GetName() + std::to_string(i));

			monster->DeSerialize(input);
			monster->DeserializeAddcomponent(input["components"]);

			_object.push(monster); //emplace(new Monster(std::to_string(i)));
			Event->Subscribe("MonsterDead" + _owner->GetName() + std::to_string(i), [&](void* data) {
				ReturnSpawnObject(static_cast<Object*>(data));
				GameManagement->GetCurrWorld()->DespawnObject(static_cast<Object*>(data));
				GameManagement->EnemyDiscount();
				});
			//_object.emplace_back(new Monster(std::to_string(i)));
		}
	}

	if (_spawnObjectName == MetaType<Wood>::type) //나무일 경우
	{
		for (size_t i = 0; i < _maxSpawnCount; i++)
		{
			std::string path = PathFinder::Relative("Prefab\\").string() + _prefabName + ".json";
			std::ifstream input_file(path);
			if (!input_file.is_open())
			{
				Log::Error("Failed to open file : " + path);
				return;
			}
			json input;
			input_file >> input;
			input_file.close();

			if (input.find("PrefebType") == input.end())
			{
				Log::Error("Invalid JSON format");
				return;
			}

			std::string prefabType = input["PrefebType"].get<std::string>();
			//auto obj = CreateObject<Monster>(name);
			auto wood = new Wood(_owner->GetName() + std::to_string(i));

			wood->DeSerialize(input);
			wood->DeserializeAddcomponent(input["components"]);

			_object.push(wood);
			//_object.emplace(new Wood(std::to_string(i)));
			Event->Subscribe("WoodReturn" + _owner->GetName() + std::to_string(i), [&](void* data) {
				GameManagement->GetCurrWorld()->DespawnObject(static_cast<Object*>(data));
				ReturnSpawnObject(static_cast<Object*>(data));
				});
			//_object.emplace_back(new Wood(std::to_string(i)));
		}
	}

	if (_spawnObjectName == MetaType<Torch>::type) //횃불일 경우
	{
		for (size_t i = 0; i < _maxSpawnCount; i++)
		{
			std::string path = PathFinder::Relative("Prefab\\").string() + _prefabName + ".json";
			std::ifstream input_file(path);
			if (!input_file.is_open())
			{
				Log::Error("Failed to open file : " + path);
				return;
			}
			json input;
			input_file >> input;
			input_file.close();

			if (input.find("PrefebType") == input.end())
			{
				Log::Error("Invalid JSON format");
				return;
			}

			std::string prefabType = input["PrefebType"].get<std::string>();
			//auto obj = CreateObject<Monster>(name);
			auto torch = new Torch(_owner->GetName() + std::to_string(i));

			torch->DeSerialize(input);
			torch->DeserializeAddcomponent(input["components"]);

			_object.push(torch);
			//_object.emplace(new Torch(std::to_string(i)));
			Event->Subscribe("TorchReturn" + _owner->GetName() + std::to_string(i), [&](void* data) {
				ReturnSpawnObject(static_cast<Object*>(data));
				GameManagement->GetCurrWorld()->DespawnObject(static_cast<Object*>(data));
				});
			//_object.emplace_back(new Torch(std::to_string(i)));
		}
	}

	if (_spawnObjectName == MetaType<Loot>::type) //횃불일 경우
	{
		for (size_t i = 0; i < _maxSpawnCount; i++)
		{
			std::string path = PathFinder::Relative("Prefab\\").string() + _prefabName + ".json";
			std::ifstream input_file(path);
			if (!input_file.is_open())
			{
				Log::Error("Failed to open file : " + path);
				return;
			}
			json input;
			input_file >> input;
			input_file.close();

			if (input.find("PrefebType") == input.end())
			{
				Log::Error("Invalid JSON format");
				return;
			}

			std::string prefabType = input["PrefebType"].get<std::string>();
			//auto obj = CreateObject<Monster>(name);
			auto Loot = new Torch(_owner->GetName() + std::to_string(i));

			Loot->DeSerialize(input);
			Loot->DeserializeAddcomponent(input["components"]);

			_object.push(Loot);
			//_object.emplace(new Loot(std::to_string(i)));
			Event->Subscribe("LootReturn" + _owner->GetName() + std::to_string(i), [&](void* data) {
				ReturnSpawnObject(static_cast<Object*>(data));
				GameManagement->GetCurrWorld()->DespawnObject(static_cast<Object*>(data));
				});
			//_object.emplace_back(new Loot(std::to_string(i)));
		}
	}

};

void SpawnerComponent::FixedUpdate(float fixedTick)
{
}

void SpawnerComponent::Update(float tick)
{

	if (_flagObject) //플레그 오브젝트가 있을 때
	{
		_isSpawning = !_flagObject->IsFire(); //스폰 플레그 가져오기. 불이 꺼졌을 때 스폰.
	}

	if (_isSpawning) //스폰 플레그 온
	{
		if (_spawnCount < _maxSpawnCount) //스폰된 몬스터 수가 최대 몬스터 수보다 작을 때
		{
			_spawnTimer -= tick; //스폰 타이머 감소
		}

		if (_spawnTimer <= 0) //스폰 타이머가 0이하일 때
		{
			SpawnObject(); //Object 스폰
			_spawnCount++; //스폰된 몬스터 수 증가
			_spawnTimer = _spawnTime; //스폰 타이머를 스폰 시간으로 설정
		}

	}
}

void SpawnerComponent::LateUpdate(float tick)
{
}

void SpawnerComponent::EditorContext()
{
	ImGui::Text("SpawnerComponent");

	if (ImGui::Button("Object"))
	{
		ImGui::OpenPopup("Object");
	}
	ImGui::SameLine();
	ImGui::Text(":");
	ImGui::SameLine();
	ImGui::Text(_spawnObjectName.c_str());

	if (ImGui::BeginPopup("Object"))
	{
		if (ImGui::MenuItem("Monster"))
		{
			_spawnObjectName = MetaType<Monster>::type;
			Initialize();
		}
		if (ImGui::MenuItem("Wood"))
		{
			_spawnObjectName = MetaType<Wood>::type;
			Initialize();
		}
		if (ImGui::MenuItem("Torch"))
		{
			_spawnObjectName = MetaType<Torch>::type;
			Initialize();
		}
		if (ImGui::MenuItem("Loot"))
		{
			_spawnObjectName = MetaType<Loot>::type;
			Initialize();
		}
		ImGui::EndPopup();
	}
	if (ImGui::InputText("prefabName", nameBuffer.data(), nameBuffer.size())) {
		_prefabName = std::string(nameBuffer.data());
	}

	ImGui::Text("");

	ImGui::Text("Spawner State : ");

	ImGui::Text(_isSpawning ? "Spawn" : "False");

	ImGui::Text("");


	if (ImGui::Button("Spawner Test")) {
		_isSpawning = !_isSpawning;
	}

	ImGui::Text("");

	ImGui::Text("Max Spawn Count : ");
	ImGui::Text(std::to_string(_maxSpawnCount).c_str());
	ImGui::DragInt("count", &_maxSpawnCount);

	ImGui::Text("");

	ImGui::Text("Spawn Offset");

	ImGui::DragFloat3("Offset", &_spawnOffset.x);

}

void SpawnerComponent::Serialize(_inout json& out)
{
	json data;
	data["Type"] = MetaType<SpawnerComponent>::type;
	data["SpawnerComponent"]["SpawnObjectName"] = _spawnObjectName;
	data["SpawnerComponent"]["MaxSpawnCount"] = _maxSpawnCount;
	data["SpawnerComponent"]["InitSpawnTime"] = _initSpawnTime;
	data["SpawnerComponent"]["PrefabName"] = _prefabName;
	data["SpawnerComponent"]["SpawnOffset"] = { _spawnOffset.x, _spawnOffset.y, _spawnOffset.z };
	//data["FlagObject"] = _flagObject->GetName();
	out["components"].push_back(data);
}

void SpawnerComponent::DeSerialize(_in json& in)
{
	_spawnObjectName =	in["SpawnerComponent"]["SpawnObjectName"];
	_maxSpawnCount =	in["SpawnerComponent"]["MaxSpawnCount"];
	_initSpawnTime =	in["SpawnerComponent"]["InitSpawnTime"];
	if (!in["SpawnerComponent"]["SpawnOffset"].empty())
	{
		_spawnOffset = Mathf::jsonToVector3(in["SpawnerComponent"]["SpawnOffset"]);
	}
	if(!in["SpawnerComponent"]["PrefabName"].empty())
		_prefabName =		in["SpawnerComponent"]["PrefabName"].get<std::string>();
	//_flagObject = static_cast<FlagObject*>(GameManagement->GetCurrWorld()->GetNameObject(in["FlagObject"]));
	//Initialize();
}

Object* SpawnerComponent::SpawnObject()
{
	if (!_object.empty())
	{
		auto obj = _object.front();
		GameManagement->GetCurrWorld()->SpawnObject(obj);
		_object.pop();
		//obj->SetPosition(_owner->GetWorldMatrix().Translation());
		//obj->translate(_owner->GetWorldMatrix().Translation());
		obj->SetPosition(_owner->GetWorldMatrix().Translation() + _spawnOffset);
		obj->Initialize();

		auto renders = obj->GetComponents<RenderComponent>();
		for (auto& r : renders) {
			r->SetVisiable(true);
			r->SetSubVisible(true);
			r->SetAxeVisible(true);
		}

		auto colliders = obj->GetComponents<ColliderComponent>();
		for (auto& c : colliders) {
			c->SetEnable(true);
		}
		return obj;
	}
	return nullptr;
}

void SpawnerComponent::ReturnSpawnObject(Object* object)
{
	_object.push(object);
}
