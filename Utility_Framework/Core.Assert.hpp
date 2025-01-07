#pragma once
#include <cstdlib>
#include <cstdio>
#include <Windows.h>
//EASTL�� ASSERT�� ���
namespace Core
{
    // Assertion ���� �� ȣ��� �Լ� ������
    using AssertionFailureFunction = void (*)(const char* expression, void* context);

    // �⺻ Assertion ���� ó�� �Լ�
    inline void AssertionFailureFunctionDefault(const char* expression, void* context)
    {
        // �⺻������ ���� �޽��� ���
        printf("Assertion failed: %s\n", expression);

        // ����� ȯ�濡�� �ߴ�
#if defined(_DEBUG)
        if (::IsDebuggerPresent())
        {
            OutputDebugStringA(expression);  // ����� ������ �޽��� ���
        }
        __debugbreak();  // ����� �ߴ�
#endif

        // ���α׷� ����
        std::abort();
    }

    // ���� ������ Assertion ���� �Լ��� ���ؽ�Ʈ ����
    static AssertionFailureFunction g_assertionFailureFunction = AssertionFailureFunctionDefault;
    static void* g_assertionFailureFunctionContext = nullptr;

    // Assertion ���� �Լ� ����
    inline void SetAssertionFailureFunction(AssertionFailureFunction failureFunction, void* context)
    {
        g_assertionFailureFunction = failureFunction;
        g_assertionFailureFunctionContext = context;
    }

    // Assertion ���� ó�� �Լ� ȣ��
    inline void AssertionFailure(const char* expression)
    {
        if (g_assertionFailureFunction)
        {
            g_assertionFailureFunction(expression, g_assertionFailureFunctionContext);
        }
    }
}

// CORE_ASSERT �Լ� ����
#ifndef CORE_ASSERT
#ifdef NDEBUG
#define CORE_ASSERT(expression) ((void)0)  // NDEBUG���� assert ��Ȱ��ȭ
#else
#define CORE_ASSERT(expression) \
    if (!(expression))           \
    {                            \
        Core::AssertionFailure(#expression); \
    }
#endif
#endif // !CORE_ASSERT
