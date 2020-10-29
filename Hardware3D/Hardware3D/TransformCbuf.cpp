#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
parent(parent)
{
	if (!pVcBuf)
	{
		pVcBuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx);
	}
}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	pVcBuf->Update(gfx,
		DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection())
	);
	pVcBuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcBuf;