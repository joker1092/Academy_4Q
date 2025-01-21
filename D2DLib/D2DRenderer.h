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
		static D2DRenderer& GetInstance() {
			static D2DRenderer one;
			if (shouldInit) {

			}
			return one;
		}

		D2DRenderer(const D2DRenderer& _ref) = delete;
		D2DRenderer& operator=(const D2DRenderer& _ref) = delete;

		void InitD2DRender(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
	private:
		D2DRenderer();
		static bool shouldInit;
		std::vector<D2DObjScene>				m_D2DObjVec;
		std::shared_ptr<DeviceResources>		m_DeviceResources;
		
	};

	class D2DLoader {
	public:
		static D2DLoader& GetInstance() {
			static D2DLoader one;
			return one;
		}

		D2DLoader(const D2DLoader& _ref) = delete;
		D2DLoader& operator=(const D2DLoader& _ref) = delete;
		HRESULT LoadBitmapFromFile(
			ID2D1RenderTarget* pRenderTarget,
			IWICImagingFactory2* pIWICFactory,
			PCWSTR uri,
			std::wstring _name,
			ID2D1Bitmap** ppBitmap
		);

		void InitD2DLoader(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources);
	private:
		static bool shouldInit;
		D2DLoader();
		std::shared_ptr<DeviceResources>		m_DeviceResources;
		std::map<std::wstring, ID2D1Bitmap*>	m_D2DImage;
	};
}



