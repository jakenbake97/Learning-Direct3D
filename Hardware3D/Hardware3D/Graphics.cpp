#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")

// graphics exception checking / throwing macros (some with dxgi info)
#define GFX_EXCEPT_NOINFO(hRes) Graphics::HResException(__LINE__,__FILE__,(hRes))
#define GFX_THROW_NOINFO(hrCall) if(FAILED(hr = (hrCall))) throw Graphics::HResException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hRes) Graphics::HResException(__LINE__,__FILE__,(hRes), infoManager.GetMessages())
#define GFX_THROW_INFO(hrCall) infoManager.Set(); if(FAILED(hr = (hrCall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hRes) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hRes), infoManager.GetMessages())
#else
#define GFX_EXCEPT(hRes) Graphics::HResException(__LINE__, __FILE__, (hRes))
#define GFX_THROW_INFO(hrCall) GFX_THROW_NOINFO(hrCall)
#define GFX_DEVICE_REMOVED_EXCEPT(hRes) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hRes))
#endif


Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;
	
	// create device, front/back buffers, swap chain, rendering context
	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	// gain access to texture sub-resource in swap chain (back buffer)
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif

	if( FAILED(hr = pSwap->Present(1u,0u)))
	{
		if(hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
	pSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

// Graphics Exception Stuff
Graphics::HResException::HResException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
Exception(line, file),
hRes(hr)
{
	// join all info messages with newlines into a single string
	for(const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if(!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HResException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
		if(!info.empty())
		{
			oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
		}
		oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HResException::GetType() const noexcept
{
	return "Half-Way Engine Graphics Exception";
}

HRESULT Graphics::HResException::GetErrorCode() const noexcept
{
	return hRes;
}

std::string Graphics::HResException::GetErrorString() const noexcept
{
	return DXGetErrorString(hRes);
}

std::string Graphics::HResException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hRes, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HResException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Half-Way Engine Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}