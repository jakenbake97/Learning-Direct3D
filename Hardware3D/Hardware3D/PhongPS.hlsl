cbuffer CBuf
{
	float3 lightPos;
};

static const float3 materialColor = (0.7f, 0.7f, 0.9f);
static const float3 ambient = { 0.05f,0.05f,0.05f };
static const float3 diffuseColor = { 1.0f,1.0f,1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 0.045f;
static const float attQuad = 0.0075f;

float4 main(float3 worldPos : Position, float3 n : Normal) : SV_TARGET
{
	// fragment to light vector data
	const float3 vecToLight = lightPos - worldPos;
const float distToLight = length(vecToLight);
const float3 dirToLight = vecToLight / distToLight;
	// diffuse attenuation
const float atten = 1.0f / (attConst + attLin + attQuad * (distToLight * distToLight));
	// diffuse intensity
const float3 diffuse = diffuseColor * diffuseIntensity * atten * max(0.0f, dot(dirToLight, n));
	// final color
return float4(saturate(diffuse + ambient), 1.0f);
}