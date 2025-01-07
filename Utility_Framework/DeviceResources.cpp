#include "DirectXHelper.h"
#include "DeviceResources.h"
#include "CoreWindow.h"
#include "DirectXMath.h"
using namespace DirectX;

namespace DisplayMetrics
{
    // ���ػ� ���÷��̴� �������ϴ� �� ���� GPU �� ���͸� ������ �ʿ��� �� �ֽ��ϴ�.
    // ���� ��� ���ػ� �޴����� ���ӿ��� ��ȭ���� �ʴ� 60�������� �������Ϸ���
    // ��� ª�� ���͸� �������� ���� ������ �߻��� �� �ֽ��ϴ�.
    // ��� �÷��� �� �� ���Ϳ��� ��ȭ���� �������ϴ� ������
    // �����ϰ� ������ �մϴ�.
    static const bool SupportHighResolutions = false;

    // �����ػ󵵡� ���÷��̸� �����ϴ� �⺻ �Ӱ谪�Դϴ�. �Ӱ谪�� �ʰ��ϰų�
    // SupportHighResolutions�� false�� ��� ũ�Ⱑ 50%��
    //�پ��ϴ�.
    static const float DpiThreshold = 192.0f;		// ǥ�� ����ũ�� ���÷����� 200%�Դϴ�.
    static const float WidthThreshold = 1920.0f;	// �ʺ� 1080p�Դϴ�.
    static const float HeightThreshold = 1080.0f;	// ���̰� 1080p�Դϴ�.
};

DirectX11::DeviceResources::DeviceResources() :
    m_screenViewport(),
    m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
    m_d3dRenderTargetSize(),
    m_outputSize(),
    m_logicalSize(),
    m_dpi(-1.0f),
    m_effectiveDpi(-1.0f),
    m_deviceNotify(nullptr)
{
    CreateDeviceIndependentResources();
    CreateDeviceResources();
}

void DirectX11::DeviceResources::SetWindow(CoreWindow& window)
{
    m_window = &window;
    m_logicalSize = { static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight()) };
    m_dpi = static_cast<float>(GetDpiForWindow(window.GetHandle()));
    //m_d2dContext->SetDpi(m_dpi, m_dpi);

    CreateWindowSizeDependentResources();
}

void DirectX11::DeviceResources::SetLogicalSize(Size logicalSize)
{
    if (m_logicalSize.width != logicalSize.width || m_logicalSize.height != logicalSize.height)
    {
        m_logicalSize = logicalSize;
        CreateWindowSizeDependentResources();
    }
}

void DirectX11::DeviceResources::SetDpi(float dpi)
{
    if (m_dpi != dpi)
    {
        m_dpi = dpi;
        m_effectiveDpi = dpi;
        m_d2dContext->SetDpi(m_dpi, m_dpi);
        CreateWindowSizeDependentResources();
    }
}

void DirectX11::DeviceResources::ValidateDevice()
{
    // �⺻ ����Ͱ� ����̽��� ������� ���Ŀ� ����ǰų� ����̽��� ���ŵ� ���
    // D3D ����̽��� �� �̻� ��ȿ���� �ʽ��ϴ�.
    // ����, ����̽��� ������� ���� �⺻ ����Ϳ� ���� ������ �����ɴϴ�.

    ComPtr<IDXGIDevice3> dxgiDevice;
    DirectX11::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

    ComPtr<IDXGIAdapter> deviceAdapter;
    DirectX11::ThrowIfFailed(dxgiDevice->GetAdapter(&deviceAdapter));

    ComPtr<IDXGIFactory4> deviceFactory;
    DirectX11::ThrowIfFailed(deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));

    ComPtr<IDXGIAdapter1> previousDefaultAdapter;
    DirectX11::ThrowIfFailed(deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));

    DXGI_ADAPTER_DESC1 previousDesc;
    DirectX11::ThrowIfFailed(previousDefaultAdapter->GetDesc1(&previousDesc));

    // ��������, ���� �⺻ ����Ϳ� ���� ������ �����ɴϴ�.
    ComPtr<IDXGIFactory4> currentFactory;
    DirectX11::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));

    ComPtr<IDXGIAdapter1> currentDefaultAdapter;
    DirectX11::ThrowIfFailed(currentFactory->EnumAdapters1(0, &currentDefaultAdapter));

    DXGI_ADAPTER_DESC1 currentDesc;
    DirectX11::ThrowIfFailed(currentDefaultAdapter->GetDesc1(&currentDesc));

    // ����� LUID�� ��ġ���� �ʰų� ����̽��� ���ŵǾ��ٰ� �����ϴ� ���
    // �� D3D ����̽��� ������ �մϴ�.

    if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
        previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
        FAILED(m_d3dDevice->GetDeviceRemovedReason()))
    {
        // ���� ����̽��� ���õ� ���ҽ��� ���� ������ �����մϴ�.
        dxgiDevice = nullptr;
        deviceAdapter = nullptr;
        deviceFactory = nullptr;
        previousDefaultAdapter = nullptr;

        // �� ����̽� �� ���� ü���� ����ϴ�.
        HandleDeviceLost();
    }
}

