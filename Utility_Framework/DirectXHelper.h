#pragma once
#include "Core.Definition.h"
#include <ppltasks.h>

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