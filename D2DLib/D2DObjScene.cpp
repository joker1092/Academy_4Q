#include "pch.h"
#include "D2DObjScene.h"

D2DObjScene::D2DObjScene()
{
	m_RelativeTransform = m_WorldTransform = D2D1::Matrix3x2F::Identity();
}

D2DObjScene::~D2DObjScene()
{

}

Vector2 D2DObjScene::GetWorldLocation() const
{
	
	Vector2 out;
	out.x = m_WorldTransform._31;
	out.y = m_WorldTransform._32;
	return out;
	
}

void D2DObjScene::UpdateTransform()
{
	m_RelativeTransform = D2D1::Matrix3x2F::Scale(D2D1::SizeF(m_RelativeScale.x, m_RelativeScale.y)) *
		D2D1::Matrix3x2F::Rotation(m_RelativeRotation) *
		D2D1::Matrix3x2F::Translation(m_RelativePos.x, m_RelativePos.y);

	if (m_pParentObjScene) {
		m_WorldTransform = m_RelativeTransform * m_pParentObjScene->m_WorldTransform;
	}
	else {
		m_WorldTransform = m_RelativeTransform;
	}
}

void D2DObjScene::SetRelativeRoation(float _Rotation)
{
	m_RelativeRotation = _Rotation;
	m_RelativeRotation = fmodf(m_RelativeRotation, 360.0f);
}

void D2DObjScene::AddRelativeRoation(float _Rotation)
{
	m_RelativeRotation += _Rotation;
	m_RelativeRotation = fmodf(m_RelativeRotation, 360.0f);
}

void D2DObjScene::SetRelativePos(const Vector2& _Location)
{
	m_RelativePos = _Location;
}

void D2DObjScene::AddRelativePos(float x, float y)
{
	m_RelativePos = { m_RelativePos.x + x,m_RelativePos.y + y };
}

void D2DObjScene::SetRelativeScale(float x, float y)
{
	m_RelativeScale = Vector2(x, y);
}

void D2DObjScene::Update(float deltaTime)
{
	UpdateTransform();
}
