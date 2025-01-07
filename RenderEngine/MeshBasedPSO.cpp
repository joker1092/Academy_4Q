#include "MeshBasedPSO.h"
#include "DataSystem.h"

MeshBasedPSO::MeshBasedPSO(StateDevice* deviceResources) : 
	m_device(deviceResources->GetDevice()), 
	m_deviceContext(deviceResources->GetDeviceContext())
{
	m_stateDevice = deviceResources;
	InitializeShaders();
	CreateInputLayout();
	CreateBuffers();
	CreateTextures();
	CreateSamplers();
	CreateComputeResources();
}

void MeshBasedPSO::Prepare(CameraBuffer* camera, SceneBuffer* scene)
{
	m_target->ClearView();
	m_shadowTarget->ClearView();

	m_stateDevice->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DrawCubeMap(camera);

	m_stateDevice->SetRasterizerState(RasterizerType::SolidFrontCull);
	m_stateDevice->SetDepthStencilState(DepthStencilType::Less);

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = m_cameraBuffer.Map(D3D11_MAP::D3D11_MAP_WRITE_DISCARD);
		memcpy(mappedResource.pData, camera, sizeof(CameraBuffer));
		m_cameraBuffer.Unmap();
	}
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = m_sceneBuffer.Map(D3D11_MAP::D3D11_MAP_WRITE_DISCARD);
		memcpy(mappedResource.pData, scene, sizeof(SceneBuffer));
		m_sceneBuffer.Unmap();
	}

	m_deviceContext->IASetInputLayout(m_meshInputLayout.Get());

	CD3D11_VIEWPORT viewport{
		0.f,
		0.f,
		SHADOW_MAP_SIZE,
		SHADOW_MAP_SIZE,
	};

	m_deviceContext->OMSetRenderTargets(0, nullptr, m_shadowDepthStencilView.Get());
	m_deviceContext->RSSetViewports(1, &viewport);

	m_deviceContext->VSSetShader(m_shadowVertexShader.Get(), nullptr, 0);
	m_deviceContext->PSSetShader(m_shadowPixelShader.Get(), nullptr, 0);

	float nearPlane = 5.f, farPlane = 30.f;
	Mathf::xMatrix lightProjection = XMMatrixOrthographicRH(32.f, 32.f, nearPlane, farPlane);

	Mathf::xMatrix lookDir = XMMatrixLookAtRH(
		scene->SunPos,
		{ 0.f, 0.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f, 0.f }
	);

	Mathf::xMatrix lightSpace = XMMatrixMultiplyTranspose(lookDir, lightProjection);
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = m_lightSpaceBuffer.Map(D3D11_MAP::D3D11_MAP_WRITE_DISCARD);
		memcpy(mappedResource.pData, &lightSpace, sizeof(LightSpaceBuffer));
		m_lightSpaceBuffer.Unmap();
	}

	m_deviceContext->VSSetConstantBuffers(0, 1, m_lightSpaceBuffer.GetAddressOf());
}

void MeshBasedPSO::DrawMesh(const Buffer<Index>& indexBuffer, const Buffer<Vertex>& vertexBuffer, const Material* material)
{
	m_deviceContext->PSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = m_materialBuffer.Map(D3D11_MAP::D3D11_MAP_WRITE_DISCARD);
		memcpy(mappedResource.pData, &material->m_Properties, sizeof(MaterialProperties));
		m_materialBuffer.Unmap();
	}

	uint32 srtide = vertexBuffer.Stride();
	const uint32 offset = 0;

	ID3D11ShaderResourceView* srvs[]
	{
		material->m_Textures.baseColor.Get(),
		material->m_Textures.metallic.Get(),
		material->m_Textures.roughness.Get(),
		material->m_Textures.occlusion.Get(),
		material->m_Textures.emissive.Get(),
		material->m_Textures.normal.Get(),
	};

	m_deviceContext->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

	m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &srtide, &offset);
	m_deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->DrawIndexed(indexBuffer.Size(), 0, 0);
}

