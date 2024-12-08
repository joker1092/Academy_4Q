#include "Dx11Main.h"

DirectX11::Dx11Main::Dx11Main(const std::shared_ptr<DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources)
{
	m_deviceResources->RegisterDeviceNotify(this);

	//�Ʒ� ������	�ʱ�ȭ �ڵ带 ���⿡ �߰��մϴ�.
}

DirectX11::Dx11Main::~Dx11Main()
{
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

void DirectX11::Dx11Main::CreateWindowSizeDependentResources()
{
	//�������� â ũ�⿡ ���� ���ҽ��� �ٽ� ����� �ڵ带 ���⿡ �߰��մϴ�.
}

void DirectX11::Dx11Main::Update()
{
	//�������� ������Ʈ �ڵ带 ���⿡ �߰��մϴ�.
	m_timeSystem.Tick([&]
	{
		//�������� ������Ʈ �ڵ带 ���⿡ �߰��մϴ�.
	});
}

bool DirectX11::Dx11Main::Render()
{
	// ó�� ������Ʈ�ϱ� ���� �ƹ� �͵� ���������� ������.
	if (m_timeSystem.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// ������ ����� ȭ�鿡 ���� �ٽ� �����մϴ�.
	ID3D11RenderTargetView* const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());


	// �� ���� �� ���� ���ٽ� �並 ����ϴ�.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ��� ��ü�� �������մϴ�.

	return true;
}
// �������� �ʿ��� ����̽� ���ҽ��� �������� �˸��ϴ�.
void DirectX11::Dx11Main::OnDeviceLost()
{
}

// ����̽� ���ҽ��� ���� �ٽ� ������� �� ������ �������� �˸��ϴ�.
void DirectX11::Dx11Main::OnDeviceRestored()
{
	CreateWindowSizeDependentResources();
}