void DirectX11::DeviceResources::HandleDeviceLost()
{
    m_swapChain = nullptr;

    if (m_deviceNotify != nullptr)
    {
        m_deviceNotify->OnDeviceLost();
    }

    CreateDeviceResources();
    m_d2dContext->SetDpi(m_dpi, m_dpi);
    CreateWindowSizeDependentResources();

    if (m_deviceNotify != nullptr)
    {
        m_deviceNotify->OnDeviceRestored();
    }
}

void DirectX11::DeviceResources::RegisterDeviceNotify(IDeviceNotify* deviceNotify)
{
    m_deviceNotify = deviceNotify;
}

void DirectX11::DeviceResources::Trim()
{
    ComPtr<IDXGIDevice3> dxgiDevice;
    m_d3dDevice.As(&dxgiDevice);

    dxgiDevice->Trim();
}

void DirectX11::DeviceResources::Present()
{
    // ù ��° �μ��� DXGI�� VSync���� �����ϵ��� �����Ͽ� ���ø����̼���
    // ���� VSync���� ����ϵ��� �մϴ�. �̸� ���� ȭ�鿡 ǥ�õ��� �ʴ� ��������
    // �������ϴ� �ֱ⸦ �������� ���� �� �ֽ��ϴ�.
    DXGI_PRESENT_PARAMETERS parameters = { 0 };
    HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

    // ������ ����� �������� �����մϴ�.
    // �� �۾��� ���� �������� ������ ��� ��쿡��
    // �ùٸ��ϴ�. ���� �Ǵ� ��ũ�� ������ ���Ǵ� ��쿡�� �� ȣ���� �����ؾ� �մϴ�.
    m_d3dContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0);

    // ���� ���ٽ��� �������� �����մϴ�.
    m_d3dContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0);

    // ������ ����ų� ����̹� ���׷��̵�� ���� ����̽��� ���ŵǸ� 
    // ��� ����̽� ���ҽ��� �ٽ� ������ �մϴ�.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        HandleDeviceLost();
    }
    else
    {
        DirectX11::ThrowIfFailed(hr);
    }
}

void DirectX11::DeviceResources::CreateDeviceIndependentResources()
{
    // Direct2D ���ҽ��� �ʱ�ȭ�մϴ�.
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
    // ������Ʈ�� ����� ���� ���� ��� SDK ���̾ ���� Direct2D ������� ����մϴ�.
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    // Direct2D ���͸��� �ʱ�ȭ�մϴ�.
    DirectX11::ThrowIfFailed(
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory3),
            &options,
            &m_d2dFactory
        )
    );

    // DirectWrite ���͸��� �ʱ�ȭ�մϴ�.
    DirectX11::ThrowIfFailed(
        DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory3),
            &m_dwriteFactory
        )
    );

    // WIC(Windows Imaging Component) ���͸��� �ʱ�ȭ�մϴ�. //�̰� �� ������? ������
    //DirectX11::ThrowIfFailed(
    //	CoCreateInstance(
    //		CLSID_WICImagingFactory2,
    //		nullptr,
    //		CLSCTX_INPROC_SERVER,
    //		IID_PPV_ARGS(&m_wicFactory)
    //	)
    //);
}

