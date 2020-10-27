#pragma once
#include "WinInclude.h"
#include "D3DException.h"
#include <d3d11.h>

class Graphics
{
public:
	class Exception : public D3DException
	{
		using D3DException::D3DException;
	};
	class HResException : public Exception
	{
	public:
		HResException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hRes;
	};
	class DeviceRemovedException : public HResException
	{
		using HResException::HResException;
	public:
		const char* GetType() const noexcept override;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	//Graphics(const Graphics&&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	//Graphics& operator=(const Graphics&&) = delete;
	~Graphics();

	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};
