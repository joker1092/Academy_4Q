#pragma once
#include "TypeDefinition.h"
#include "MemoryPool.h"
#include <concepts>
#include <vector>
#include <list>

template<typename T>
concept Pointer =
std::is_pointer_v<T> ||
std::is_null_pointer_v<T> ||
std::derived_from<T, IUnknown>;

namespace Memory
{
	//�޸� �Ҵ� �� ����
	inline void AllocateAndCopy(void* pDst, const void* pSrc, uint32 size)
	{
		pDst = malloc(size);
		memcpy(pDst, pSrc, size);
	}
  //�޸� ���� : IUnknown�� ��ӹ��� Ŭ������ ��� Release() ȣ�� -> �� �� delete ȣ��
    void SafeDelete(Pointer auto& ptr)
    {
        if constexpr (std::is_pointer_v<decltype(ptr)>)
        {
            if constexpr (std::derived_from<decltype(ptr), IUnknown>)
            {
                ptr->Release();
            }
            else
            {
                delete ptr;
                ptr = nullptr;
            }
        }
    }
}
//���� ������ : ���� �����̳ʿ� ����� ���(������)���� �������� ����� �����ϴ� Ŭ���� -> function ��ü�� ����Ͽ� ���� ������ ������ �� ����
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
//���ø� �߷� ���̵�
template<typename T, typename Allocator>
DeferredDeleter(std::vector<T*, Allocator>*, auto) -> DeferredDeleter<T, std::vector<T*, Allocator>>;

