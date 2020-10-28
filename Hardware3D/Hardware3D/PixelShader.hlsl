cbuffer CBuf
{
	float4 faceColors[6];
}

float4 main(uint tId : SV_PrimitiveID) : SV_Target
{
	return faceColors[tId / 2];
}