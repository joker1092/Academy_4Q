#pragma once
#include "../Utility_Framework/Core.Mathf.h"

using namespace Mathf;
class D2DObjScene
{
private:
	D2DObjScene*		m_pParentObjScene = nullptr;
	Vector2				m_RelativeScale = { 1.0f,1.0f };
	float				m_RelativeRotation = 0.f;
	Vector2				m_RelativePos = { 0.f,0.f };
	D2D_MATRIX_3X2_F	m_RelativeTransform;
	D2D_MATRIX_3X2_F	m_WorldTransform;


public:
	D2DObjScene();

	Vector2 GetWorldLocation();
	void UpdateTransform();

	void SetRelativeRoation(float _Rotation);
	void AddRelativeRoation(float _Rotation);
	float GetRelativeRotation() { return m_RelativeRotation; }

	void SetRelativePos(const Vector2& _Location);
	void AddRelativePos(float x, float y);
	Vector2 GetRelativeLocation() { return m_RelativePos; }

	void SetRelativeScale(float x, float y);
	Vector2 GetRelativeScale(){ return m_RelativeScale; }
	

};

