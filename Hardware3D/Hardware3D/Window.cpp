#include "Window.h"
#include <sstream>
#include "resource.h"
#include "WindowErrorMacros.h"
#include "imgui/imgui_impl_win32.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst,MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 64, 64, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 48, 48, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

// window main class stuff

Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
	// create window and get handle
	hWnd = CreateWindow(WindowClass::GetName(), name,
	                    WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
	                    CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
	                    nullptr, nullptr, WindowClass::GetInstance(), this
	);
	// check for hWnd error
	if (hWnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}
	// show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	// Init ImGui Win32 Implementation
	ImGui_ImplWin32_Init(hWnd);

	// Create Graphics Object
	pGfx = std::make_unique<Graphics>(hWnd, width, height);

	// register mouse raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw WND_LAST_EXCEPT();
	}
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
}

void Window::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::EnableCursor() noexcept
{
	cursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	cursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool Window::CursorEnabled() const noexcept
{
	return cursorEnabled;
}

void Window::ShowCursor() noexcept
{
	while(::ShowCursor(TRUE) < 0);
}

void Window::HideCursor() noexcept
{
	while(::ShowCursor(FALSE) >= 0);
}

void Window::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block the update loop)
	while (PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
	{
		// check for quit because PeekMessage does not signal via return
		if (msg.message == WM_QUIT)
		{
			// return of optional wrapping int signals quit (arg to PostQuitMessage is in wParam)
			return (int)msg.wParam;
		}

		// TranslateMessage will post Auxiliary WM_CHAR messages from key messages
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

Graphics& Window::Gfx()
{
	if (!pGfx)
	{
		throw WND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

int Window::GetWidth() const
{
	return width;
}

int Window::GetHeight() const
{
	return height;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer on WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from the creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message procedure to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before WM_NCCREATE, handle it with the default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window instance
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

void Window::ReleaseMouse(const POINTS pt)
{
	// release mouse if outside of window
	if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
	{
		ReleaseCapture();
		mouse.OnMouseLeave();
	}
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	const auto& imGuiIO = ImGui::GetIO();

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// clear keystate when window loses focus to prevent input from getting stuck in a keydown state
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	case WM_ACTIVATE:
		// confine/free cursor on window to foreground/background if cursor is set to disabled
		if(!cursorEnabled)
		{
			if (wParam & WA_ACTIVE)
			{
				ConfineCursor();
				HideCursor();
			}
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;
		/********** KEYBOARD MESSAGES **********/
	case WM_KEYDOWN:
		// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	case WM_SYSKEYDOWN:
		// don't use this keyboard message if ImGui wants to capture
		if (imGuiIO.WantCaptureKeyboard) break;

		// the lParam indicates if the key was previously down in the 30th bit, so this mask checks that
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // filters autoRepeat events
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// don't use this keyboard message if ImGui wants to capture
		if (imGuiIO.WantCaptureKeyboard) break;

		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		// don't use this keyboard message if ImGui wants to capture
		if (imGuiIO.WantCaptureKeyboard) break;

		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/********** END KEYBOARD MESSAGES **********/
		/********** MOUSE MESSAGES **********/
	case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			
			if (!cursorEnabled)
			{
				if (!mouse.IsInWindow())
				{
					SetCapture(hWnd);
					mouse.OnMouseEnter();
					HideCursor();
				}
				break;
			}
			
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;
			
			// in client region -> log move, and log enter + capture mouse (if not previously in widow)
			if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
			{
				mouse.OnMouseMove(pt.x, pt.y);
				if (!mouse.IsInWindow())
				{
					SetCapture(hWnd);
					mouse.OnMouseEnter();
				}
			}
				// not in client -> log move / maintain capture if button down
			else
			{
				if (mouse.LeftIsPressed() || mouse.WheelIsPressed() || mouse.RightIsPressed())
				{
					mouse.OnMouseMove(pt.x, pt.y);
				}
					// button up -> release capture / log event for leaving
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
			break;
		}
	case WM_LBUTTONDOWN:
		{
			// bring window to foreground on left click of the client region
			SetForegroundWindow(hWnd);
			if (!cursorEnabled)
			{
				ConfineCursor();
				HideCursor();
			}
			
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;

			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
			break;
		}
	case WM_RBUTTONDOWN:
		{
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;

			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
			break;
		}
	case WM_MBUTTONDOWN:
		{
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;

			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnWheelPressed(pt.x, pt.y);
			break;
		}
	case WM_LBUTTONUP:
		{
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;

			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
			ReleaseMouse(pt);
			break;
		}
	case WM_RBUTTONUP:
		{
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;

			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
			ReleaseMouse(pt);
			break;
		}
	case WM_MBUTTONUP:
		{
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;

			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnWheelReleased(pt.x, pt.y);
			ReleaseMouse(pt);
			break;
		}
	case WM_MOUSEWHEEL:
		{
			// don't use this mouse message if ImGui wants to capture
			if (imGuiIO.WantCaptureMouse) break;

			const POINTS pt = MAKEPOINTS(lParam);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
			mouse.OnWheelDelta(pt.x, pt.y, delta);
			break;
		}
		/********** END MOUSE MESSAGES **********/
		/********** RAW MOUSE MESSAGES **********/
	case WM_INPUT:
		{
			if (!mouse.RawEnabled())
			{
				break;
			}
			
			UINT size;
			// first get the size of the input data
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
			{
				// break out of msg processing if error
				break;
			}
			rawBuffer.resize(size);
			// read in the input data
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
			{
				// break out of msg processing if error
				break;
			}
			// process the raw input data
			const auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			{
				mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
			}
			break;
		}
		/********** END RAW MOUSE MESSAGES **********/
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Window Exception stuff
std::string Window::Exception::TranslateErrorCode(HRESULT hRes) noexcept
{
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer to point to it
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hRes, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}

Window::HResultException::HResultException(int line, const char* file, HRESULT hRes) noexcept
	:
	Exception(line, file),
	hRes(hRes)
{
}

const char* Window::HResultException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HResultException::GetType() const noexcept
{
	return "Half-Way Engine Window Exception";
}

HRESULT Window::HResultException::GetErrorCode() const noexcept
{
	return hRes;
}

std::string Window::HResultException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hRes);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Half-Way Engine Window Exception [No Graphics]";
}
