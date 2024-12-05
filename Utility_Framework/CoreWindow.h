#pragma once
#include <windows.h>
#include <functional>
#include <unordered_map>

#pragma warning(disable: 28251)
#define MAIN_ENTRY int WINAPI

class CoreWindow
{
public:
    using MessageHandler = std::function<LRESULT(HWND, WPARAM, LPARAM)>;

    CoreWindow(HINSTANCE hInstance, const wchar_t* title, int width, int height)
        : m_hInstance(hInstance), m_width(width), m_height(height)
    {
        RegisterWindowClass();
        CreateAppWindow(title);
        s_instance = this;
    }

    ~CoreWindow()
    {
        if (m_hWnd)
        {
            DestroyWindow(m_hWnd);
        }
        UnregisterClass(L"CoreWindowApp", m_hInstance);
    }

    // �޽��� �ڵ鷯 ���
    template <typename Instance>
    void RegisterHandler(UINT message, Instance* instance, LRESULT(Instance::* handler)(HWND, WPARAM, LPARAM))
    {
        m_handlers[message] = [=](HWND hWnd, WPARAM wParam, LPARAM lParam)
            {
                return (instance->*handler)(hWnd, wParam, lParam);
            };
    }

    template <typename Initializer>
    CoreWindow InitializeTask(Initializer fn_initializer)
    {
        fn_initializer();
        return *this;
    }

    // �޽��� ���� ����
    template <typename MessageLoop>
    void Then(MessageLoop fn_messageLoop)
    {
        MSG msg = {};
        while (true)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }
                else
                {
                    DispatchMessage(&msg);
                }
            }
            else
            {
                fn_messageLoop();
            }
        }
    }

    // ������ �ڵ� ��ȯ
    HWND GetHandle() const { return m_hWnd; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    static CoreWindow* GetForCurrentInstance()
    {
        return s_instance;
    }

private:
    static CoreWindow* s_instance;
    HINSTANCE m_hInstance = nullptr;
    HWND m_hWnd = nullptr;
    int m_width = 800;
    int m_height = 600;
    std::unordered_map<UINT, MessageHandler> m_handlers;

    // ������ Ŭ���� ���
    void RegisterWindowClass()
    {
        WNDCLASS wc = {};
        wc.lpfnWndProc = CoreWindow::WndProc;
        wc.hInstance = m_hInstance;
        wc.lpszClassName = L"CoreWindowApp";

        RegisterClass(&wc);
    }

    // ������ ����
    void CreateAppWindow(const wchar_t* title)
    {
        RECT rect{};
        GetWindowRect(GetDesktopWindow(), &rect);
        int x = (rect.right - rect.left - m_width) / 2;
        int y = (rect.bottom - rect.top - m_height) / 2;

        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

        m_hWnd = CreateWindowEx(
            0,
            L"CoreWindowApp",
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            m_width, m_height,
            nullptr,
            nullptr,
            m_hInstance,
            this);

        if (m_hWnd)
        {
            ShowWindow(m_hWnd, SW_SHOWNORMAL);
            UpdateWindow(m_hWnd);
        }
    }

    // ������ ���ν���
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        CoreWindow* self = nullptr;

        if (message == WM_NCCREATE)
        {
            // ������ ���� �� �ʱ�ȭ
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            self = static_cast<CoreWindow*>(cs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        }
        else
        {
            self = reinterpret_cast<CoreWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (self)
        {
            return self->HandleMessage(hWnd, message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    // �޽��� ó��
    LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto it = m_handlers.find(message);
        if (it != m_handlers.end())
        {
            return it->second(hWnd, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
};