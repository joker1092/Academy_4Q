#pragma once
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// ��� ����
#include <windows.h>
#include <utility>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <DirectXColors.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <comdef.h>
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <Directxtk/WICTextureLoader.h>
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <directxtk/simplemath.h>
#include <wrl/client.h>
#include <unordered_map>
#include <string>
#include <filesystem>
namespace file = std::filesystem;
#include <fstream>
#include <memory>
#include <vector>

#include "ClassProperty.h"
#include "TypeDefinition.h"