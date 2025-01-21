#pragma once
#include "D2DRenderScene.h"

class D2DBitmapScene : public D2DRenderScene
{
	

	std::wstring m_strBitmapFilePath;
	ID2D1Bitmap* m_pBitmap = nullptr;
	D2D_MATRIX_3X2_F BitmapInverseTransform; // 애니메이션 프레임 중앙 맞추는 행렬(내가 넣음)
	D2D_MATRIX_3X2_F BitmapTransform; // 박스 콜라이더 프레임 중앙 맞추는 행렬(내가 넣음)

public:
	D2DBitmapScene();
	D2DBitmapScene(std::wstring strFilePath);
	virtual ~D2DBitmapScene();
	void LoadD2DBitmap(const std::wstring strFilePath, const DirectX11::D2DLoader _Loader);
};

