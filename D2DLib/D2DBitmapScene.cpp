#include "pch.h"
#include "D2DBitmapScene.h"
#include "D2DRenderer.h"
D2DBitmapScene::D2DBitmapScene()
{
}

D2DBitmapScene::D2DBitmapScene(std::wstring strFilePath)
	: m_strBitmapFilePath(strFilePath)
{

}

D2DBitmapScene::~D2DBitmapScene()
{
}

bool D2DBitmapScene::LoadD2DBitmap(const std::wstring strFilePath,DirectX11::D2DRenderer& _Render)
{

	m_strBitmapFilePath = strFilePath;
	auto& D2DMap = _Render.GetD2DImageMap();
	auto iter = D2DMap.find(strFilePath);
	if (iter != D2DMap.end()) {
		m_pBitmap = iter->second;
		return true;
	}
	
	DirectX11::DeviceResources* DeviceResosu = _Render.GetDeviceResources();
	auto* WicFac = DeviceResosu->GetWicImagingFactory();

	
}
