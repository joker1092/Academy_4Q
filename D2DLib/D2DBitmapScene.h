#pragma once
#include "D2DRenderScene.h"
namespace DirectX11
{
	class D2DRenderer;
}
class D2DBitmapScene : public D2DRenderScene
{
	

	std::wstring m_strBitmapFilePath;
	ID2D1Bitmap* m_pBitmap = nullptr;
	D2D_MATRIX_3X2_F BitmapInverseTransform; // �ִϸ��̼� ������ �߾� ���ߴ� ���(���� ����)
	D2D_MATRIX_3X2_F BitmapTransform; // �ڽ� �ݶ��̴� ������ �߾� ���ߴ� ���(���� ����)

public:
	D2DBitmapScene();
	D2DBitmapScene(std::wstring strFilePath);
	virtual ~D2DBitmapScene();
	bool LoadD2DBitmap(const std::wstring strFilePath, DirectX11::D2DRenderer& _Render);
	
};

