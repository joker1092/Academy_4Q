#pragma once
#include "Core.Minimal.h"
#include "DXStates.h"

class RenderTarget
{
public:
	RenderTarget(ComPtr<ID3D11Texture2D>& texture, DXGI_FORMAT _format, UINT _width, UINT _height);
	RenderTarget(DXGI_FORMAT _format, UINT _width, UINT _height);
	~RenderTarget();

	void Resize(UINT _width, UINT _height);

	void inline ClearView()
    {
		DX::States::Context->ClearRenderTargetView(_rtv.Get(), color);
		DX::States::Context->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	inline ID3D11RenderTargetView* const* GetRTVAddress() const { return _rtv.GetAddressOf(); }
	inline ID3D11RenderTargetView* GetRTV() const { return _rtv.Get(); }
	inline ID3D11DepthStencilView* const* GetDSVAddress() const { return _dsv.GetAddressOf(); }
	inline ID3D11DepthStencilView* GetDSV() const { return _dsv.Get(); }
	inline ID3D11ShaderResourceView* GetSRV() const { return _srv.Get(); }

	inline ID3D11Texture2D* GetTexture()
	{
		return _texture.Get();
	}
	inline ID3D11Texture2D* GetStencil()
	{
		return _stencil.Get();
	}

	DirectX::XMVECTORF32 color;
	UINT width;
	UINT height;
	DXGI_FORMAT format;

private:
	void CreateTexture();
	void CreateTargets();
	void CreateShaderResourceView();
	void DestroyTargets();

	BOOL _bsrv;
	ComPtr<ID3D11Texture2D>				_texture;
	ComPtr<ID3D11Texture2D>				_stencil;

	ComPtr<ID3D11RenderTargetView>		_rtv;
	ComPtr<ID3D11DepthStencilView>		_dsv;
	ComPtr<ID3D11ShaderResourceView>	_srv;

};
