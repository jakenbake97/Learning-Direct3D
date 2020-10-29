#pragma once
#include "Bindable.h"
#include "GraphicsErrorMacros.h"

template <typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& gfx, const C& contents)
	{
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetContext(gfx)->Map(pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u, &msr));
		memcpy(msr.pData, &contents, sizeof(contents));
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
	ConstantBuffer(Graphics& gfx, const C& contents)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC constBufferDesc;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = 0u;
		constBufferDesc.ByteWidth = sizeof(contents);
		constBufferDesc.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA constSubData = {};
		constSubData.pSysMem = &contents;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&constBufferDesc, &constSubData, &pConstantBuffer));
	}
	ConstantBuffer(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC constBufferDesc;
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = 0u;
		constBufferDesc.ByteWidth = sizeof(C);
		constBufferDesc.StructureByteStride = 0u;

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&constBufferDesc, nullptr, &pConstantBuffer));
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using ConstantBuffer<C>::pConstantBuffer;
	using Bindable::GetContext;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};