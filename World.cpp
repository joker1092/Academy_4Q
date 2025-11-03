#include "World.h"
#include "RenderEngine/ImGuiRegister.h"
#include "Utility_Framework/Core.Minimal.h"
#include "RenderComponent.h"
#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "StairColliderComponent.h"
#include "RigidbodyComponent.h"
#include "InteractFirewood.h"
#include "RenderEngine/UI_DataSystem.h"
#include "RenderEngine/FontManager.h"
#include "InputManager.h"
#include "TestComponent.h"
#include "CharacterController.h"
#include "FireComponent.h"
#include "TorchComponet.h"
#include "SpawnerComponent.h"
#include "ObjectFactory.h"
#include "ComponentFactory.h"
#include "SoundManager.h"
#include "TestInteractableComponent.h"
#include "MeshEditor.h"
#include "../GridEditor.h"
#include "GameManager.h"
#include "EventSystem.h"
#include "D2DLib/UIController.h"
#include "D2DLib/UIEventManager.h"

static char nameBuffer[256] = ""; // 입력된 이름을 저장할 임시 버퍼
static char scenePath[256] = "SampleScene.json";
static char prefabBuffer[256] = "Prefab";
static char textUIBuffer[256] = "Input Text";
static bool openPopup = false;    // 팝업 열기 플래그
static bool openfilePopup = false;    // 파일 열기 플래그
static bool savefilePopup = false;    // 파일 저장 플래그
static bool openPrefabPopup = false;    // 프리팹 열기 플래그
static bool createObject = false; // 오브젝트 생성 플래그
static std::string userName = ""; // 최종 입력된 이름 저장
static std::string objType = "Object";  //생성할 오브젝트의 타입의 number Flag
static int openStep = 0;          // 팝업 단계

std::wstring ConvertToWString(const char* charArray)
{
	if (!charArray) return L"";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, charArray, -1, nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, charArray, -1, &wstr[0], size_needed);

	// 널 문자가 포함되므로 제거
	if (!wstr.empty() && wstr.back() == L'\0')
		wstr.pop_back();

	return wstr;
}

