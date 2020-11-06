#pragma once
#include "Bindable.h"
#include "GraphicsErrorMacros.h"
#include "Vertex.h"

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		template<class V>
		VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
			:
			stride(sizeof(V))
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC bufDesc = {};
			bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufDesc.Usage = D3D11_USAGE_DEFAULT;
			bufDesc.CPUAccessFlags = 0u;
			bufDesc.MiscFlags = 0u;
			bufDesc.ByteWidth = UINT(sizeof(V) * vertices.size());
			bufDesc.StructureByteStride = sizeof(V);

			D3D11_SUBRESOURCE_DATA subData = {};
			subData.pSysMem = vertices.data();
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufDesc, &subData, &pVertexBuffer));
		}
		VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf);
		void Bind(Graphics& gfx) noexcept override;
	protected:
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};
	
}
