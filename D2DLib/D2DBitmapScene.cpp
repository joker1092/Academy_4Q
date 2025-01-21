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

void D2DBitmapScene::LoadD2DBitmap(const std::wstring strFilePath,const DirectX11::D2DLoader _Loader)
{

	m_strBitmapFilePath = strFilePath;

	
}
