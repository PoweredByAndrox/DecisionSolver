#pragma once
#include <vector>
#include <DirectXMath.h>
#include <stdio.h> 
#include <direct.h>
#include <tchar.h>
#include <memory>
#include <d3dcompiler.h>

#include <d3dx11.h>
#include <dxgi1_2.h>

	// Included for use USES_CONVERSION (or A2W, W2A)
#include <AtlConv.h>

#pragma warning( push )
#pragma warning( disable: 4995 )
#include <fstream>
#include <sstream>
#pragma warning( pop )

#include "DXUT.h"
#include "SDKmisc.h"
#include "DXUTmisc.h"

	// For ThrowIfFailed
#include "../Src/PlatformHelpers.h"

using namespace DirectX;
using namespace std;

#if defined(Never)
	// Maybe this doesn't cause errors?
#define SPDLOG_TRACE_ON
#include "spdlog/spdlog.h"
using namespace spdlog;
#endif

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3dx9d.lib")

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "DxErr.lib")
#pragma comment (lib, "Dxgi.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment (lib, "D3DCSX.lib")
#pragma comment (lib, "DXUT.lib")
#pragma comment (lib, "DXUTOpt.lib")
#pragma comment (lib, "assimp-vc140-mt.lib")