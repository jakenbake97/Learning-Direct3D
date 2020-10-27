#pragma once
#include "WinInclude.h"
#include "D3DException.h"
#include <d3d11.h>
#include "DxgiInfoManager.h"
#include <string>
#include <vector>
#include <wrl.h>

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
		HResException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hRes;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string>) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HResException
	{
		using HResException::HResException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics(const Graphics&&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	Graphics& operator=(const Graphics&&) = delete;
	~Graphics() = default;

	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawTestTriangle();
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};
