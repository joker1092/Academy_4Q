#pragma once
#include "Core.Minimal.h"
#include "IPipelineStateObject.h"
#include "StateDevice.h"
#include "Shader.h"
#include "Sampler.h"
#include "Primitives.h"

constexpr uint32 SHADOW_MAP_SIZE = 4096;

class MeshBasedPSO : public IPipelineStateObject
{
public:
    MeshBasedPSO(StateDevice* deviceResources);
    ~MeshBasedPSO() override = default;

	void Prepare(CameraBuffer* camera, SceneBuffer* scene);
	void DrawMesh(const Buffer<Index>& indexBuffer, const Buffer<Vertex>& vertexBuffer, const Material* material);
	void DrawMeshShadows(const Buffer<Index>& indexBuffer, const Buffer<Vertex>& vertexBuffer);

    void FinishShadows();

	void SetModelConstants(const ModelBuffer* modelBuffer);
	void Finish();
	void CreateCubeMap(const Texture2D& texture);
	void Resize(uint32 width, uint32 height);

private:
	void DrawCubeMap(CameraBuffer* camera);
	void CreateInputLayout();
	void InitializeShaders();
	void CreateBuffers();
	void CreateSamplers();
	void CreateTextures();
	void CreateComputeResources();
	void Blend();

private:
	ID3D11Device* m_device{};
	ID3D11DeviceContext* m_deviceContext{};

	StateDevice* m_stateDevice{};
	ComPtr<ID3D11InputLayout> m_meshInputLayout{};
	ComPtr<ID3D11InputLayout> m_cubeMapInputLayout{};

	VertexShader m_vertexShader{};
	PixelShader m_pixelShader{};

	VertexShader m_cubeMapVertexShader{};
	PixelShader m_cubeMapPixelShader{};

	VertexShader m_shadowVertexShader{};
	PixelShader m_shadowPixelShader{};

	ComputeShader m_blendComputeShader{};

	Buffer<MaterialProperties> m_materialBuffer{};
	Buffer<ModelBuffer> m_modelBuffer{};
	Buffer<LightSpaceBuffer> m_lightSpaceBuffer{};
	Buffer<CameraBuffer> m_cameraBuffer{};
	Buffer<SceneBuffer> m_sceneBuffer{};

	Buffer<MVPBuffer> m_mvpBuffer{};

	Sampler m_anisotropicSampler{};
	Sampler m_linearSampler{};
	Sampler m_cubeMapSampler{};
	Sampler m_IBLSampler{};
	Sampler m_clampSampler{};

	ComPtr<ID3D11Texture2D> m_blendOutput{};
	ComPtr<ID3D11ShaderResourceView> m_blendOutputSRV{};
	ComPtr<ID3D11UnorderedAccessView> m_blendOutputUAV{};

	Texture2D m_shadowMap{};
	Texture2D m_blurredShadowMap{};
	std::unique_ptr<RenderTarget> m_target{};
	std::unique_ptr<RenderTarget> m_shadowTarget{};
	ComPtr<ID3D11DepthStencilView> m_shadowDepthStencilView{};
	std::unique_ptr<Sphere> m_skyBox{};
};