void World::Initialize()
{
	//todo: Initialize
	_objects.reserve(100); //기본값 10개 변경가능
	_objectIds.reserve(100);

	PreObjectLoad();
	PreComponentLoad();

	ImGui::ContextRegister("Scene Settings", [&]()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				{
					// New action
					AllDestroy();
					Destroy();
				}
				if (ImGui::MenuItem("Open World", "Ctrl+O"))
				{
					if (0 == openStep)
					{
						openfilePopup = true;
					}
				}
				if (ImGui::MenuItem("Save World", "Ctrl+S"))
				{
					// Save action
					savefilePopup = true;

				}
				if (ImGui::MenuItem("Exit"))
				{
					// Exit action
					PostQuitMessage(0);
				}
				ImGui::EndMenu();

			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Behavior Tree Editor"))
				{
					_isCreateBt = !_isCreateBt;
				}
				if (ImGui::MenuItem("Create Canvas"))
				{
					_isCreateCanvas = !_isCreateCanvas;
				}
				if (ImGui::MenuItem("Mesh Editor"))
				{
					MeshEditorSystem->show = !MeshEditorSystem->show;
				}
				if (ImGui::MenuItem("Grid Editor"))
				{
					GridEditorSystem->show = !GridEditorSystem->show;
				}
				if(ImGui::MenuItem("Edit Plane"))
				{
					static bool isEdit = true;
					isEdit = !isEdit;
					_scene->SetEditorMode(isEdit);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if(openfilePopup)
		{
			ImGui::OpenPopup("Open File");
		}

		if (savefilePopup)
		{
			ImGui::OpenPopup("Save File");
		}

		if (ImGui::BeginPopupModal("Open File", &openfilePopup))
		{
			ImGui::InputText("File Path", scenePath, sizeof(scenePath));
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				openStep++;
				openfilePopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				openStep = 0;
				openfilePopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("Save File"))
		{
			ImGui::InputText("File Path", scenePath, sizeof(scenePath));
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				savefilePopup = false;
				OutputJson(PathFinder::Relative("Scene\\").string() + scenePath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				savefilePopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (1 == openStep)
		{
			AllDestroy();
			openStep++;
		}
		else if (2 == openStep)
		{
			LoadJson(PathFinder::Relative("Scene\\").string() + scenePath);
			openStep = 0;
		}
	});

	ImGui::ContextRegister("GameObjects", [&]()
	{
		if (ImGui::Button("new", ImVec2(80, 30)))
		{
			openPopup = true;
			createObject = false;
			ImGui::OpenPopup("New Object");
		}

		ImGui::SameLine();
		if (ImGui::Button("Load Prefab", ImVec2(80, 30)))
		{
			ImGui::OpenPopup("Prefab");
		}

		if (ImGui::BeginPopup("Prefab"))
		{
			ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));
			ImGui::InputText("Prefab Name", prefabBuffer, sizeof(prefabBuffer));
			ImGui::Text("");

			std::vector<std::string_view> prefabList{
				GetTypeName<Object>(),
				GetTypeName<EnemyPotal>(),
				GetTypeName<FloorBox>(),
				GetTypeName<Stairs>(),
				GetTypeName<Wall>(),

				GetTypeName<Bongsu>(),
				GetTypeName<Drum>(),
				GetTypeName<FlagObject>(),
				GetTypeName<Loot>(),
				GetTypeName<Pail>(),
				GetTypeName<Pond>(),
				GetTypeName<Torch>(),
				GetTypeName<TorchBox>(),
				GetTypeName<WeekFloor>(),
				GetTypeName<Wood>(),
				GetTypeName<WoodPile>(),

				GetTypeName<Player>(),
				GetTypeName<Monster>()
			};

			if (ImGui::BeginCombo("Prefab Type", objType.c_str()))
			{
				for (auto& prefab : prefabList)
				{
					bool isSelected = (objType == prefab);
					if (ImGui::Selectable(prefab.data(), isSelected))
					{
						objType = prefab;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				LoadPrefab(objType, prefabBuffer, nameBuffer);
				memset(nameBuffer, 0, sizeof(nameBuffer));
				memset(prefabBuffer, 0, sizeof(prefabBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				memset(nameBuffer, 0, sizeof(nameBuffer));
				memset(prefabBuffer, 0, sizeof(prefabBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("New Object", &openPopup))
		{
			ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer));
			ImGui::Text("");
			std::vector<std::string_view> objectList{
				GetTypeName<Object>(),
				GetTypeName<EnemyPotal>(),
				GetTypeName<FloorBox>(),
				GetTypeName<Stairs>(),
				GetTypeName<Wall>(),

				GetTypeName<Bongsu>(),
				GetTypeName<Drum>(),
				GetTypeName<FlagObject>(),
				GetTypeName<Loot>(),
				GetTypeName<Pail>(),
				GetTypeName<Pond>(),
				GetTypeName<Torch>(),
				GetTypeName<TorchBox>(),
				GetTypeName<WeekFloor>(),
				GetTypeName<Wood>(),
				GetTypeName<WoodPile>(),

				GetTypeName<Player>(),
				GetTypeName<Monster>()
			};

			if (ImGui::BeginCombo("Object Type", objType.c_str()))
			{
				for (auto& object : objectList)
				{
					bool isSelected = (objType == object);
					if (ImGui::Selectable(object.data(), isSelected))
					{
						objType = object;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Text("");
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				userName = nameBuffer;
				openPopup = false;
				memset(nameBuffer, 0, sizeof(nameBuffer));
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				openPopup = false;
				memset(nameBuffer, 0, sizeof(nameBuffer));
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (!createObject && !userName.empty())
		{
			if (objType == MetaType<Object>::type.data()) { //기본 오브젝트 생성
				auto obj = CreateObject(userName);
			}
			if (objType == MetaType<EnemyPotal>::type.data()) { //적 포탈 오브젝트 생성
				auto obj = CreateObject<EnemyPotal>(userName);
			}
			if (objType == MetaType<FloorBox>::type.data()) //바닥블록 오브젝트 생성
			{
				auto obj = CreateObject<FloorBox>(userName);
			}
			if (objType == MetaType<Stairs>::type.data())	//계단 오브젝트 생성
			{
				auto obj = CreateObject<Stairs>(userName);
			}
			if (objType == MetaType<Wall>::type.data())		//벽 오브젝트 생성
			{
				auto obj = CreateObject<Wall>(userName);
			}

			if (objType == MetaType<Bongsu>::type.data()) //봉수대 오브젝트 생성
			{
				auto obj = CreateObject<Bongsu>(userName);
			}
			if (objType == MetaType<Drum>::type.data()) //북 오브젝트 생성
			{
				auto obj = CreateObject<Drum>(userName);
			}
			if (objType == MetaType<FlagObject>::type.data()) //벽걸이 횃대 오브젝트 생성
			{
				auto obj = CreateObject<FlagObject>(userName);
			}
			if (objType == MetaType<Loot>::type.data()) //털뭉치 오브젝트 생성
			{
				auto obj = CreateObject<Loot>(userName);
			}
			if (objType == MetaType<Pail>::type.data()) //양동이 오브젝트 생성
			{
				auto obj = CreateObject<Pail>(userName);
			}
			if (objType == MetaType<Pond>::type.data()) //연못 오브젝트 생성
			{
				auto obj = CreateObject<Pond>(userName);
			}
			if (objType == MetaType<Torch>::type.data()) //횃불 오브젝트 생성
			{
				auto obj = CreateObject<Torch>(userName);
			}
			if (objType == MetaType<TorchBox>::type.data()) //횃대 상자 오브젝트 생성
			{
				auto obj = CreateObject<TorchBox>(userName);
			}
			if (objType == MetaType<WeekFloor>::type.data()) //약한 바닥 오브젝트 생성
			{
				auto obj = CreateObject<WeekFloor>(userName);
			}
			if (objType == MetaType<Wood>::type.data()) //나무 오브젝트 생성
			{
				auto obj = CreateObject<Wood>(userName);
			}
			if (objType == MetaType<WoodPile>::type.data()) //장작 더미 오브젝트 생성
			{
				auto obj = CreateObject<WoodPile>(userName);
			}

			if (objType == MetaType<Player>::type.data())
			{
				auto obj = CreateObject<Player>(userName);
				obj->Initialize();
			}
			if (objType == MetaType<Monster>::type.data())
			{
				auto obj = CreateObject<Monster>(userName);
			}

			userName.clear();
			createObject = true;
		}

		ImGui::Separator();

		for (auto& obj : _objects)
		{
			if (obj->IsDestroyedMark())
			{
				continue;
			}

			if (ImGui::TreeNodeEx(obj->GetName().c_str(),
				ImGuiTreeNodeFlags_CollapsingHeader 
				| ImGuiTreeNodeFlags_NoTreePushOnOpen 
				| ImGuiTreeNodeFlags_Selected))
			{
				selectedGameObject = obj->GetName();

				if (ImGui::BeginPopupContextItem("R_ClickMenu"))
				{
					if (ImGui::MenuItem("Create Prefab"))
					{
						openPrefabPopup = true;
					}

					if (ImGui::MenuItem("Delete"))
					{

						DestroyObject(obj.get());

					}
					ImGui::EndPopup();
				}
			}
		}

		ImGui::Separator();
		for (auto& obj : _temporaryObjects)
		{
			if (obj->IsDestroyedMark())
			{
				continue;
			}
			if (ImGui::TreeNodeEx(obj->GetName().c_str(),
				ImGuiTreeNodeFlags_CollapsingHeader
				| ImGuiTreeNodeFlags_NoTreePushOnOpen
				| ImGuiTreeNodeFlags_Selected))
			{
				selectedGameObject = obj->GetName();
				if (ImGui::BeginPopupContextItem("R_ClickMenu"))
				{
					if (ImGui::MenuItem("Create Prefab"))
					{
						openPrefabPopup = true;
					}
					if (ImGui::MenuItem("Delete"))
					{
						DestroyObject(obj);
					}
					ImGui::EndPopup();
				}
			}
		}

		if (openPrefabPopup)
		{
			ImGui::OpenPopup("Create Prefab");
		}

		if (ImGui::BeginPopupModal("Create Prefab", &openPrefabPopup, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Prefab Name", nameBuffer, sizeof(nameBuffer));
			ImGui::Text("");

			std::vector<std::string_view> prefabList{
				GetTypeName<Object>(),
				GetTypeName<EnemyPotal>(),
				GetTypeName<FloorBox>(),
				GetTypeName<Stairs>(),
				GetTypeName<Wall>(),

				GetTypeName<Bongsu>(),
				GetTypeName<Drum>(),
				GetTypeName<FlagObject>(),
				GetTypeName<Loot>(),
				GetTypeName<Pail>(),
				GetTypeName<Pond>(),
				GetTypeName<Torch>(),
				GetTypeName<TorchBox>(),
				GetTypeName<WeekFloor>(),
				GetTypeName<Wood>(),
				GetTypeName<WoodPile>(),

				GetTypeName<Player>(),
				GetTypeName<Monster>()
			};

			if (ImGui::BeginCombo("Prefab Type", objType.c_str()))
			{
				for (auto& prefab : prefabList)
				{
					bool isSelected = (objType == prefab);
					if (ImGui::Selectable(prefab.data(), isSelected))
					{
						objType = prefab;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				CreatePrefab(objType, nameBuffer, GetNameObject(selectedGameObject));
				memset(nameBuffer, 0, sizeof(nameBuffer));
				openPrefabPopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				openPrefabPopup = false;
				memset(nameBuffer, 0, sizeof(nameBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}


	}, ImGuiWindowFlags_None);

	ImGui::ContextRegister("GameObject Inspector", [&]()
	{
		if (selectedGameObject.empty())
		{
			return;
		}
		auto obj = GetNameObject(selectedGameObject);

		if (obj)
		{
			ImGui::Text("Object Name : %s", obj->GetName().c_str());
			ImGui::DragFloat3("Position", &obj->_position.x, 0.01f);
			ImGui::DragFloat3("Rotation", &obj->_rotation.x, 0.01f);
			ImGui::DragFloat3("Scale", &obj->_scale.x, 0.01f);

			obj->EditorContext();

			ImGui::Separator();

			for (auto& component : obj->_components)
			{
				component->EditorContext();
				ImGui::Separator();
			}

			ImGui::Text("");
			ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f - 60, 2.f);
			if (ImGui::Button("AddComponent"))
			{
				ImGui::OpenPopup("AddComponent");
			}

			if (ImGui::BeginPopup("AddComponent", ImGuiWindowFlags_AlwaysVerticalScrollbar))
			{
				if (ImGui::MenuItem("RenderComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<RenderComponent>(_scene);
				}

				if (ImGui::MenuItem("BoxColliderComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<BoxColliderComponent>();
				}

				if (ImGui::MenuItem("SphereColliderComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<SphereColliderComponent>();
				}
				if (ImGui::MenuItem("StairColliderComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<StairColliderComponent>();
				}

				if (ImGui::MenuItem("RigidbodyColliderComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<RigidbodyComponent>();
				}

				if (ImGui::MenuItem("InteractFirewood"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<InteractFirewood>();
				}

				if (ImGui::MenuItem("TestComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<TestComponent>();
				}
				if (ImGui::MenuItem("CharacterController"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}

					obj->AddComponent<CharacterController>();
				}
				if (ImGui::MenuItem("TestInteractableComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}
					obj->AddComponent<TestInteractableComponent>();
				}

				if (ImGui::MenuItem("FireComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}
					obj->AddComponent<FireComponent>(_scene);
				}

				if (ImGui::MenuItem("TorchComponet"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}
					obj->AddComponent<TorchComponet>(_scene);
				}


				if (ImGui::MenuItem("SpawnerComponent"))
				{
					if (!_scene)
					{
						Log::Error("Scene is not set");
						return;
					}
					obj->AddComponent<SpawnerComponent>();
				}


				ImGui::EndPopup();
			}
		}

	}, ImGuiWindowFlags_None);

	ImGui::ContextRegister("UI Objects", [&]() 
	{
		if (ImGui::Button("Create Canvas", ImVec2(80, 30)))
		{
			CreateCanvas();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Canvas", ImVec2(80, 30)))
		{
			ImGui::OpenPopup("Load Canvas");
		}

		if (ImGui::BeginPopup("Load Canvas"))
		{
			ImGui::InputText("File Path", scenePath, sizeof(scenePath));
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				LoadCanvas(PathFinder::Relative("Scene\\").string() + scenePath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("UI", ImVec2(80, 30)))
		{
			ImGui::OpenPopup("Create UI");
		}

		if (ImGui::BeginPopup("Create UI"))
		{
			ImGui::InputText("name", nameBuffer, sizeof(nameBuffer));
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				CreateUIObject(nameBuffer);
				memset(nameBuffer, 0, sizeof(nameBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				memset(nameBuffer, 0, sizeof(nameBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Text", ImVec2(80, 30)))
		{
			ImGui::OpenPopup("Create Text UI");
		}

		if (ImGui::BeginPopup("Create Text UI"))
		{
			ImGui::InputText("name", nameBuffer, sizeof(nameBuffer));
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				CreateTextUIObject(nameBuffer);
				memset(nameBuffer, 0, sizeof(nameBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				memset(nameBuffer, 0, sizeof(nameBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (_currentCanvas)
		{
			if(!_currentCanvas->getObjList().empty())
			{
				for (auto& obj : _currentCanvas->getObjList())
				{
					if (obj->m_DestoryMark)
					{
						continue;
					}

					ImGui::Text("UI object");

					if (ImGui::TreeNodeEx(obj->m_Name.c_str(), ImGuiTreeNodeFlags_Selected))
					{
						selectedUIObject = obj->m_Name;

						if (ImGui::BeginPopupContextItem("R_ClickMenu"))
						{
							if (ImGui::MenuItem("Delete"))
							{
								DestroyUIObject(obj->m_Name);
							}
							ImGui::EndPopup();
						}
						ImGui::TreePop();
					}
				}
			}

			if (!_currentCanvas->getTextList().empty())
			{
				for (auto& text : _currentCanvas->getTextList())
				{
					if (text->m_DestoryMark)
					{
						continue;
					}

					ImGui::Separator();
					ImGui::Text("Text UI object");

					if (ImGui::TreeNodeEx(text->m_Name.c_str(), ImGuiTreeNodeFlags_Selected))
					{
						selectedTextUIObject = text->m_Name;

						if (ImGui::BeginPopupContextItem("R_ClickMenu"))
						{
							if (ImGui::MenuItem("Delete"))
							{
								DestroyTextUIObject(text->m_Name);
							}
							ImGui::EndPopup();
						}
						ImGui::TreePop();
					}
				}
			}
		}
	});

	ImGui::ContextRegister("UI Object Inspector", [&]()
	{
		if (selectedUIObject.empty() && selectedTextUIObject.empty())
		{
			return;
		}
		auto& obj = _currentCanvas->getObjList();
		auto iter = std::ranges::find_if(obj, [&](UIObject* obj) 
		{ 
			return obj->m_Name == selectedUIObject; 
		});
		auto& text = _currentCanvas->getTextList();
		auto textIter = std::ranges::find_if(text, [&](TextUIObject* obj)
		{
			return obj->m_Name == selectedTextUIObject;
		});

		if (iter != obj.end())
		{
			ImGui::Text("Object Name : %s", (*iter)->m_Name.c_str());
			ImGui::DragFloat2("Position", &(*iter)->Pos.x, 0.01f);
			ImGui::SameLine();
			ImGui::Checkbox("Lock", &(*iter)->m_IsPosLock);
			ImGui::DragFloat("Rotation", &(*iter)->Rotation, 0.01f);
			ImGui::DragFloat2("Scale", &(*iter)->Scale.x, 0.01f);
			ImGui::DragFloat("Opacity", &(*iter)->m_OriginOpacity, 0.01f, 0.f, 1.f);
			(*iter)->m_Opacity = (*iter)->m_OriginOpacity;
			ImGui::DragInt("ZOrder", &(*iter)->m_ZOrder, 10);
			ImGui::Checkbox("IsVisible", &(*iter)->m_Visible);
			//if ((*iter)->m_IsScoreObject)
			//{
			//	ImGui::InputInt("Score", &(*iter)->m_Score, 1);
			//}
			if (ImGui::Button("Set Bitmap"))
			{
				ImGui::OpenPopup("Set Bitmap");
			}

			ImGui::SameLine();
			if (ImGui::Button("Set Select Bitmap"))
			{
				ImGui::OpenPopup("Set Select Bitmap");
			}
			ImGui::DragInt("select UI ID Code", &(*iter)->m_SelectID, 1);

			if (ImGui::BeginPopup("Set Bitmap"))
			{
				ImGui::Text("Please Click the desired bitmap into the content window.");
				{
					auto UIObj = UISystem->GetDragDropBitmap();
					auto AniUIObj = UISystem->GetDragDropBitmaps();
					auto UIKey = UISystem->GetDragDropKey();

					if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
					{
						//iter에 모든 UIObject의 정보를 저장
						(*iter)->m_Key = UIKey;
						(*iter)->m_pBitmap = UIObj;
						D2D_SIZE_F size;
						size = (*iter)->m_pBitmap->GetSize();
						if (size.width < 1900.f)
						{
							(*iter)->m_Size = { size.width, size.height };
							(*iter)->m_Center = { size.width / 2, size.height / 2 };
						}
						(*iter)->m_IsAnimation = false;
						UISystem->ClearDragDropBitmap();
					}

					if (AniUIObj.size() > 0 && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
					{
						//이친구도 iter에 모든 UIObject의 정보를 저장
						(*iter)->m_Key = UIKey;
						(*iter)->m_pBitmaps = AniUIObj;
						D2D_SIZE_F size;
						size = (*iter)->m_pBitmaps[0]->GetSize();
						if (size.width < 1900.f)
						{
							(*iter)->m_Size = { size.width, size.height };
							(*iter)->m_Center = { size.width / 2, size.height / 2 };
						}
						(*iter)->m_IsAnimation = true;

						UISystem->ClearDragDropBitmaps();
					}
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginPopup("Set Select Bitmap"))
			{
				ImGui::Text("Please Click the desired bitmap into the content window.");
				{
					auto UIObj = UISystem->GetDragDropBitmap();
					auto UIKey = UISystem->GetDragDropKey();
					if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
					{
						//iter에 모든 UIObject의 정보를 저장
						(*iter)->m_selectKey = UIKey;
						(*iter)->m_pSelectBitmap = UIObj;
						UISystem->ClearDragDropBitmap();
					}
				}
				ImGui::EndPopup();
			}

			ImGui::Checkbox("Is Width Clipping", &(*iter)->m_IsVClipping);
			ImGui::SameLine();
			ImGui::Checkbox("Is Height Clipping", &(*iter)->m_IsHClipping);
			//ImGui::DragFloat("TestPercent", &(*iter)->m_ViewPercentage, 0.01f, 0.f, 1.f);

			ImGui::Separator();
			if (ImGui::BeginCombo("Global Event", (*iter)->m_EventKey.c_str()))
			{
				for (auto& [key, value] : Event->eventListners)
				{
					bool isSelected = ((*iter)->m_EventKey == key);
					if (ImGui::Selectable(key.c_str(), isSelected))
					{
						(*iter)->m_EventKey = key;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::DragFloat("Event Delay", &(*iter)->m_EventDelayTime, 0.01f);

			std::vector<std::string_view> typeString{
				"int",
				"float"
			};
			std::string type = "int";

			if (ImGui::BeginCombo("Event Value Type", type.c_str()))
			{
				for (auto& _type : typeString)
				{
					bool isSelected = (_type == type);
					if (ImGui::Selectable(type.data(), isSelected))
					{
						_type = type;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			static int value = 0;
			static float fValue = 0.f;
			static int playerID = 0;
			static int bongsuID = 0;

			if (type == "int")
			{
				ImGui::InputInt("Event Value", &value);
				if (ImGui::Button("Set Value"))
				{
					(*iter)->intValue = value;
				}
			}
			else if (type == "float")
			{
				ImGui::InputFloat("Event Value", &fValue);
				if (ImGui::Button("Set Value"))
				{
					(*iter)->floatValue = fValue;
				}
			}

			ImGui::Separator();
			ImGui::InputInt("UI Layer", &(*iter)->m_LayerID);
			if (ImGui::BeginCombo("UI Event", (*iter)->m_UIEventKey.c_str()))
			{
				for (auto& [key, value] : UIEvent->eventListners)
				{
					bool isSelected = ((*iter)->m_UIEventKey == key);
					if (ImGui::Selectable(key.c_str(), isSelected))
					{
						(*iter)->m_UIEventKey = key;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if ((*iter)->m_UIEventKey == "Clipping")
			{
				ImGui::InputInt("Clipping ID", &(*iter)->m_ClippingID);
				Mathf::Clamp((*iter)->m_ClippingID, 0, 2);
			}
			else if ((*iter)->m_UIEventKey == "Select")
			{
				std::string selectID = "Select ID : " + std::to_string((*iter)->m_SelectID);
				ImGui::Text(selectID.c_str());
			}
			else if ((*iter)->m_UIEventKey == "Click")
			{
				static std::string button = "Button : ";
				std::vector<std::string> buttonList{
					"A",
					"B",
					"X",
					"Y",
					"D_UP",
					"D_DOWN",
					"D_LEFT",
					"D_RIGHT",
					"START",
					"BACK",
					"LS",
					"RS"
				};

				if (ImGui::BeginCombo("Button", button.c_str()))
				{
					for (int i = 0; i < (int)ControllerButton::MAX; i++)
					{
						bool isSelected = ((*iter)->m_Button == (ControllerButton)i);
						if (ImGui::Selectable(buttonList[i].c_str(), isSelected))
						{
							(*iter)->m_Button = (ControllerButton)i;
							button = "Button : " + buttonList[i];
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				ImGui::Text((*iter)->m_EventKey.c_str());
				ImGui::SameLine();
				ImGui::Checkbox("Is Select Chain", &(*iter)->m_IsSelectExcute);
			}
			else if ((*iter)->m_UIEventKey == "StateChange")
			{
				ImGui::InputInt("Player ID", &playerID);
				std::vector<std::string_view> stateString{
				"Idle",
				"Cold1",
				"Cold2",
				"Frozen",
				"Stun",
				};
				ImGui::Checkbox("State UI Object", &(*iter)->m_IsStateChain);
				if (ImGui::BeginCombo("State Bitmap Setting", stateString[(*iter)->m_Index].data()))
				{
					for (int i = 0; i < stateString.size(); i++)
					{
						bool isSelected = ((*iter)->m_Index == i);
						if (ImGui::Selectable(stateString[i].data(), isSelected))
						{
							(*iter)->m_Index = i;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Set State Bitmap"))
				{
					ImGui::OpenPopup("Set State Bitmap");
				}

				if (ImGui::BeginPopup("Set State Bitmap"))
				{
					ImGui::Text("Please Click the desired bitmap into the content window.");
					{
						auto UIObj = UISystem->GetDragDropBitmap();
						auto UIKey = UISystem->GetDragDropKey();
						if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
						{
							//iter에 모든 UIObject의 정보를 저장
							(*iter)->m_BitmapKeys[(*iter)->m_Index] = UIKey;
							(*iter)->m_pBitmaps[(*iter)->m_Index] = UIObj;
							UISystem->ClearDragDropBitmap();
						}
					}
					ImGui::EndPopup();
				}
			}
			else if ((*iter)->m_UIEventKey == "FarBongsu")
			{
				ImGui::InputInt("Bongsu ID", &bongsuID);
			}
			else if ((*iter)->m_UIEventKey == "Interact")
			{
				ImGui::InputInt("Player ID", &playerID);
				std::vector<std::string_view> stateString{
				"Extinguish",
				"Torch",
				"Burn",
				"Drum",
				"Pick",
				"Slash",
				"Water",
				"Wood",
				};

				if (ImGui::BeginCombo("Interact Bitmap Setting", stateString[(*iter)->m_Index].data()))
				{
					for (int i = 0; i < stateString.size(); i++)
					{
						bool isSelected = ((*iter)->m_Index == i);
						if (ImGui::Selectable(stateString[i].data(), isSelected))
						{
							(*iter)->m_Index = i;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Set Interact Bitmap"))
				{
					ImGui::OpenPopup("Set Interact Bitmap");
				}

				if (ImGui::BeginPopup("Set Interact Bitmap"))
				{
					ImGui::Text("Please Click the desired bitmap into the content window.");
					{
						auto UIObj = UISystem->GetDragDropBitmap();
						auto UIKey = UISystem->GetDragDropKey();
						if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
						{
							//iter에 모든 UIObject의 정보를 저장
							(*iter)->m_BitmapKeys[(*iter)->m_Index] = UIKey;
							(*iter)->m_pBitmaps[(*iter)->m_Index] = UIObj;
							UISystem->ClearDragDropBitmap();
						}
					}
					ImGui::EndPopup();
				}
			}
			else if ((*iter)->m_UIEventKey == "Score")
			{
				ImGui::InputInt("ScoreID", &(*iter)->m_ScoreID, 1);
			}
			else if ((*iter)->m_UIEventKey == "Stamp")
			{
				ImGui::InputInt("Player ID", &playerID);
				std::vector<std::string_view> stateString{
					"Daetong",
					"Ketong",
					"Yaktong",
					"Jotong",
					"Bultong"
				};

				if (ImGui::BeginCombo("Stamp Bitmap Setting", stateString[(*iter)->m_Index].data()))
				{
					for (int i = 0; i < stateString.size(); i++)
					{
						bool isSelected = ((*iter)->m_Index == i);
						if (ImGui::Selectable(stateString[i].data(), isSelected))
						{
							(*iter)->m_Index = i;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Set Interact Bitmap"))
				{
					ImGui::OpenPopup("Set Interact Bitmap");
				}

				if (ImGui::BeginPopup("Set Interact Bitmap"))
				{
					ImGui::Text("Please Click the desired bitmap into the content window.");
					{
						auto UIObj = UISystem->GetDragDropBitmap();
						auto UIKey = UISystem->GetDragDropKey();
						if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
						{
							//iter에 모든 UIObject의 정보를 저장
							(*iter)->m_BitmapKeys[(*iter)->m_Index] = UIKey;
							(*iter)->m_pBitmaps[(*iter)->m_Index] = UIObj;
							UISystem->ClearDragDropBitmap();
						}
					}
					ImGui::EndPopup();
				}
			}
			else if ((*iter)->m_UIEventKey == "Manduk")
			{
				ImGui::InputInt("Player ID", &playerID);
				std::vector<std::string_view> stateString{
					"Daetong",
					"Ketong",
					"Yaktong",
					"Jotong",
					"Bultong"
				};

				if (ImGui::BeginCombo("Manduk Bitmap Setting", stateString[(*iter)->m_Index].data()))
				{
					for (int i = 0; i < stateString.size(); i++)
					{
						bool isSelected = ((*iter)->m_Index == i);
						if (ImGui::Selectable(stateString[i].data(), isSelected))
						{
							(*iter)->m_Index = i;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Set Interact Bitmap"))
				{
					ImGui::OpenPopup("Set Interact Bitmap");
				}

				if (ImGui::BeginPopup("Set Interact Bitmap"))
				{
					ImGui::Text("Please Click the desired bitmap into the content window.");
					{
						auto UIObj = UISystem->GetDragDropBitmap();
						auto UIKey = UISystem->GetDragDropKey();
						if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
						{
							//iter에 모든 UIObject의 정보를 저장
							(*iter)->m_BitmapKeys[(*iter)->m_Index] = UIKey;
							(*iter)->m_pBitmaps[(*iter)->m_Index] = UIObj;
							UISystem->ClearDragDropBitmap();
						}
					}
					ImGui::EndPopup();
				}
			}
			else if ((*iter)->m_UIEventKey == "StageImg")
			{
				ImGui::InputInt("Player ID", &playerID);
				std::vector<std::string_view> stateString{
					"Stage1",
					"Stage2",
					"Stage3",
					"Stage4"
				};

				if (ImGui::BeginCombo("Stage Img Setting", stateString[(*iter)->m_Index].data()))
				{
					for (int i = 0; i < stateString.size(); i++)
					{
						bool isSelected = ((*iter)->m_Index == i);
						if (ImGui::Selectable(stateString[i].data(), isSelected))
						{
							(*iter)->m_Index = i;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Set Stage Bitmap"))
				{
					ImGui::OpenPopup("Set Stage Bitmap");
				}

				if (ImGui::BeginPopup("Set Stage Bitmap"))
				{
					ImGui::Text("Please Click the desired bitmap into the content window.");
					{
						auto UIObj = UISystem->GetDragDropBitmap();
						auto UIKey = UISystem->GetDragDropKey();
						if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
						{
							//iter에 모든 UIObject의 정보를 저장
							(*iter)->m_BitmapKeys[(*iter)->m_Index] = UIKey;
							(*iter)->m_pBitmaps[(*iter)->m_Index] = UIObj;
							UISystem->ClearDragDropBitmap();
						}
					}
					ImGui::EndPopup();
				}
			}
			else if ((*iter)->m_UIEventKey == "Desc")
			{
				ImGui::InputInt("Player ID", &playerID);
				std::vector<std::string_view> stateString{
					"Stage1",
					"Stage2",
					"Stage3",
					"Stage4"
				};

				if (ImGui::BeginCombo("Desc Img Setting", stateString[(*iter)->m_Index].data()))
				{
					for (int i = 0; i < stateString.size(); i++)
					{
						bool isSelected = ((*iter)->m_Index == i);
						if (ImGui::Selectable(stateString[i].data(), isSelected))
						{
							(*iter)->m_Index = i;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Set Desc Bitmap"))
				{
					ImGui::OpenPopup("Set Desc Bitmap");
				}

				if (ImGui::BeginPopup("Set Desc Bitmap"))
				{
					ImGui::Text("Please Click the desired bitmap into the content window.");
					{
						auto UIObj = UISystem->GetDragDropBitmap();
						auto UIKey = UISystem->GetDragDropKey();
						if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
						{
							//iter에 모든 UIObject의 정보를 저장
							(*iter)->m_BitmapKeys[(*iter)->m_Index] = UIKey;
							(*iter)->m_pBitmaps[(*iter)->m_Index] = UIObj;
							UISystem->ClearDragDropBitmap();
						}
					}
					ImGui::EndPopup();
				}
			}
			else if ((*iter)->m_UIEventKey == "CheckList")
			{
				std::vector<std::string_view> stateString{
					"Stage1",
					"Stage2",
					"Stage3",
					"Stage4"
				};

				if (ImGui::BeginCombo("CheckList Img Setting", stateString[(*iter)->m_Index].data()))
				{
					for (int i = 0; i < stateString.size(); i++)
					{
						bool isSelected = ((*iter)->m_Index == i);
						if (ImGui::Selectable(stateString[i].data(), isSelected))
						{
							(*iter)->m_Index = i;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Set CheckList Bitmap"))
				{
					ImGui::OpenPopup("Set CheckList Bitmap");
				}

				if (ImGui::BeginPopup("Set CheckList Bitmap"))
				{
					ImGui::Text("Please Click the desired bitmap into the content window.");
					{
						auto UIObj = UISystem->GetDragDropBitmap();
						auto UIKey = UISystem->GetDragDropKey();
						if (UIObj && InputManagement->IsMouseButtonReleased(MouseKey::LEFT))
						{
							//iter에 모든 UIObject의 정보를 저장
							(*iter)->m_BitmapKeys[(*iter)->m_Index] = UIKey;
							(*iter)->m_pBitmaps[(*iter)->m_Index] = UIObj;
							UISystem->ClearDragDropBitmap();
						}
					}
					ImGui::EndPopup();
				}
			}

			if (ImGui::Button("Add UI Event"))
			{
				if ((*iter)->m_UIEventKey == "Opacity")
				{
					UIControl->AddOpacityEventUI((*iter)->m_LayerID, (*iter));
				}
				else if ((*iter)->m_UIEventKey == "Select")
				{
					UIControl->AddSelectEventUI((*iter)->m_LayerID, (*iter));
				}
				else if ((*iter)->m_UIEventKey == "Clipping")
				{
					UIControl->AddClippingEventUI((*iter));
				}
				else if ((*iter)->m_UIEventKey == "Selected")
				{
					UIControl->AddSelectedEventUI((*iter)->m_LayerID, (*iter));
				}
				else if ((*iter)->m_UIEventKey == "StateChange")
				{
					UIControl->AddStateEventUI((*iter), playerID);
				}
				else if ((*iter)->m_UIEventKey == "FarBongsu")
				{
					UIControl->AddFarBongsuEventUI((*iter), bongsuID);
				}
				else if ((*iter)->m_UIEventKey == "Interact")
				{
					UIControl->AddInteractEventUI((*iter), playerID);
				}
				else if ((*iter)->m_UIEventKey == "Score")
				{
					(*iter)->m_IsScoreObject = true;
					UIControl->AddScoreEventUI((*iter)->m_LayerID, (*iter));
				}
				else if ((*iter)->m_UIEventKey == "Stamp")
				{
					UIControl->SetStampEventUI((*iter));
				}
				else if ((*iter)->m_UIEventKey == "Manduk")
				{
					UIControl->SetMandukEventUI((*iter));
				}
				else if ((*iter)->m_UIEventKey == "StageImg")
				{
					UIControl->SetStageImgEventUI((*iter));
				}
				else if ((*iter)->m_UIEventKey == "Desc")
				{
					UIControl->SetDescEventUI((*iter));
				}
				else if ((*iter)->m_UIEventKey == "CheckList")
				{
					UIControl->SetCheckListEventUI((*iter));
				}
			}
		}

		if (textIter != text.end())
		{
			ImGui::PushID((*textIter)->m_ZOrder);
			ImGui::Text("Object Name : %s", (*textIter)->m_Name.c_str());
			ImGui::DragFloat2("Position", &(*textIter)->Pos.x, 0.1f);
			ImGui::DragFloat("Rotation", &(*textIter)->Rotation, 0.1f);
			ImGui::DragFloat2("Scale", &(*textIter)->Scale.x, 0.01f);
			ImGui::DragInt("ZOrder", &(*textIter)->m_ZOrder, 10);
			ImGui::PopID();
			ImGui::InputText("Text", textUIBuffer, sizeof(textUIBuffer));
			if (ImGui::Button("Set Text"))
			{
				(*textIter)->SetText(ConvertToWString(textUIBuffer));
			}
			ImGui::DragFloat("FontSize", &(*textIter)->m_FontSize, 1);

			float color[4] = { 
				(*textIter)->m_Color.r,
				(*textIter)->m_Color.g,
				(*textIter)->m_Color.b,
				(*textIter)->m_Color.a
			};

			ImGui::ColorEdit4("Color", color);
			(*textIter)->m_Color = { color[0], color[1], color[2], color[3] };
			if (ImGui::BeginCombo("Font", (*textIter)->m_Key.c_str()))
			{
				for (auto& font : FontSystem->GetFontKeyList())
				{
					bool isSelected = ((*textIter)->m_Key == font);
					if (ImGui::Selectable(font.c_str(), isSelected))
					{
						(*textIter)->m_Key = font;
						(*textIter)->m_pTextFormat = FontSystem->GetFont((*textIter)->m_Key);
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Separator();
			if (ImGui::BeginCombo("Global Event Text", (*textIter)->m_EventKey.c_str()))
			{
				for (auto& [key, value] : Event->eventListners)
				{
					bool isSelected = ((*textIter)->m_EventKey == key);
					if (ImGui::Selectable(key.c_str(), isSelected))
					{
						(*textIter)->m_EventKey = key;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
	});
}

void World::Finalize()
{
	AllDestroy();
	Destroy();
}

void World::FixedUpdate(float fixedTick)
{
	if (InputManagement->IsKeyDown('V')) {
		Matrix inverseV, inverseP;

		Vector4 rayClip(0, 0, -1, 1);    // 화면의 중앙 좌표.

		inverseV = _camera->GetInvViewMatrix();
		inverseP = _camera->GetInvProjMatrix();

		Vector4 rayEye = XMVector4Transform(rayClip, inverseP);
		rayEye.z = 1.f;
		rayEye.w = 0.f;

		Vector4 rayWorld = XMVector4Transform(rayEye, inverseV);

		Vector3 rayOrigin = _camera->GetPosition();
		Vector3 rayDir = XMVector3Normalize(Vector3(rayWorld.x, rayWorld.y, rayWorld.z));

		physx::PxRaycastBuffer hitbuff;
		physx::PxVec3 pxPos(rayOrigin.x, rayOrigin.y, rayOrigin.z);
		physx::PxVec3 pxDir(rayDir.x, rayDir.y, rayDir.z);

		auto coll = raycast(rayOrigin, -rayDir, 1000.f);
		if (coll) {
			selectedGameObject = coll->GetOwner()->GetName();
			std::cout << selectedGameObject << std::endl;
		}
	}

	for (auto& obj : _objects)
	{
		if (obj->IsDestroyedMark())
		{
			continue;
		}
		obj->FixedUpdate(fixedTick);
	}
	for (auto& tempObj : _temporaryObjects)
	{
		if (tempObj->IsDestroyedMark())
		{
			continue;
		}
		tempObj->FixedUpdate(fixedTick);
	}
}

void World::Update(float tick)
{
	

	for (auto& obj : _objects)
	{
		if (obj->IsDestroyedMark())
		{
			continue;
		}
		obj->Update(tick * GameManagement->GetTimeScale());
	}

	for (auto& tempObj : _temporaryObjects )
	{
		if (tempObj->IsDestroyedMark())
		{
			continue;
		}
		tempObj->Update(tick * GameManagement->GetTimeScale());
	}

	if (_currentCanvas)
	{
		_currentCanvas->Update(tick);
	}

}

void World::LateUpdate(float tick)
{
	for (auto& obj : _objects)
	{
		if (obj->IsDestroyedMark())
		{
			continue;
		}
		obj->LateUpdate(tick);
	}
	for (auto& tempObj : _temporaryObjects)
	{
		if (tempObj->IsDestroyedMark())
		{
			continue;
		}
		tempObj->LateUpdate(tick);
	}
}

void World::Destroy()
{
	for (auto& obj : _objects)
	{
		if (obj->IsDestroyedMark())
		{
			continue;
		}

		obj->DestroyComponentStage();
	}

	for (auto& tempObj : _temporaryObjects)
	{
		if (tempObj->IsDestroyedMark())
		{
			continue;
		}
		tempObj->DestroyComponentStage();
	}

	_scene->DestroyModels();
	_currentCanvas->DestroyUI();

	std::erase_if(_objects, [](auto& obj) { return obj->IsDestroyedMark(); });
}

Object* World::GetNameObject(const std::string& name)
{
	if (_objectIds.find(name) == _objectIds.end())
	{
		return nullptr;
	}

	if (_objects.size() == 0)
	{
		return nullptr;
	}

	auto objindex = _objectIds.find(name);

	return _objects[objindex->second].get();
}

void World::CreatePrefab(const std::string& type, const std::string& prefabName, Object* targetObject)
{
	json prefab;
	std::ofstream output_file(PathFinder::Relative("Prefab\\" + prefabName + ".json").string());
	if (!output_file.is_open())
	{
		Log::Error("Failed to open file : " + prefabName);
		return;
	}

	prefab["components"] = json::array();
	prefab["PrefebType"] = type;
	prefab["componentCount"] = targetObject->_components.size();
	prefab["scale"] = { targetObject->_scale.x, targetObject->_scale.y, targetObject->_scale.z };
	prefab["rotation"] = { targetObject->_rotation.x, targetObject->_rotation.y, targetObject->_rotation.z };
	prefab["position"] = { 0, 0, 0 };

	for (auto& component : targetObject->_components)
	{
		component->Serialize(prefab);
	}

	output_file << prefab.dump(4);
	output_file.close();

	Log::Info("Save to JSON file : " + prefabName);

}

void World::LoadPrefab(const std::string& type, const std::string& prefabName, const std::string& name)
{
	std::ifstream input_file(PathFinder::Relative("Prefab\\" + prefabName + ".json").string());
	if (!input_file.is_open())
	{
		Log::Error("Failed to open file : " + prefabName);
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
	if (MetaType<Player>::type.data() == prefabType)
	{
		auto obj = CreateObject<Player>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
		return;
	}
	else if (MetaType<FloorBox>::type.data() == prefabType)
	{
		auto obj = CreateObject<FloorBox>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Wall>::type.data() == prefabType)
	{
		auto obj = CreateObject<Wall>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Stairs>::type.data() == prefabType)
	{
		auto obj = CreateObject<Stairs>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Bongsu>::type.data() == prefabType)
	{
		auto obj = CreateObject<Bongsu>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<FlagObject>::type.data() == prefabType)
	{
		auto obj = CreateObject<FlagObject>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Drum>::type.data() == prefabType)
	{
		auto obj = CreateObject<Drum>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<TorchBox>::type.data() == prefabType)
	{
		auto obj = CreateObject<TorchBox>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<WoodPile>::type.data() == prefabType)
	{
		auto obj = CreateObject<WoodPile>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Pond>::type.data() == prefabType)
	{
		auto obj = CreateObject<Pond>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Pail>::type.data() == prefabType)
	{
		auto obj = CreateObject<Pail>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Monster>::type.data() == prefabType)
	{
		auto obj = CreateObject<Monster>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<EnemyPotal>::type.data() == prefabType)
	{
		auto obj = CreateObject<EnemyPotal>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Loot>::type.data() == prefabType)
	{
		auto obj = CreateObject<Loot>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Torch>::type.data() == prefabType)
	{
		auto obj = CreateObject<Torch>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<WeekFloor>::type.data() == prefabType)
	{
		auto obj = CreateObject<WeekFloor>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else if (MetaType<Wood>::type.data() == prefabType)
	{
		auto obj = CreateObject<Wood>(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
	else
	{
		auto obj = CreateObject(name);
		obj->DeSerialize(input);
		obj->DeserializeAddcomponent(input["components"]);
	}
}

void World::DestroyObject(Object* object)
{
	object->DestroyMark();
}

void World::AllDestroy()
{
	selectedUIObject.clear();
	selectedTextUIObject.clear();
	for (auto& obj : _objects)
	{
		obj->DestroyMark();
	}
	for (auto& obj : _temporaryObjects)
	{
		obj->DestroyMark();
	}
	selectedGameObject.clear();
	delete _currentCanvas;
	_scene->AllDestroy();
	_currentCanvas = nullptr;
}

bool World::CreateCanvas()
{
	_currentCanvas = new Canvas;
	return true;
}

bool World::CreateUIObject(const std::string_view& name)
{
	if (_currentCanvas)
	{
		UIObject* obj = new UIObject;
		obj->m_Name = name;
		_currentCanvas->getObjList().push_back(obj);
	}
	else
	{
		Log::Error("Canvas is not created");
		return false;
	}

	return true;
}

bool World::CreateTextUIObject(const std::string_view& name)
{
	if (_currentCanvas)
	{
		TextUIObject* obj = new TextUIObject;
		obj->m_Name = name;
		_currentCanvas->getTextList().push_back(obj);
	}
	else
	{
		Log::Error("Canvas is not created");
		return false;
	}

	return true;
}

bool World::DestroyUIObject(const std::string_view& name)
{
	auto& objList = _currentCanvas->getObjList();
	auto iter = std::ranges::find_if(objList, [&](UIObject* obj) 
	{ 
		return obj->m_Name == name; 
	});

	if (iter != objList.end())
	{
		(*iter)->m_DestoryMark = true;
		return true;
	}
	return false;
}

bool World::DestroyTextUIObject(const std::string_view& name)
{
	auto& objList = _currentCanvas->getTextList();
	auto iter = std::ranges::find_if(objList, [&](TextUIObject* obj)
	{
		return obj->m_Name == name;
	});

	if (iter != objList.end())
	{
		(*iter)->m_DestoryMark = true;
		return true;
	}

	return false;
}

void World::Serialize(_inout json& output)  //World 직렬화 output에 직렬화된 데이터 저장
{
	json data;
	data["World"]["name"] = "jsonTestWorld"; //임시로 넣어둔 값
	data["World"]["objectCount"] = _objects.size(); //오브젝트 갯수
	//float3 pos = _camera->GetPosition();
	float3 pos{};
	XMStoreFloat3(&pos, _camera->GetPosition());
	data["World"]["Camera"]["Position"] = { pos.x, pos.y, pos.z };
	data["World"]["Camera"]["YawPitchRoll"] = { _camera->yaw, _camera->pitch, _camera->roll };
	data["World"]["Camera"]["Fov"] = _camera->_fov;
	data["World"]["Camera"]["Speed"] = _camera->_speed;
	data["World"]["Camera"]["MouseSensitivity"] = _camera->_mouseSensitivity;
	data["World"]["Camera"]["Exposure"] = _camera->exposure;
	data["World"]["Camera"]["Near"] = _camera->fnear;
	data["World"]["Camera"]["Far"] = _camera->ffar;

	data["World"]["Scene"]["suncolor"] = { _scene->suncolor.x, _scene->suncolor.y, _scene->suncolor.z };
	data["World"]["Scene"]["sunpos"] = { _scene->sunpos.x, _scene->sunpos.y, _scene->sunpos.z };

	data["World"]["objects"] = json::array(); //오브젝트 배열
	for (auto& obj : _objects)
	{
		obj->Serialize(data);
	}				//오브젝트 데이터 직렬화 오브젝트 내부 컴포넌트는 오브젝트에서 직렬화

	data["World"]["Canvas"] = json::array();
	if (_currentCanvas)
	{
		_currentCanvas->Serialize(data);
	}

	data["World"]["GridX"] = _worldX;
	data["World"]["GridY"] = _worldY;
	data["World"]["Grid"] = json::array();
	for (int i = 0; i < _worldX; i++)
	{
		for (int j = 0; j < _worldY; j++)
		{
			data["World"]["Grid"].push_back(_worldGrid[i][j]);
		}
	}

	output = data;
}

void World::UnRegisterImGui()
{
	ImGui::ContextUnregister("Scene Settings");
	ImGui::ContextUnregister("GameObjects");
	ImGui::ContextUnregister("GameObject Inspector");
	ImGui::ContextUnregister("UI Objects");
	ImGui::ContextUnregister("UI Object Inspector");
}

bool World::OutputJson(const std::string& path) //파일에 직렬화된 데이터 저장
{
	json output;
	Serialize(output);   //직렬화

	std::ofstream output_file(path); //파일 열기
	if (!output_file.is_open())	//파일 열기 실패시
	{
		Log::Error("Failed to open file : " + path);
		return false; //실패
	}

	output_file << output.dump(4); //파일에 데이터 쓰기
	output_file.close(); //파일 닫기
	Log::Info("Save to JSON file : " + path);

	return true;
}

bool World::LoadJson(const std::string& path) //파일에서 직렬화된 데이터 불러오기
{

	std::ifstream input_file(path); //파일 열기
	if (!input_file.is_open()) //파일 열기 실패시
	{
		Log::Error("Failed to open file : " + path);
		return false; //실패
	}

	json input;
	input_file >> input; //파일에서 데이터 읽기
	input_file.close(); //파일 닫기

	if (!Deserialize(input)) //데이터 역직렬화
	{
		Log::Error("Failed to deserialize");
		return false; //실패
	}
	GridEditorSystem->Initialize(this); //그리드 에디터 초기화
}

void World::LoadCanvas(const std::string& path)
{
	std::ifstream input_file(path);
	if (!input_file.is_open())
	{
		Log::Error("Failed to open file : " + path);
		return;
	}

	json input;
	input_file >> input;
	auto& canvas = input["Canvas"];

	if (!canvas.empty())
	{
		delete _currentCanvas;
		_currentCanvas = new Canvas;
		_currentCanvas->DeSerialize(canvas[0]);
	}
}

bool World::Deserialize(json& in)
{
	if (in.find("World") == in.end())
	{
		Log::Error("Invalid JSON format");
		return false;
	}
	auto& world = in["World"];
	std::string name = world["name"].get<std::string>();
	int objectCount = world["objectCount"].get<int>();

	auto& camera = world["Camera"];
	float3 pos = { camera["Position"][0].get<float>(), camera["Position"][1].get<float>(), camera["Position"][2].get<float>() };
	float3 rot = { camera["YawPitchRoll"][0].get<float>(), camera["YawPitchRoll"][1].get<float>(), camera["YawPitchRoll"][2].get<float>() };
	float fov = camera["Fov"].get<float>();
	float speed = camera["Speed"].get<float>();
	float mouseSensitivity = camera["MouseSensitivity"].get<float>();
	float exposure = camera["Exposure"].get<float>();
	float fnear = camera["Near"].get<float>();
	float ffar = camera["Far"].get<float>();

	_camera->SetPosition(pos);
	_camera->SetRotation(rot.x, rot.y, rot.z);
	_camera->_fov = fov;
	_camera->_speed = speed;
	_camera->_mouseSensitivity = mouseSensitivity;
	_camera->exposure = exposure;
	_camera->fnear = fnear;
	_camera->ffar = ffar;

	if (!world["Scene"].empty())
	{
		_scene->suncolor = { world["Scene"]["suncolor"][0].get<float>(), world["Scene"]["suncolor"][1].get<float>(), world["Scene"]["suncolor"][2].get<float>() };
		_scene->sunpos = { world["Scene"]["sunpos"][0].get<float>(), world["Scene"]["sunpos"][1].get<float>(), world["Scene"]["sunpos"][2].get<float>() };
	}

	json& objects = world["objects"];
	for (json& obj : objects)
	{
		auto name = obj["name"].get<std::string>();
		auto metaType = obj["type"].get<std::string>();
		auto object = ObjectFactory::GetInstance()->CreateObject(metaType);
		object->_name = name;
		_objectIds[name] = _objects.size();
		_objects.push_back(std::unique_ptr<Object>(object));

		//auto object = CreateObject(name);
		
		object->DeSerialize(obj);
		object->DeserializeAddcomponent(obj["components"]);
	}

	auto& canvas = in["Canvas"];
	if (!canvas.empty())
	{
		_currentCanvas = new Canvas;
		_currentCanvas->DeSerialize(canvas[0]);
	}

	if (!world["Grid"].empty())
	{
		_worldX = world["GridX"].get<int>();
		_worldY = world["GridY"].get<int>();
		_worldGrid = std::vector<std::vector<int>>(_worldX, std::vector<int>(_worldY, 0));
		auto& grid = world["Grid"];
		for (int i = 0; i < _worldX; i++)
		{
			for (int j = 0; j < _worldY; j++)
			{
				_worldGrid[i][j] = grid[i * _worldY + j].get<int>();
			}
		}
	}

	return true;
}


void World::PreObjectLoad()
{
	try 
	{
		// interactable - 12개
		RegisterObject<Object>();
		RegisterObject<Bongsu>();
		RegisterObject<Drum>();
		RegisterObject<FlagObject>();
		RegisterObject<Loot>();
		RegisterObject<Pail>();
		RegisterObject<Pond>();
		RegisterObject<Torch>();
		RegisterObject<TorchBox>();
		RegisterObject<WeekFloor>();
		RegisterObject<Wood>();
		RegisterObject<WoodPile>();

		// movable - 2개
		RegisterObject<Player>();
		RegisterObject<Monster>();

		// non-interactable - 4개
		RegisterObject<EnemyPotal>();
		RegisterObject<FloorBox>();
		RegisterObject<Stairs>();
		RegisterObject<Wall>();
	}
	catch (const std::exception& e)
	{
		Log::Error("Failed to register object : " + std::string(e.what()));
	}
}

void World::PreComponentLoad()
{
	try 
	{
		UnregisterAllComponent();
		RegisterComponent<BoxColliderComponent>();
		RegisterComponent<SphereColliderComponent>();
		RegisterComponent<StairColliderComponent>();
		RegisterComponent<RigidbodyComponent>();
		RegisterComponent<RenderComponent>(_scene);
		RegisterComponent<FireComponent>(_scene);
		RegisterComponent<TorchComponet>(_scene);
		RegisterComponent<SpawnerComponent>();
		RegisterComponent<TestComponent>();
		RegisterComponent<CharacterController>();
	}
	catch (const std::exception& e)
	{
		Log::Error("Failed to register component : " + std::string(e.what()));
	}
}