#include "pch.h"
#include "D2DRenderer.h"

DirectX11::D2DRenderer::D2DRenderer(const std::shared_ptr<DirectX11::DeviceResources>& deviceResources)
: m_DeviceResources(deviceResources)
{

}

HRESULT DirectX11::D2DImageLoader::LoadBitmapFormFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory2* pIWICFactory, PCWSTR uri, std::wstring _name, ID2D1Bitmap** ppBitmap)
{
    return E_NOTIMPL;
}
