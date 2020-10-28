#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// graphics exception checking / throwing macros (some with dxgi info)
#define GFX_EXCEPT_NOINFO(hRes) Graphics::HResException(__LINE__,__FILE__,(hRes))
#define GFX_THROW_NOINFO(hrCall) if(FAILED(hr = (hrCall))) throw Graphics::HResException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hRes) Graphics::HResException(__LINE__,__FILE__,(hRes), infoManager.GetMessages())
#define GFX_THROW_INFO(hrCall) infoManager.Set(); if(FAILED(hr = (hrCall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hRes) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hRes), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__, __FILE__, v);}}
#else
#define GFX_EXCEPT(hRes) Graphics::HResException(__LINE__, __FILE__, (hRes))
#define GFX_THROW_INFO(hrCall) GFX_THROW_NOINFO(hrCall)
#define GFX_DEVICE_REMOVED_EXCEPT(hRes) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hRes))
#define GFX_THROW_INFO_ONLY(call) (call)
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
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
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

	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDepthSState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&depthDesc, &pDepthSState));

	// bind depth state
	pContext->OMSetDepthStencilState(pDepthSState.Get(), 1u);

	// create depth stencil texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = 800u;
	depthTexDesc.Height = 600u;
	depthTexDesc.MipLevels = 1u;
	depthTexDesc.ArraySize = 1u;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.SampleDesc.Count = 1u;
	depthTexDesc.SampleDesc.Quality = 0u;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	GFX_THROW_INFO(pDevice->CreateTexture2D(&depthTexDesc, nullptr, &pDepthStencil));

	// create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0u;

	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvDesc, &pDSV));

	// bind depth stencil view to OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif

	if (FAILED(hr = pSwap->Present(1u,0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
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
	const float color[] = {red, green, blue, 1.0f};
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u );
}

void Graphics::DrawTestTriangle(float angle, float x, float y, float z)
{	
	struct Vertex
	{
		struct
		{
			float x;
			float y;
			float z;
		} pos;
	};

	// create an array of vertices to fill the buffer with (A 2D triangle at center of screen)
	const Vertex vertices[] =
	{
		{-1.0f, -1.0f, -1.0f},
		{1.0f,  -1.0f, -1.0f},
		{-1.0f, 1.0f,  -1.0f},
		{1.0f,  1.0f,  -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f,  -1.0f, 1.0f},
		{-1.0f, 1.0f,  1.0f},
		{1.0f,  1.0f,  1.0f},
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = vertices;

	HRESULT hr;

	GFX_THROW_INFO(pDevice->CreateBuffer(&bufferDesc, &subData, &pVertexBuffer));

	// Bind vertex buffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);


	// create index buffer
	const short indices[] =
	{
		0,2,1,  2,3,1,
		1,3,5,  3,7,5,
		2,6,3,  3,6,7,
		4,5,7,  4,7,6,
		0,4,2,  2,4,6,
		0,1,4,  1,5,4
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC indBufferDesc;
	indBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indBufferDesc.CPUAccessFlags = 0u;
	indBufferDesc.MiscFlags = 0u;
	indBufferDesc.ByteWidth = sizeof(indices);
	indBufferDesc.StructureByteStride = sizeof(short);

	subData.pSysMem = indices;

	GFX_THROW_INFO(pDevice->CreateBuffer(&indBufferDesc, &subData, &pIndexBuffer));
	// Bind Index buffer to pipeline
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// create constant buffer for transformation matrix
	struct ConstantBuffer
	{
		dx::XMMATRIX transform;
	};

	const ConstantBuffer matrixBuffer =
	{
		dx::XMMatrixTranspose(
			dx::XMMatrixRotationZ(angle) *
			dx::XMMatrixRotationX(2 * angle) *
			dx::XMMatrixTranslation(x , y, z + 4) *
			dx::XMMatrixPerspectiveLH(1.0f, 0.75f, 0.5f, 10.0f)
		)
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(matrixBuffer);
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &matrixBuffer;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

	// bind constant buffer
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// create vertex shader
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader)
	);

	// bind vertex shader
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// input (vertex) layout (3D position)
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	GFX_THROW_INFO(pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(), &pInputLayout));

	// bind vertex layout
	pContext->IASetInputLayout(pInputLayout.Get());

	// create Color Buffer
	struct ConstantColorBuffer
	{
		struct 
		{
			float r;
			float g;
			float b;
			float a;
		} faceColors[6];
	};
	const ConstantColorBuffer colorBuf =
	{
		{
			{1.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f, 1.0f},
			{1.0f, 1.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 1.0f, 1.0f},
		}
	};

	wrl::ComPtr<ID3D11Buffer> pColorBuffer;
	D3D11_BUFFER_DESC colorDesc;
	colorDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colorDesc.Usage = D3D11_USAGE_DYNAMIC;
	colorDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colorDesc.MiscFlags = 0u;
	colorDesc.ByteWidth = sizeof(colorBuf);
	colorDesc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA colorSubD = {};
	colorSubD.pSysMem = &colorBuf;
	GFX_THROW_INFO(pDevice->CreateBuffer(&colorDesc, &colorSubD, &pColorBuffer));

	// bind color buffer
	pContext->PSSetConstantBuffers(0u, 1u, pColorBuffer.GetAddressOf());

	// create pixel shader
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// bind pixel shader
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	// Set primitive topology to triangle list (groups of 3 vertices)
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	pContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(std::size(indices), 0, 0));
}

// Graphics Exception Stuff
Graphics::HResException::HResException(int line, const char* file, HRESULT hr,
                                       std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hRes(hr)
{
	// join all info messages with newlines into a single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
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
	if (!info.empty())
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

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}

	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Half-Way Engine Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Half-Way Engine Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
