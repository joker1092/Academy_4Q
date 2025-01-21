#pragma once
#include "D2DObjScene.h"

class D2DRenderScene : public D2DObjScene
{
protected:
	int m_ZOrder;		// �׸��� ����  
	bool m_Visible = true;		// �׸�������

	D2D1_MATRIX_3X2_F	m_RenderTransform;	// x������̳� ���Ʒ� ����
public:
	void SetZOrder(int zOrder) { m_ZOrder = zOrder; }
	int GetZOrder() const { return m_ZOrder; }
	void SetVisible(bool visible) { m_Visible = visible; }

	static bool CompareRenderOrder(D2DRenderScene* a, D2DRenderScene* b)
	{
		return a->m_ZOrder < b->m_ZOrder;
	}
};

