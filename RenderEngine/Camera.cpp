#include "Camera.h"
#include "ImGuiRegister.h"

Camera::Camera(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources, float fov) :
	_fov(fov), 
	deviceResources(deviceResources), 
	_speed(50.0f), 
	_position{ Mathf::xVectorZero },
	_front{ 0.0f, 0.0f, -1.0f, 0.0f },
	_up{ Mathf::xVectorZero },
	_mouseSensitivity(50.0f), 
	yaw(270.0f), 
	pitch(0.0f), 
	roll(0.0f), 
	exposure(1.0f), 
	fnear(0.1f), 
	ffar(1000.0f)
{
	ImGui::ContextRegister("Camera", [&]()
	{
		ImGui::DragFloat("Speed", &_speed, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Mouse Sensitivity", &_mouseSensitivity, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Exposure", &exposure, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Near", &fnear, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Far", &ffar, 0.1f, 0.0f, 1000.0f);
		ImGui::DragFloat("Fov", &_fov, 0.1f, 0.0f, 100.0f);
		ImGui::Separator();
		float rot[3] = { yaw, pitch, roll };
		ImGui::DragFloat3("Rotation", rot, 0.1f);
		yaw = rot[0];
		pitch = rot[1];
		roll = rot[2];

		ImGui::Separator();
		Mathf::Vector3 pos = _position;
		ImGui::DragFloat3("Position", &pos.x, 0.1f);
		_position = pos;
	});

}

Mathf::xMatrix Camera::GetViewMatrix() const
{
	Mathf::xVector pos = GetPosition();
	return DirectX::XMMatrixLookAtRH(pos, DirectX::XMVectorAdd(pos, _front), _up);
}

Mathf::xMatrix Camera::GetProjectionMatrix() const
{
	return DirectX::XMMatrixPerspectiveFovRH(
		_fov * 0.01f,
		deviceResources->GetAspectRatio(),
		fnear,
		ffar
	);
}

void Camera::Update(float deltaSeconds)
{
	float speed = _speed * deltaSeconds;

	if (InputManagement->IsKeyPressed('W'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ speed, speed, speed, speed },
			_front,
			_position
		);
	}
	if (InputManagement->IsKeyPressed('S'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ -speed, -speed, -speed, -speed },
			_front,
			_position
		);
	}
	if (InputManagement->IsKeyPressed('A'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ -speed, -speed, -speed, -speed },
			GetRight(),
			_position
		);
	}
	if (InputManagement->IsKeyPressed('D'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ speed, speed, speed, speed },
			GetRight(),
			_position
		);
	}
	if (InputManagement->IsKeyPressed('E'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ speed, speed, speed, speed },
			GetUp(),
			_position
		);
	}
	if (InputManagement->IsKeyPressed('Q'))
	{
		_position = DirectX::XMVectorMultiplyAdd(
			Mathf::xVector{ -speed, -speed, -speed, -speed },
			GetUp(),
			_position
		);
	}
	if (InputManagement->IsMouseButtonDown(MouseKey::MIDDLE))
	{
		float mousesens = _mouseSensitivity * deltaSeconds;
		float2 delta = InputManagement->GetMouseDelta();

		yaw += delta.x * mousesens;
		pitch -= delta.y * mousesens;
	}

	Mathf::Wrap(yaw, 0.0f, 360.0f);
	Mathf::Wrap(pitch, -89.0f, 89.0f);

	float3 front{};

	float cospitch = DirectX::XMScalarCos(DirectX::XMConvertToRadians(pitch));
	front.x = DirectX::XMScalarCos(DirectX::XMConvertToRadians(yaw)) * cospitch;
	front.y = DirectX::XMScalarSin(DirectX::XMConvertToRadians(pitch));
	front.z = DirectX::XMScalarSin(DirectX::XMConvertToRadians(yaw)) * cospitch;
	_front = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));

	Mathf::xVector right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(_front, Mathf::xVectorUp));
	_up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, _front));
}

Mathf::xVector Camera::GetRight()
{
	return DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(_front, _up)
	);
}

Mathf::xVector Camera::GetPosition() const
{
	return _position;
}

Mathf::xVector Camera::GetViewPosition() const
{
	return _front;
}

Mathf::xVector Camera::GetUp() const
{
	return _up;
}

void Camera::SetPosition(DirectX::XMFLOAT3 pos)
{
	_position = DirectX::XMLoadFloat3(&pos);
}

void Camera::SetPosition(float x, float y, float z)
{
	_position = Mathf::xVector{ x, y, z, 0.0f };
}

void Camera::SetRotation(float yaw, float pitch, float roll)
{
	this->yaw = yaw;
	this->pitch = pitch;
	this->roll = roll;
}

Mathf::xMatrix Camera::GetInvViewMatrix() const
{
	XMVECTOR determinant;
	Mathf::xMatrix view = GetViewMatrix();
	Mathf::xMatrix invView = DirectX::XMMatrixInverse(&determinant, view);

	if (XMVectorGetX(determinant) == 0) {
		// 역행렬이 존재하지 않음 -> 로그 출력 또는 예외 처리
		throw std::runtime_error("View matrix is singular, cannot invert.");
	}

	return invView;
}

Mathf::xMatrix Camera::GetInvProjMatrix() const
{
	XMVECTOR determinant;
	Mathf::xMatrix proj = GetProjectionMatrix();
	Mathf::xMatrix invProj = DirectX::XMMatrixInverse(&determinant, proj);

	if (XMVectorGetX(determinant) == 0) {
		// 역행렬이 존재하지 않음
		throw std::runtime_error("Projection matrix is singular, cannot invert.");
	}

	return invProj;
}
