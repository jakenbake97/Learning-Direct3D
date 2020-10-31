Texture2D tex;

SamplerState sampState;

float4 main(float2 tc : TexCoord) : SV_TARGET
{
	return tex.Sample(sampState, tc);
}