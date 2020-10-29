#pragma once

// HRESULT hr should exist in the local scope for these macros to work
#define GFX_EXCEPT_NOINFO(hRes) Graphics::HResultException(__LINE__,__FILE__,(hRes))
#define GFX_THROW_NOINFO(hrCall) if(FAILED(hr = (hrCall))) throw Graphics::HResultException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPT(hRes) Graphics::HResultException(__LINE__,__FILE__,(hRes), infoManager.GetMessages())
#define GFX_THROW_INFO(hrCall) infoManager.Set(); if(FAILED(hr = (hrCall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hRes) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hRes), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__, __FILE__, v);}}
#else
#define GFX_EXCEPT(hRes) Graphics::HResultException(__LINE__, __FILE__, (hRes))
#define GFX_THROW_INFO(hrCall) GFX_THROW_NOINFO(hrCall)
#define GFX_DEVICE_REMOVED_EXCEPT(hRes) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hRes))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager(Graphics& gfx) must exist
#ifndef NDEBUG
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager(gfx)
#else
#define INFOMAN(gfx) HRESULT hr
#endif
