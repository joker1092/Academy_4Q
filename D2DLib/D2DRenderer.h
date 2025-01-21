#pragma once
#include "../Utility_Framework/DeviceResources.h"
#include "D2DObjScene.h"
#include <map>
namespace DirectX11 
{
	class D2DRenderer
	{
	public:
		D2DRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
		~D2DRenderer() = default;
		DeviceResources* GetDeviceResources() { return m_DeviceResources.get(); }
		std::map<std::wstring, ID2D1Bitmap*>& GetD2DImageMap() { return m_D2DImage; }
		HRESULT LoadBitmapFromFile(ID2D1HwndRenderTarget* pRenderTarget, IWICImagingFactory2* pIWICFactory, PCWSTR uri, std::wstring _name, ID2D1Bitmap** ppBitmap);
	private:

		std::vector<D2DObjScene>				m_D2DObjVec;
		ID2D1HwndRenderTarget*					m_D2DRenderTarget;
		std::shared_ptr<DeviceResources>		m_DeviceResources;
		std::map<std::wstring, ID2D1Bitmap*>	m_D2DImage;
	};

}



