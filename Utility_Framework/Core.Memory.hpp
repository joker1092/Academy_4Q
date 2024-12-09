#pragma once
#include "TypeDefinition.h"
#include "LinkedListLib.hpp"
#include <vector>
#include <list>

namespace Memory
{
	//메모리 할당 및 복사
	inline void AllocateAndCopy(void* pDst, const void* pSrc, uint32 size)
	{
		pDst = malloc(size);
		memcpy(pDst, pSrc, size);
	}
	//메모리 해제 : IUnknown을 상속받은 클래스의 경우 Release() 호출 -> void*의 경우 free() 호출 -> 그 외 delete 호출
	template<typename T>
	inline void SafeDelete(T*& ptr)
	{
		if constexpr (std::is_base_of_v<IUnknown, T>)
		{
			if (ptr)
			{
				ptr->Release();
				ptr = nullptr;
			}
		}
		else if constexpr (std::is_same_v<T, void>)
		{
			if (ptr)
			{
				free(ptr);
				ptr = nullptr;
			}
		}
		else
		{
			if (ptr)
			{
				delete ptr;
				ptr = nullptr;
			}
		}
	}
}
//지연 삭제자 : 연속 컨테이너에 저장된 요소(포인터)들을 스코프가 벗어나면 삭제하는 클래스 -> function 객체를 사용하여 삭제 조건을 지정할 수 있음
template<typename T, typename Container>
class DeferredDeleter final
{
public:
	using Func = std::function<bool(T*)>;
public:
	DeferredDeleter() = default;
	DeferredDeleter(Container* container, Func func = [](T* ptr) { return true; }) : _container(container), m_deleteElementFunc(func) {}
	~DeferredDeleter()
	{
		for (auto& ptr : *_container)
		{
			if (m_deleteElementFunc(ptr))
			{
				Memory::SafeDelete(ptr);
			}
		}

		_container->erase(std::remove(_container->begin(), _container->end(), nullptr), _container->end());
	}

	void operator()(Container* container)
	{
		_container = container;
	}

private:
	Container* _container{};
	Func m_deleteElementFunc{};
};
//템플릿 추론 가이드
template<typename T, typename Allocator>
DeferredDeleter(std::vector<T*, Allocator>*, auto) -> DeferredDeleter<T, std::vector<T*, Allocator>>;

