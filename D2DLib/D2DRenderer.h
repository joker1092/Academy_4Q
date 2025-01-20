#pragma once
#include "../Utility_Framework/DeviceResources.h"
#include "D2DObjScene.h"
#include <memory>
#include <vector>
#include <map>
namespace DirectX11 {


	class D2DRenderer
	{
	public:
		D2DRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);

	private:
		std::vector<D2DObjScene>			m_D2DObjVec;
		std::shared_ptr<DeviceResources>					m_DeviceResources;

	};

	class D2DImageLoader {
	public:
		HRESULT LoadBitmapFormFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory2* pIWICFactory,
			PCWSTR uri, std::wstring _name, ID2D1Bitmap** ppBitmap);

	private:
		std::map<std::wstring, ID2D1Bitmap*> m_LoadedImage;
	};
}



