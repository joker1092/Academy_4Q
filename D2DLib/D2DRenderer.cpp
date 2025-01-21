#include "pch.h"
#include "D2DRenderer.h"
#include <memory>
#include "../Utility_Framework/CoreWindow.h"




DirectX11::D2DRenderer::D2DRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources)
{
	m_DeviceResources = deviceResources;
	ID2D1Factory* D2DFac = deviceResources.get()->GetD2DFactory();
	CoreWindow* CoreWindo = deviceResources.get()->GetWindow();
	HWND hWnd = CoreWindo->GetHandle();
	D2D1_SIZE_U size = D2D1::SizeU(CoreWindo->GetWidth(), CoreWindo->GetHeight());
	D2DFac->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, size),
		&m_D2DRenderTarget
	);
}

DirectX11::D2DRenderer::~D2DRenderer()
{
	m_DeviceResources.reset();
}



HRESULT DirectX11::D2DRenderer::LoadBitmapFromFile(ID2D1HwndRenderTarget* pRenderTarget, IWICImagingFactory2* pIWICFactory, PCWSTR uri, std::wstring _name, ID2D1Bitmap** ppBitmap)
{
	//decode wic �ҷ��°��� �� Ǯ�������
	IWICBitmapDecoder* pDecoder = NULL;
	//decoder�� Ǯ� ���� ���������� �ҷ��ͼ� ����
	IWICBitmapFrameDecode* pSource = NULL;
	//IWICStream* pStream = NULL;
	//wic decoder�κ��� ������ �̹����� D2D��Ʈ������ ��ȯ�ϱ� ���� ����������
	IWICFormatConverter* pConverter = NULL;
	//IWICBitmapScaler* pScaler = NULL;

	//1.
	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
	);
	//1. �̹��� ������ ���ڵ��ϱ� ���� WICImagingFactory�� CreateDecoderFromFilename�Լ��� ����Ͽ� ���ڴ��� �����մϴ�.

	//2.
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	//2. �̹����� ���� ���˿� ���� ������ ���� �� ���ڴ��κ��� ù��° �������� �����ͼ� �޸𸮿� ��Ʈ���� ����!

	//3.
	if (SUCCEEDED(hr))
	{

		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);

	}

	//3. �̾��� �ܰ迡�� WICImagingFactory�� FormatConverter�� ����Ͽ� �̹����� ��ȯ�� ���Դϴ�.
	// ��ȯ�� PNG,GIF,BMP�� WIM(Windows Image Format)���� 32bppPBGRA�� ��ȯ�� ���Դϴ�. �̴� D2Dd���� ����ϴ� ��Ʈ�� �����Դϴ�.
	// �װ� ���� ���������͸� �����մϴ�.


	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
		);
	}

	//4. ������������ Initialize�Լ��� ����Ͽ� pSource���� pConverter�� �̹����� ��ȯ�� �غ� �մϴ�.
	// png,jepg,bmp,gif -> ID2D1Bitmap* pRenderTarget�� �̰� ����
	//GUID_WICPixelFormat32bppPBGRA -> ID2D1Bitmap pRenderTarget�̰Ŵ� ��
	//png -> GUID_WICPixelFormat32bppPBGRA �̰� ���ִ°� wic�̴�.
	//png -> GUID_WICPixelFormat32bppPBGRA -> ID2D1Bitmap*
	if (SUCCEEDED(hr))
	{

		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}



	return hr;
}
//
//HRESULT DirectX11::D2DRenderer::InitD2DRender(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources)
//{	
//	m_DeviceResources = deviceResources;
//
//	HRESULT hr = S_OK;
//
//
//	ID2D1Factory* D2DFac = deviceResources.get()->GetD2DFactory();
//	CoreWindow* CoreWindo = deviceResources.get()->GetWindow();
//	HWND hWnd = CoreWindo->GetHandle();
//	D2D1_SIZE_U size = D2D1::SizeU(CoreWindo->GetWidth(), CoreWindo->GetHeight());
//	hr = D2DFac->CreateHwndRenderTarget(
//		D2D1::RenderTargetProperties(),
//		D2D1::HwndRenderTargetProperties(hWnd, size),
//		&m_D2DRenderTarget
//	);
//
//	return hr;
//}
