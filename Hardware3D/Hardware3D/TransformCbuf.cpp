#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent)
	:
vcBuf(gfx),
parent(parent)
{
}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	vcBuf.Update(gfx,
		DirectX::XMMatrixTranspose(parent.GetTransformXM() * gfx.GetProjection())
	);
	vcBuf.Bind(gfx);
}
