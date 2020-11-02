cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attenConstant;
	float attenLin;
	float attenQuad;
}

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	float padding[2];
}

float4 main(float3 worldPos : Position, float3 n : Normal, float3 color : Color) : SV_TARGET
{
	// fragment to light vector data
	const float3 vecToLight = lightPos - worldPos;
	const float distToLight = length(vecToLight);
	const float3 dirToLight = vecToLight / distToLight;

	// attenuation
	const float atten = 1.0f / (attenConstant + attenLin * distToLight + attenQuad * (distToLight * distToLight));

	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * atten * max(0.0f, dot(dirToLight, n));

	// reflected light vector
	const float3 r = reflect(vecToLight, n);

	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = atten * (diffuseColor * diffuseIntensity) * specularIntensity * pow(
		max(0.0f, dot(normalize(r), normalize(worldPos))), specularPower);

	// final color
	return float4(saturate((diffuse + ambient + specular) * color), 1.0f);
}
