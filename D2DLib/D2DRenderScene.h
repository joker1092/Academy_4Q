#pragma once
#include "D2DObjScene.h"

class D2DRenderScene : public D2DObjScene
{
protected:
	int m_ZOrder;		// 그리는 순서  
	bool m_Visible = true;		// 그리려는지

	D2D1_MATRIX_3X2_F	m_RenderTransform;	// x축반전이나 위아래 반전
public:
	void SetZOrder(int zOrder) { m_ZOrder = zOrder; }
	int GetZOrder() const { return m_ZOrder; }
	void SetVisible(bool visible) { m_Visible = visible; }

	static bool CompareRenderOrder(D2DRenderScene* a, D2DRenderScene* b)
	{
		return a->m_ZOrder < b->m_ZOrder;
	}
};

