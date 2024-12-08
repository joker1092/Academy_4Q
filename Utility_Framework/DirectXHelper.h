#pragma once
#include "Core.Definition.h"
#include <ppltasks.h>
#include <DirectXTex.h>

#define SHADER_MACRO_DEFINITION(name) D3D_SHADER_MACRO name[] =
constexpr D3D_SHADER_MACRO ShaderMacroTerminator = { nullptr, nullptr };

namespace DirectX11
{
	class ComException : public std::exception
	{
	public:
		ComException() = default;
		const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", result);
			return s_str;
		}

		static std::exception CreateException(HRESULT hr)
		{
			return ComException(hr);
		}

	private:
		ComException(HRESULT hr) : result(hr) {}

	private:
		HRESULT result;
	};

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw DirectX11::ComException::CreateException(hr);
		}
	}

	inline Concurrency::task<std::vector<byte>> ReadDataAsync(const file::path& fileName)
	{
		using namespace Concurrency;

		auto folder = file::current_path();
		auto path = folder / fileName;

		return create_task([path]() -> std::vector<byte> {
			// std::ifstream을 사용해 파일 열기
			std::ifstream file(path, std::ios::binary | std::ios::ate);
			if (!file.is_open())
			{
				throw std::runtime_error("Failed to open the file.");
			}

			// 파일 크기를 가져오기
			std::streamsize fileSize = file.tellg();
			file.seekg(0, std::ios::beg);

			// 버퍼 할당 및 파일 데이터 읽기
			std::vector<byte> buffer(static_cast<size_t>(fileSize));
			if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
			{
				throw std::runtime_error("Failed to read the file.");
			}

			return buffer;
			});
	}

	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f);
	}
	//Map/Unmap을 사용하여 리소스를 스코프 영역에서 매핑/언매핑하는 클래스
	class SharedMap final
	{
	public:
		SharedMap(ID3D11DeviceContext* pDeviceContext, ID3D11Resource* pResource, D3D11_MAPPED_SUBRESOURCE* pMappedResource, uint32 subresource = 0, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, uint32 mapFlags = 0)
			: m_pDeviceContext(pDeviceContext), m_pResource(pResource), m_Subresource(subresource)
		{
			m_pDeviceContext->Map(m_pResource, m_Subresource, mapType, mapFlags, pMappedResource);
		}
		~SharedMap()
		{
			m_pDeviceContext->Unmap(m_pResource, m_Subresource);
		}

	private:
		ID3D11DeviceContext* m_pDeviceContext;
		ID3D11Resource* m_pResource;
		uint32 m_Subresource;
	};

	//디버그 레이어를 사용할 수 있는지 확인하는 함수
#if defined(_DEBUG)
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,
			0,
			D3D11_CREATE_DEVICE_DEBUG,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			nullptr,
			nullptr,
			nullptr
		);

		return SUCCEEDED(hr);
	}
#endif
}
//TGA 파일을 읽어와서 텍스처를 생성하는 helper함수
namespace DirectX
{
	HRESULT CreateTGATextureFormFile(ID3D11Device* pDevice, const wchar_t* pTexturePath, ID3D11ShaderResourceView** ppTexture)
	{
		HRESULT hResult = S_OK;
		ScratchImage image;
		TexMetadata metadata;

		hResult = LoadFromTGAFile(pTexturePath, &metadata, image);
		if (FAILED(hResult))
		{
			return hResult;
		}

		return CreateShaderResourceView(pDevice, image.GetImages(), image.GetImageCount(), metadata, ppTexture);
	}
}