void MeshBasedPSO::DrawMeshShadows(const Buffer<Index>& indexBuffer, const Buffer<Vertex>& vertexBuffer)
{
	uint32 srtide = vertexBuffer.Stride();
	const UINT offset = 0;

	m_stateDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &srtide, &offset);
	m_stateDevice->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	m_stateDevice->DrawIndexed(indexBuffer.Size(), 0, 0);
}

void MeshBasedPSO::FinishShadows()
{
	m_stateDevice->SetRenderTarget(m_target.get(), m_shadowTarget.get());
	m_stateDevice->SetRasterizerState(RasterizerType::Solid);

	m_stateDevice->GetDeviceContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	m_stateDevice->GetDeviceContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	ID3D11Buffer* VSBuffers[]
	{
		m_cameraBuffer.Get(),
		nullptr,
		m_lightSpaceBuffer.Get(),
	};
	ID3D11Buffer* PSBuffers[]
	{
		m_cameraBuffer.Get(),
		m_sceneBuffer.Get(),
	};

	m_stateDevice->GetDeviceContext()->VSSetConstantBuffers(0, 3, VSBuffers);
	m_stateDevice->GetDeviceContext()->PSSetConstantBuffers(0, 2, PSBuffers);

	ID3D11SamplerState* samplers[4]
	{
		m_anisotropicSampler.Get(),
		m_linearSampler.Get(),
		m_clampSampler.Get(),
		m_IBLSampler.Get()
	};

	m_stateDevice->GetDeviceContext()->PSSetSamplers(0, 4, samplers);

	m_stateDevice->GetDeviceContext()->PSSetShaderResources(6, 1, m_skyBox->material->m_Textures.baseColor.GetAddressOf());
	m_stateDevice->GetDeviceContext()->PSSetShaderResources(7, 1, m_shadowMap.GetAddressOf());
}

void MeshBasedPSO::SetModelConstants(const ModelBuffer* modelBuffer)
{
	m_stateDevice->GetDeviceContext()->VSSetConstantBuffers(1, 1, m_modelBuffer.GetAddressOf());
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = m_modelBuffer.Map(D3D11_MAP::D3D11_MAP_WRITE_DISCARD);
		memcpy(mappedResource.pData, modelBuffer, sizeof(ModelBuffer));
		m_modelBuffer.Unmap();
	}
}

void MeshBasedPSO::Finish()
{
	ID3D11ShaderResourceView* srv[]{ nullptr };
	m_deviceContext->PSSetShaderResources(7, 1, srv);
	m_deviceContext->ClearDepthStencilView(m_shadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_stateDevice->SetRenderTarget();
}

void MeshBasedPSO::CreateCubeMap(const Texture2D& texture)
{
	m_skyBox = std::make_unique<Sphere>(m_device, m_deviceContext, "CubeMap", 30);
	m_skyBox->material->m_Textures.baseColor = texture;
}

void MeshBasedPSO::Resize(uint32 width, uint32 height)
{
	m_target->Resize(width, height);
	m_shadowTarget->Resize(width, height);
	m_blurredShadowMap.Resize(m_device, width, height);
}

void MeshBasedPSO::DrawCubeMap(CameraBuffer* camera)
{
	m_deviceContext->VSSetShader(m_cubeMapVertexShader.Get(), nullptr, 0);
	m_deviceContext->PSSetShader(m_cubeMapPixelShader.Get(), nullptr, 0);

	m_stateDevice->SetRasterizerState(RasterizerType::SolidNoCull);
	m_stateDevice->SetDepthStencilState(DepthStencilType::LessEqual);
	m_stateDevice->SetRenderTarget(m_target.get());

	Mathf::xMatrix mat = DirectX::XMMatrixIdentity();
	Mathf::xMatrix scale = DirectX::XMMatrixScaling(5.f, 5.f, 5.f);
	Mathf::xMatrix translation = DirectX::XMMatrixTranslation(camera->position.x, camera->position.y, camera->position.z);

	mat = DirectX::XMMatrixMultiply(scale, translation);

	m_deviceContext->VSSetConstantBuffers(0, 1, m_mvpBuffer.GetAddressOf());
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = m_mvpBuffer.Map(D3D11_MAP::D3D11_MAP_WRITE_DISCARD);
		memcpy(mappedResource.pData, &mat, sizeof(MVPBuffer));
		m_mvpBuffer.Unmap();
	}

	m_deviceContext->PSSetSamplers(0, 1, m_cubeMapSampler.GetAddressOf());
	m_deviceContext->PSSetShaderResources(0, 1, m_skyBox->material->m_Textures.baseColor.GetAddressOf());

	uint32 srtide = m_skyBox->bvertex.Stride();
	const UINT offset = 0;

	m_deviceContext->IASetInputLayout(m_cubeMapInputLayout.Get());

	m_deviceContext->IASetVertexBuffers(0, 1, m_skyBox->bvertex.GetAddressOf(), &srtide, &offset);
	m_deviceContext->IASetIndexBuffer(m_skyBox->bindex.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0);

	m_deviceContext->DrawIndexed(m_skyBox->indices.size(), 0, 0);
}

