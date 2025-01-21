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
	//decode wic 불러온것을 다 풀어버리것
	IWICBitmapDecoder* pDecoder = NULL;
	//decoder로 풀어낸 것을 프레임으로 불러와서 저장
	IWICBitmapFrameDecode* pSource = NULL;
	//IWICStream* pStream = NULL;
	//wic decoder로부터 가져온 이미지를 D2D비트맵으로 변환하기 위한 포맷컨버터
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
	//1. 이미지 파일을 디코딩하기 위해 WICImagingFactory의 CreateDecoderFromFilename함수를 사용하여 디코더를 생성합니다.

	//2.
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	//2. 이미지의 파일 포맷에 따른 압축을 해제 후 디코더로부터 첫번째 프레임을 가져와서 메모리에 비트맵을 생성!

	//3.
	if (SUCCEEDED(hr))
	{

		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);

	}

	//3. 이어질 단계에서 WICImagingFactory의 FormatConverter를 사용하여 이미지를 변환할 것입니다.
	// 변환은 PNG,GIF,BMP등 WIM(Windows Image Format)들을 32bppPBGRA로 변환할 것입니다. 이는 D2Dd에서 사용하는 비트맵 포맷입니다.
	// 그걸 위한 포맷컨버터를 생성합니다.


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

	//4. 포맷컨버터의 Initialize함수를 사용하여 pSource에서 pConverter로 이미지를 변환할 준비를 합니다.
	// png,jepg,bmp,gif -> ID2D1Bitmap* pRenderTarget이 이걸 못해
	//GUID_WICPixelFormat32bppPBGRA -> ID2D1Bitmap pRenderTarget이거는 됨
	//png -> GUID_WICPixelFormat32bppPBGRA 이걸 해주는게 wic이다.
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
