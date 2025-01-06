#pragma once
#define WIN32_LEAN_AND_MEAN 
// header
#include <comdef.h>
#include <dxgi1_4.h>
#include <d3d11.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <DirectXColors.h>
#include <dwrite_3.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include <Directxtk/DDSTextureLoader.h>
#include <Directxtk/WICTextureLoader.h>
#include <DirectXTex.h>
#include <DirectXMath.h>
#include <directxtk/simplemath.h>
#include <windows.h>
#include <wincodec.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;
//STL
#include <array>
#include <algorithm>
#include <filesystem>
namespace file = std::filesystem;
#include <fstream>
#include <functional>
#include <new>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <stack>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <cstdint>
#include <ranges>
//Custom
#include "ClassProperty.h"
#include "TypeDefinition.h"
#include "DirectXHelper.h"
#include "LinkedListLib.hpp"