void MeshBasedPSO::CreateInputLayout()
{
	{
		// Mesh
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",   1, DXGI_FORMAT_R32G32B32_FLOAT,	0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,     0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DirectX11::ThrowIfFailed(
			m_stateDevice->GetDevice()->CreateInputLayout(
				layout,
				ARRAYSIZE(layout),
				m_vertexShader.GetBufferPointer(),
				m_vertexShader.GetBufferSize(),
				&m_meshInputLayout
			)
		);

		DirectX::SetName(m_meshInputLayout.Get(), "Mesh Input Layout");

	}

	{
		// Cubemap
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,     0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DirectX11::ThrowIfFailed(
			m_stateDevice->GetDevice()->CreateInputLayout(
				layout,
				ARRAYSIZE(layout),
				m_cubeMapVertexShader.GetBufferPointer(),
				m_cubeMapVertexShader.GetBufferSize(),
				&m_cubeMapInputLayout
			)
		);

		DirectX::SetName(m_cubeMapInputLayout.Get(), "CubeMap Input Layout");
	}
}

void MeshBasedPSO::InitializeShaders()
{
	m_vertexShader = DataSystem::GetInstance()->m_VertexShaders["mesh"];
	m_pixelShader = DataSystem::GetInstance()->m_PixelShaders["mesh"];

	m_cubeMapVertexShader = DataSystem::GetInstance()->m_VertexShaders["cubemap"];
	m_cubeMapPixelShader = DataSystem::GetInstance()->m_PixelShaders["cubemap"];

	m_shadowVertexShader = DataSystem::GetInstance()->m_VertexShaders["shadow"];
	m_shadowPixelShader = DataSystem::GetInstance()->m_PixelShaders["shadow"];
}

void MeshBasedPSO::CreateBuffers()
{
	m_mvpBuffer = Buffer<MVPBuffer>{
		m_stateDevice->GetDevice(),
		m_stateDevice->GetDeviceContext(),
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	};

	m_modelBuffer = Buffer<ModelBuffer>{
		m_stateDevice->GetDevice(),
		m_stateDevice->GetDeviceContext(),
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	};

	m_materialBuffer = Buffer<MaterialProperties>{
		m_stateDevice->GetDevice(),
		m_stateDevice->GetDeviceContext(),
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	};

	m_cameraBuffer = Buffer<CameraBuffer>{
		m_stateDevice->GetDevice(),
		m_stateDevice->GetDeviceContext(),
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	};

	m_lightSpaceBuffer = Buffer<LightSpaceBuffer>{
		m_stateDevice->GetDevice(),
		m_stateDevice->GetDeviceContext(),
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	};

	m_sceneBuffer = Buffer<SceneBuffer>{
		m_stateDevice->GetDevice(),
		m_stateDevice->GetDeviceContext(),
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	};
}

