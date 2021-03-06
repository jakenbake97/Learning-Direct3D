#pragma once

#include "WinInclude.h"
#include "D3DException.h"
#include "Graphics.h"
#include "Keyboard.h"
#include <memory>
#include "Mouse.h"
#include <optional>


class Window
{
public:
	class Exception : public D3DException
	{
		using D3DException::D3DException;
	public:

		static std::string TranslateErrorCode(HRESULT hRes) noexcept;
	};
	class HResultException : public Exception
	{
	public:
		HResultException(int line, const char* file, HRESULT hRes) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hRes;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
private:
	// singleton for managing the registration and cleanup of window class
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = default;
		WindowClass& operator=(const WindowClass&) = delete;

		static constexpr const char* wndClassName = "Half-Way D3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	
	void SetTitle(const std::string& title);
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	static std::optional<int> ProcessMessages() noexcept;
	Graphics& Gfx();

	int GetWidth() const;
	int GetHeight() const;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	void ReleaseMouse(POINTS pt);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	static void ShowCursor() noexcept;
	static void HideCursor() noexcept;
	static void EnableImGuiMouse() noexcept;
	static void DisableImGuiMouse() noexcept;
public:
	Keyboard kbd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics>pGfx;
	bool cursorEnabled = true;
	std::vector<BYTE> rawBuffer;
};