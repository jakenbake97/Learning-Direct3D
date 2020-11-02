cbuffer CBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectCBuf
{
	float3 materialColor;
	float specularIntensity;
	float specularPower;
};

float4 main(float3 worldPos : Position, float3 n : Normal) : SV_TARGET
{
	// fragment to light vector data
	const float3 vecToLight = lightPos - worldPos;
	const float distToLight = length(vecToLight);
	const float3 dirToLight = vecToLight / distToLight;
	// attenuation
	const float atten = 1.0f / (attConst + attLin + attQuad * (distToLight * distToLight));
	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * atten * max(0.0f, dot(dirToLight, n));
	// reflected light vector
	const float3 w = n * dot(vecToLight, n);
	const float3 r = w * 2.0f - vecToLight;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = atten * (diffuseColor * diffuseIntensity) * specularIntensity * pow(
		max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	// final color
	return float4(saturate(diffuse + ambient + specular) * materialColor, 1.0f);
}
