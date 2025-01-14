#pragma once
#include "DataSystem.h"
#include "PSO.h"
#include "LogicalDevice.h"
#include "Shader.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Camera.h"
#include "Primitives.h"

constexpr uint32 SHADOWMAP_RES = 8192;

// Mesh based Pipeline State object
class MPSO : public PSO
{
public:
	MPSO(LogicalDevice* device);
	~MPSO();

	//void Bind();
	void Prepare(CameraBuffer* cameraBuffer, SceneBuffer* sceneBuffer);
	void DrawMesh(const Buffer<Index>& indices, const Buffer<Vertex>& vertices, const std::shared_ptr<Material>& material);
	void DrawMeshShadows(const Buffer<Index>& indices, const Buffer<Vertex>& vertices);

	void FinishShadows();
	
	void SetModelConstants(const ModelBuffer* modelbuffer);
	void Finish(BOOL blurShadows, FxaaBuffer* fxaabuffer);
	void CreateCubeMap(const Texture2D& texture);
	void CreateIBL(const Texture2D& diffuse, const Texture2D& specular, const Texture2D& brdf);
	inline void Resize(UINT width, UINT height) {
		_target->Resize(width, height);
		_shadowtarget->Resize(width, height);
		_blurredShadows.Resize(width, height);
		CreateComputeResources();
	}


private:
	void DrawCubemap(CameraBuffer* cameraBuffer);
	void CreateInputLayout();
	void InitializeShaders();
	void CreateBuffers();
	void CreateSamplers();
	void CreateTextures();
	void CreateComputeResources();

	void Blend(BOOL blurShadows);
	void FXAA(FxaaBuffer* fxaabuffer);

private:
	LogicalDevice* _device;
	ComPtr<ID3D11InputLayout> _meshInputLayout;
	ComPtr<ID3D11InputLayout> _cubemapInputLayout;


	VertexShader	_vs;
	PixelShader		_ps;

	VertexShader	_cubemapvs;
	PixelShader		_cubemapps;

	VertexShader	_shadowvs;
	PixelShader		_shadowps;

	ComputeShader	_shadowblurcsX;
	ComputeShader	_shadowblurcsY;
	ComputeShader	_blendcs;
	ComputeShader	_fxaacs;

	Buffer<MaterialProperties>	_materialbuffer;
	Buffer<ModelBuffer>		_modelbuffer;
	Buffer<LightSpaceBuffer>_lightspacebuffer;
	Buffer<CameraBuffer>	_camerabuffer;
	Buffer<SceneBuffer>		_scenebuffer;
	Buffer<FxaaBuffer>		_fxaabuffer;

	Buffer<MVPBuffer>		_mvpbuffer;

	Sampler			_anisotropicSampler;
	Sampler			_linearSampler;
	Sampler			_cubemapSampler;
	Sampler			_iblSampler;
	Sampler			_clampSampler;

	// Shadow Gaussian filter
	ComPtr<ID3D11Buffer>					_filterbuffer;
	ComPtr<ID3D11ShaderResourceView>		_filterWeightsSRV;
	ComPtr<ID3D11Texture2D>					_filterOutputA;
	ComPtr<ID3D11Texture2D>					_filterOutputB;
	ComPtr<ID3D11UnorderedAccessView>		_filterOutputAUAV;
	ComPtr<ID3D11UnorderedAccessView>		_filterOutputBUAV;
	ComPtr<ID3D11ShaderResourceView>		_filterOutputASRV;
	ComPtr<ID3D11ShaderResourceView>		_filterOutputBSRV;
	Buffer<BlurParameters>									_blurbuffer;


	// Blending 
	ComPtr<ID3D11Texture2D>					_blendOutput;
	ComPtr<ID3D11UnorderedAccessView>		_blendOutputUAV;
	ComPtr<ID3D11ShaderResourceView>		_blendOutputSRV;

	// FXAA
	ComPtr<ID3D11Texture2D>					_fxaaOutput;
	ComPtr<ID3D11UnorderedAccessView>		_fxaaOutputUAV;

	Texture2D		_shadowmap;				// Rendered Shadowmap for blending
	Texture2D		_blurredShadows;
	std::unique_ptr<RenderTarget>	_target;
	std::unique_ptr<RenderTarget>	_shadowtarget;
	ComPtr<ID3D11DepthStencilView>	_shadowmapDsv;
	std::unique_ptr<Cube>	_skybox;

	Texture2D IBLDiffuse{};
	Texture2D IBLSpecular{};
	Texture2D IBLBrdf{};
};