void MeshBasedPSO::CreateSamplers()
{
	m_anisotropicSampler = Sampler{
		m_stateDevice->GetDevice(),
		D3D11_FILTER::D3D11_FILTER_ANISOTROPIC
	};

	m_linearSampler = Sampler{
		m_stateDevice->GetDevice(),
		D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR
	};

	Mathf::Color4 borderColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	m_clampSampler = Sampler{
		m_stateDevice->GetDevice(),
		D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER,
		0.0f,
		&borderColor
	};

	m_cubeMapSampler = Sampler{
		m_stateDevice->GetDevice(),
		D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP
	};

	m_IBLSampler = Sampler{
		m_stateDevice->GetDevice(),
		D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP,
		1.f
	};
}

void MeshBasedPSO::CreateTextures()
{
	{
		DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		CD3D11_TEXTURE2D_DESC desc{
			format,
			SHADOW_MAP_SIZE,
			SHADOW_MAP_SIZE,
			1,
			0,
			D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE
		};

		ComPtr<ID3D11Texture2D> texture{};
		DirectX11::ThrowIfFailed(
			m_stateDevice->GetDevice()->CreateTexture2D(&desc, nullptr, &texture)
		);

		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{
			D3D11_SRV_DIMENSION_TEXTURE2D,
			DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT,
			0,
			1
		};

		ComPtr<ID3D11ShaderResourceView> srv{};
		DirectX11::ThrowIfFailed(
			m_stateDevice->GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, &srv)
		);

		m_shadowMap = Texture2D{
			texture.Get(),
			srv.Get()
		};
		m_shadowMap.SetName("Shadow Map");

		CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{
			D3D11_DSV_DIMENSION_TEXTURE2D,
			DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT
		};

		DirectX11::ThrowIfFailed(
			m_stateDevice->GetDevice()->CreateDepthStencilView(texture.Get(), &dsvDesc, &m_shadowDepthStencilView)
		);
	}

	{
		DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
		CD3D11_TEXTURE2D_DESC desc{
			format,
			SHADOW_MAP_SIZE,
			SHADOW_MAP_SIZE,
			1,
			1,
			D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE
		};

		ComPtr<ID3D11Texture2D> texture{};
		DirectX11::ThrowIfFailed(
			m_stateDevice->GetDevice()->CreateTexture2D(&desc, nullptr, &texture)
		);

		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{
			D3D11_SRV_DIMENSION_TEXTURE2D,
			format,
			0,
			1
		};

		ComPtr<ID3D11ShaderResourceView> srv{};
		DirectX11::ThrowIfFailed(
			m_stateDevice->GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, &srv)
		);

		m_blurredShadowMap = Texture2D{
			texture.Get(),
			srv.Get()
		};
		m_blurredShadowMap.SetName("Blurred Shadow Map");
	}

	{
		m_target = std::make_unique<RenderTarget>(
			m_stateDevice->GetDevice(),
			DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
			m_stateDevice->GetBackBufferWidth(),
			m_stateDevice->GetBackBufferHeight()
		);
	}

	{
		m_shadowTarget = std::make_unique<RenderTarget>(
			m_stateDevice->GetDevice(),
			DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT,
		m_stateDevice->GetBackBufferWidth(),
			m_stateDevice->GetBackBufferHeight()
		);

		m_shadowTarget->color = Mathf::xVColor4{ 1.f, 1.f, 1.f, 1.f };
	}
}

void MeshBasedPSO::CreateComputeResources()
{
}

void MeshBasedPSO::Blend()
{
}
