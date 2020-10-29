#pragma once

#define WND_EXCEPT(hr) Window::HResultException(__LINE__,__FILE__, (hr))
#define WND_LAST_EXCEPT() Window::HResultException(__LINE__,__FILE__, GetLastError())
#define WND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__,__FILE__)