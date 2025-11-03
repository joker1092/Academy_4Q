#pragma once
#include "Utility_Framework/Core.Definition.h"
#include <string>
#include <functional>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include "Object.h"		//base object
#include "FloorBox.h"	//바닥블록
#include "Wall.h"		//벽
#include "Stairs.h"		//계단
#include "WeekFloor.h"  //약한바닥
#include "Bongsu.h"		//봉수대
#include "FlagObject.h" //횃대
#include "Drum.h"		//북
#include "TorchBox.h"	//횃대상자
#include "WoodPile.h"   //나무더미
#include "Pond.h" 		//연못
#include "Pail.h"		//통
#include "Player.h"		//플레이어
#include "Monster.h"	//몬스터
#include "Wood.h"		//나무
#include "Torch.h"		//횃불
#include "Loot.h" 		//전리품
#include "EnemyPotal.h"	//몬스터 포탈



template<typename T>
concept ObjectType = std::is_base_of<Object, T>::value;

class ObjectFactory : public Singleton<ObjectFactory>
{
	friend class Singleton;
public:
	using CreateFunc = std::function<Object* ()>;
	void RegisterObject(const std::string& type, CreateFunc creator)
	{
		if (_creators.find(type) != _creators.end())
		{
			throw std::runtime_error("Object type already registered");
		}
		_creators[type] = creator;
	}
	template<ObjectType T, typename... Args>
	void RegisterObjectArgs(const std::string& type, Args&... args)
	{
		RegisterObject(type, [&args...]() -> T*
			{
				return new T(args...);
			});
	}


	Object* CreateObject(const std::string& type) {
		auto it = _creators.find(type);
		if (it == _creators.end())
		{
			throw std::runtime_error("Object type not registered" + type);
		}
		return it->second();
	}
private:
	std::unordered_map<std::string, CreateFunc> _creators;
	ObjectFactory() = default;
	~ObjectFactory() = default;
};

template<ObjectType T>
inline void RegisterObject()
{
	ObjectFactory::GetInstance()->RegisterObject(MetaType<T>::type.data(), []() -> T*
		{
			return new T();
		});
}

template<ObjectType T, typename... Args>
inline void RegisterObject(Args&... args)
{
	ObjectFactory::GetInstance()->RegisterObjectArgs<T>(MetaType<T>::type.data(), args...);
}