#include "IndexBuffer.h"
#include "GraphicsErrorMacros.h"

namespace Bind
{
	IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices)
		:
		count((UINT)indices.size())
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC indexBufDesc = {};
		indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufDesc.CPUAccessFlags = 0u;
		indexBufDesc.MiscFlags = 0u;
		indexBufDesc.ByteWidth = UINT(count * sizeof(unsigned short));
		indexBufDesc.StructureByteStride = sizeof(unsigned short);

		D3D11_SUBRESOURCE_DATA indexSubData = {};
		indexSubData.pSysMem = indices.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&indexBufDesc, &indexSubData, &pIndexBuffer));
	}

	void IndexBuffer::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return count;
	}
	
}