void DirectX11::DeviceResources::CreateDeviceResources()
{
    // �� �÷��״� API �⺻���� �ٸ� �� ä�� ������ ǥ�鿡 ���� ������
    // �߰��մϴ�. Direct2D���� ȣȯ���� ���� �ʿ��մϴ�.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    if (DirectX11::SdkLayersAvailable())
    {
        // ������Ʈ�� ����� ���� ���� ��쿡�� �� �÷��װ� �ִ� SDK ���̾ ���� ������� ����Ͻʽÿ�.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
#endif

    // �� �迭�� �� ���� ���α׷����� �����ϴ� DirectX �ϵ���� ��� ���� ������ �����մϴ�.
    // ������ �����ؾ� �մϴ�.
    // ������ ���ø����̼ǿ� �ʿ��� �ּ� ��� ������ �����ؾ� �մϴ�.
    // ������ �������� ���� ��� ��� ���ø����̼��� 9.1�� �����ϴ� ������ ���ֵ˴ϴ�.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Direct3D 11 API ����̽� ��ü�� �ش� ���ؽ�Ʈ�� ����ϴ�.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    HRESULT hr = D3D11CreateDevice(
        nullptr,						// �⺻ ����͸� ����Ϸ��� nullptr�� �����մϴ�.
        D3D_DRIVER_TYPE_HARDWARE,	// �ϵ���� �׷��� ����̹��� ����Ͽ� ����̽��� ����ϴ�.
        0,							// ����̹��� D3D_DRIVER_TYPE_SOFTWARE�� �ƴ� ��� 0�̾�� �մϴ�.
        creationFlags,						// ����� �� Direct2D ȣȯ�� �÷��׸� �����մϴ�.
        featureLevels,			// �� ���� ���α׷��� ������ �� �ִ� ��� ���� ����Դϴ�.
        ARRAYSIZE(featureLevels),			// �� ����� ũ���Դϴ�.
        D3D11_SDK_VERSION,			// Microsoft Store ���� ��� �׻� �� ���� D3D11_SDK_VERSION���� �����մϴ�.
        &device,						// ������� Direct3D ����̽��� ��ȯ�մϴ�.
        &m_d3dFeatureLevel,		// ������� ����̽��� ��� ������ ��ȯ�մϴ�.
        &context				// ����̽� ���� ���ؽ�Ʈ�� ��ȯ�մϴ�.
    );

    if (FAILED(hr))
    {
        // �ʱ�ȭ�� �����ϸ� WARP ����̽��� ��ü�˴ϴ�.
        // WARP�� ���� �ڼ��� ������ ������ �����ϼ���. 
        // https://go.microsoft.com/fwlink/?LinkId=286690
        DirectX11::ThrowIfFailed(
            D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP, // �ϵ���� ����̽� ��� WARP ����̽��� ����ϴ�.
                0,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &device,
                &m_d3dFeatureLevel,
                &context
            )
        );
    }

    // Direct3D 11.3 API ����̽� �� ���� ���ؽ�Ʈ�� ���� �����͸� �����մϴ�.
    DirectX11::ThrowIfFailed(
        device.As(&m_d3dDevice)
    );

    DirectX11::ThrowIfFailed(
        context.As(&m_d3dContext)
    );

    // Direct2D ����̽� ��ü �� �ش� ���ؽ�Ʈ�� ����ϴ�.
    ComPtr<IDXGIDevice3> dxgiDevice;
    DirectX11::ThrowIfFailed(
        m_d3dDevice.As(&dxgiDevice)
    );

    DirectX11::ThrowIfFailed(
        m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice)
    );

    DirectX11::ThrowIfFailed(
        m_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &m_d2dContext
        )
    );
}

