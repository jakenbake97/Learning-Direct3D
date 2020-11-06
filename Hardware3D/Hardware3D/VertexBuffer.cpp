#include "VertexBuffer.h"

namespace Bind
{
	VertexBuffer::VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf)
		:
	stride((UINT)vbuf.GetLayout().Size())
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.CPUAccessFlags = 0u;
		bufDesc.MiscFlags = 0u;
		bufDesc.ByteWidth = UINT( vbuf.SizeBytes() );
		bufDesc.StructureByteStride = stride;

		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = vbuf.GetData();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufDesc, &subData, &pVertexBuffer));
	}

	
	void VertexBuffer::Bind(Graphics& gfx) noexcept
	{
		const UINT offset = 0u;
		GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	}
	
}
