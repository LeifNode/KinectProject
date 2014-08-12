#if !defined(LIGHTING_HELPER_HLSL)
#define LIGHTING_HELPER_HLSL

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w is the specular power
	//float4 Reflect;
	float4 Emissive; // w is emissive multiplier
};

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float3 Direction;
	float pad;
};

struct PointLight
{ 
	float4 Ambient;
	float4 Diffuse;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

void ComputeDirectionalLight(Material mat, DirectionalLight L, 
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse,
						     out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ambient = mat.Ambient * L.Ambient;

	float3 lightVector = normalize(-L.Direction);

	float diffuseFactor = dot(lightVector, normal);

	[flatten]
	if (diffuseFactor > 0.0)
	{
		//Reflection vector method which is not as physically accurate as the half vector method
		/*float3 lightVecReflection = reflect(lightVector, normal);

		float specularFactor = pow(saturate(dot(lightVecReflection, toEye)), mat.Specular.w); */

		float3 halfVector = normalize(toEye + lightVector);
		float specularFactor = pow(saturate(dot(halfVector, normal)), mat.Specular.w);

		diffuse = mat.Diffuse * L.Diffuse * diffuseFactor;
		spec = mat.Specular * L.Diffuse * specularFactor;
	}
}

void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
				   out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVector = L.Position - pos;

	float distance = length(lightVector);

	if (distance > L.Range)
		return;

	lightVector /= distance;

	ambient = mat.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVector, normal);

	[flatten]
	if (diffuseFactor > 0.0)
	{
		float3 halfVector = normalize(toEye + lightVector);
		float specularFactor = pow(saturate(dot(halfVector, normal)), mat.Specular.w);

		diffuse = mat.Diffuse * L.Diffuse * diffuseFactor;
		spec = mat.Specular * L.Diffuse * specularFactor;
	}

	float att = 1.0f / dot(L.Att, float3(1.0f, distance, distance*distance));

	diffuse *= att;
	spec    *= att;
	spec = max(spec, 0.0f);
}

void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
				  out float4 ambient, out float4 diffuse, out float4 spec)
{
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVector = L.Position - pos;

	float distance = length(L.Position - pos);

	if (distance > L.Range)
		return;

	lightVector /= distance;

	ambient = mat.Ambient * L.Ambient;

	float diffuseFactor = dot(lightVector, normal);

	[flatten]
	if (diffuseFactor > 0.0)
	{
		float3 halfVector = normalize(toEye + lightVector);
		float specularFactor = pow(saturate(dot(halfVector, normal)), mat.Specular.w);

		diffuse = mat.Diffuse * L.Diffuse * diffuseFactor;
		spec = mat.Specular * L.Diffuse * specularFactor;
	}

	float spot = pow(saturate(dot(-lightVector, L.Direction)), L.Spot);

	float att = spot / dot(L.Att, float3(1.0f, distance, distance*distance));

	ambient *= spot;
	diffuse *= att;
	spec    *= att;
}

#endif