void DirectX11::DeviceResources::CreateWindowSizeDependentResources()
{
    // ���� â ũ��� ���õ� ���ؽ�Ʈ�� ����ϴ�.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    m_d3dRenderTargetView = nullptr;
    //m_d2dContext->SetTarget(nullptr);
    //m_d2dTargetBitmap = nullptr;
    m_d3dDepthStencilView = nullptr;
    m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

    UpdateRenderTargetSize();

    if (nullptr != m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(
            2,
            lround(m_d3dRenderTargetSize.width),
            lround(m_d3dRenderTargetSize.height),
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0
        );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            HandleDeviceLost();
            return;
        }
        else
        {
            DirectX11::ThrowIfFailed(hr);
        }
    }
    else
    {
        DXGI_SCALING scaling = DisplayMetrics::SupportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

        m_d3dRenderTargetSize.width = m_logicalSize.width;
        m_d3dRenderTargetSize.height = m_logicalSize.height;

        swapChainDesc.Width = lround(m_d3dRenderTargetSize.width);		// â�� ũ�⸦ ����ϴ�.
        swapChainDesc.Height = lround(m_d3dRenderTargetSize.height);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;				// ���� �Ϲ����� ���� ü�� �����Դϴ�.
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1;								// ���� ���ø��� ������� ���ʽÿ�.
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;									// ���� ���۸��� ����Ͽ� ��� �ð��� �ּ�ȭ�մϴ�.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// ��� Microsoft Store ���� �� SwapEffect�� ����ؾ� �մϴ�.
        swapChainDesc.Flags = 0;
        swapChainDesc.Scaling = scaling;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        ComPtr<IDXGIDevice3> dxgiDevice;
        DirectX11::ThrowIfFailed(
            m_d3dDevice.As(&dxgiDevice)
        );

        ComPtr<IDXGIAdapter> dxgiAdapter;
        DirectX11::ThrowIfFailed(
            dxgiDevice->GetAdapter(&dxgiAdapter)
        );

        ComPtr<IDXGIFactory2> dxgiFactory;
        DirectX11::ThrowIfFailed(
            dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
        );

        ComPtr<IDXGISwapChain1> swapChain;
        DirectX11::ThrowIfFailed(
            dxgiFactory->CreateSwapChainForHwnd(
                m_d3dDevice.Get(),
                m_window->GetHandle(),
                &swapChainDesc,
                nullptr,
                nullptr,
                &swapChain
            )
        );

        DirectX11::ThrowIfFailed(
            swapChain.As(&m_swapChain)
        );

        DirectX11::ThrowIfFailed(
            dxgiDevice->SetMaximumFrameLatency(1)
        );

        ComPtr<ID3D11Texture2D1> backBuffer;
        DirectX11::ThrowIfFailed(
            m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))
        );

        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateRenderTargetView1(
                backBuffer.Get(),
                nullptr,
                &m_d3dRenderTargetView
            )
        );

        // �ʿ��� ��� 3D �������� ����� ���� ���ٽ� �並 ����ϴ�.
        CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            lround(m_d3dRenderTargetSize.width),
            lround(m_d3dRenderTargetSize.height),
            1, // �� ���� ���ٽ� ��� �ϳ��� ������ ������ �ֽ��ϴ�.
            1, // ���� MIP �� ������ ����մϴ�.
            D3D11_BIND_DEPTH_STENCIL
        );

        ComPtr<ID3D11Texture2D1> depthStencil;
        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateTexture2D1(
                &depthStencilDesc,
                nullptr,
                &depthStencil
            )
        );

        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
        DirectX11::ThrowIfFailed(
            m_d3dDevice->CreateDepthStencilView(
                depthStencil.Get(),
                &depthStencilViewDesc,
                &m_d3dDepthStencilView
            )
        );

        m_screenViewport = CD3D11_VIEWPORT(
            0.0f,
            0.0f,
            m_d3dRenderTargetSize.width,
            m_d3dRenderTargetSize.height
        );

        m_d3dContext->RSSetViewports(1, &m_screenViewport);

        // ���� ü�� �� ���ۿ� ����� Direct2D ���
        // ��Ʈ���� ����� �̸� ���� ������� �����մϴ�.
        D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                m_dpi,
                m_dpi
            );

        ComPtr<IDXGISurface2> dxgiBackBuffer;
        DirectX11::ThrowIfFailed(
            m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
        );

        DirectX11::ThrowIfFailed(
            m_d2dContext->CreateBitmapFromDxgiSurface(
                dxgiBackBuffer.Get(),
                &bitmapProperties,
                &m_d2dTargetBitmap
            )
        );

        m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
        m_d2dContext->SetDpi(m_effectiveDpi, m_effectiveDpi);
        m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    }
}

void DirectX11::DeviceResources::UpdateRenderTargetSize()
{
    m_effectiveDpi = m_dpi;

    if (!DisplayMetrics::SupportHighResolutions && m_dpi > DisplayMetrics::DpiThreshold)
    {
        float width = DirectX11::ConvertDipsToPixels(m_logicalSize.width, m_dpi);
        float height = DirectX11::ConvertDipsToPixels(m_logicalSize.height, m_dpi);
        if (max(width, height) > DisplayMetrics::WidthThreshold && min(width, height) > DisplayMetrics::HeightThreshold)
        {
            m_effectiveDpi /= 2.0f;
        }
    }

    m_outputSize.width = DirectX11::ConvertDipsToPixels(m_logicalSize.width, m_effectiveDpi);
    m_outputSize.height = DirectX11::ConvertDipsToPixels(m_logicalSize.height, m_effectiveDpi);

    m_outputSize.width = max(m_outputSize.width, 1);
    m_outputSize.height = max(m_outputSize.height, 1);
}
