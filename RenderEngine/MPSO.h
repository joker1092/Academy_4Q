#pragma once
#include "DataSystem.h"
#include "PSO.h"
#include "LogicalDevice.h"
#include "Shader.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Camera.h"
#include "Primitives.h"
#include "InstancedBillboard.h"

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
	void DrawMesh(const Buffer<Index>& indices, const Buffer<AnimVertex>& vertices, const std::shared_ptr<Material>& material);
	void DrawOutline(const Buffer<Index>& indices, const Buffer<Vertex>& vertices, const OutlineProperties& _outline);
	void DrawOutline(const Buffer<Index>& indices, const Buffer<AnimVertex>& vertices, const OutlineProperties& _outline);
	void DrawDebugBox(const Buffer<UINT16>& indices, const Buffer<DirectX::VertexPositionNormalTexture>& vertices, const std::shared_ptr<Material>& material);
	void DrawMeshShadows(const Buffer<Index>& indices, const Buffer<Vertex>& vertices);
	void DrawMeshShadows(const Buffer<Index>& indices, const Buffer<AnimVertex>& vertices);
	void DrawBillboard(InstancedBillboard& billboard);

	void SetMeshEditorTarget();
	const std::unique_ptr<RenderTarget>& GetMeshEditorTarget() const { return _meshEditTarget; }
	void EndMeshEditorTarget();
	void FinishShadows();
	void SetAnimeShader();
	void SetAnimeShadowsShader();
	void SetWireframeShader();
	void SetOutlineShader();
	void SetOutlineAnimShader();
	void SetBillboardShader(CameraBuffer* cameraBuffer);
	void FinishBillboardsShader();

	void SetBlendState();
	void FinishBlendState();
	
	void SetWireframeConstants(const ModelBuffer* modelbuffer);
	void SetAnimeConstants(const JointBuffer* modelbuffer);
	void SetModelConstants(const ModelBuffer* modelbuffer);
	void SetAnimeOutlineConstants(const JointBuffer* outlinebuffer);
	void Finish(BOOL blurShadows, FxaaBuffer* fxaabuffer);
	void CreateCubeMap(const Texture2D& texture);
	void CreateIBL(const Texture2D& diffuse, const Texture2D& specular, const Texture2D& brdf);
	inline void Resize(UINT width, UINT height) 
	{
		_target->Resize(width, height);
		_shadowtarget->Resize(width, height);
		_blurredShadows.Resize(width, height);
		CreateComputeResources();
	}


private:
	void DrawCubemap(CameraBuffer* cameraBuffer);
	void CreateInputLayout();
	void InitializeShaders();
	void InitializeBlendState();
	void CreateBuffers();
	void CreateSamplers();
	void CreateTextures();
	void CreateComputeResources();
	void GenerateNeutralLUT();

	void Blend(BOOL blurShadows);
	void FXAA(FxaaBuffer* fxaabuffer);
	void ColorGrading();

private:
	LogicalDevice* _device;
	ComPtr<ID3D11InputLayout> _meshInputLayout;
	ComPtr<ID3D11InputLayout> _animInputLayout;
	ComPtr<ID3D11InputLayout> _cubemapInputLayout;
	ComPtr<ID3D11InputLayout> _outlineInputLayout;
	ComPtr<ID3D11InputLayout> _billboardInputLayout;
	ComPtr<ID3D11InputLayout> _debugBoxInputLayout;
	//--- mesh shaders
	VertexShader	_vs;
	PixelShader		_ps;
	//--- wireframe shaders
	VertexShader	_wireframevs;
	PixelShader		_wireframeps;
	//--- animMesh shaders
	VertexShader    _animvs;
	VertexShader	_shadowmapvs;
	//--- cubemap shaders
	VertexShader	_cubemapvs;
	PixelShader		_cubemapps;
	//--- shadow shaders
	VertexShader	_shadowvs;
	PixelShader		_shadowps;
	//--- outline shaders
	VertexShader    _outlinevs;
	VertexShader    _outlineavs;
	PixelShader		_outlineps;
	//--- billboard shaders
	VertexShader	_billboardvs;
	GeometryShader	_billboardgs;
	PixelShader		_billboardps;
	//--- compute shaders
	ComputeShader	_shadowblurcsX;
	ComputeShader	_shadowblurcsY;
	ComputeShader	_blendcs;
	ComputeShader	_fxaacs;
	ComputeShader   _laplacianCS;

	Buffer<MaterialProperties>	_materialbuffer;
	Buffer<JointBuffer>			_jointbuffer;
	Buffer<ModelBuffer>			_modelbuffer;
	Buffer<LightSpaceBuffer>	_lightspacebuffer;
	Buffer<CameraBuffer>		_camerabuffer;
	Buffer<SceneBuffer>			_scenebuffer;
	Buffer<FxaaBuffer>			_fxaabuffer;
	Buffer<OutlineProperties>	_outlinebuffer;

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
	Buffer<BlurParameters>					_blurbuffer;

	ComPtr<ID3D11Texture2D>					_laplacianOutput;
	ComPtr<ID3D11UnorderedAccessView>       _laplacianOutputUAV;
	ComPtr<ID3D11ShaderResourceView>		_laplacianOutputSRV;
	Buffer<lapacianFilterProperties>		_laplacianBuffer;

	// Blending 
	ComPtr<ID3D11Texture2D>					_blendOutput;
	ComPtr<ID3D11UnorderedAccessView>		_blendOutputUAV;
	ComPtr<ID3D11ShaderResourceView>		_blendOutputSRV;

	// FXAA
	ComPtr<ID3D11Texture2D>					_fxaaOutput;
	ComPtr<ID3D11UnorderedAccessView>		_fxaaOutputUAV;

	// Color Grading
	ComPtr<ID3D11Texture2D>					_colorGradingOutput;
	ComPtr<ID3D11ShaderResourceView>		_colorGradingOutputSRV;
	ComPtr<ID3D11UnorderedAccessView>		_colorGradingOutputUAV;
	ComPtr<ID3D11Texture3D>					_neutralLUT;

	ComPtr<ID3D11BlendState> 				_blendState;

	Texture2D						_shadowmap;	// Rendered Shadowmap for blending
	Texture2D						_blurredShadows;
	std::unique_ptr<RenderTarget>	_target;
	std::unique_ptr<RenderTarget>	_shadowtarget;
	std::unique_ptr<RenderTarget>   _outlineTarget; //maybe not used
	std::unique_ptr<RenderTarget>	_meshEditTarget;
	ComPtr<ID3D11DepthStencilView>	_shadowmapDsv;
	std::unique_ptr<Cube>	_skybox;

	Texture2D IBLDiffuse{};
	Texture2D IBLSpecular{};
	Texture2D IBLBrdf{};

	CameraBuffer* _cameraBuffer;
};
