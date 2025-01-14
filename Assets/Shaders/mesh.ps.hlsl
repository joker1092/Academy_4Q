
#include "common.hlsli"
#include "lighting.hlsli"
#include "pbr.hlsli"

cbuffer CameraBuffer                     : register(b0)   
{
	matrix	            VP;
	float3	            CameraPosition;
	float	            Exposure;
	float3	            ViewDir;
	float 				_placeholder4;
};
cbuffer SceneBuffer                     : register(b1)          
{
	float3	            AmbientColor;
	float	            IBLIntensity;
    float3	            SunPos;
	uint	            PreciseShadows;
	float3	            SunColor;
	float	            _placeholder3;
	float3	            IBLColor;
};

cbuffer MaterialProperties               : register(b2)
{
    float3	            Diffuse;
    float				Roughness;
    float3              Emissive;
    float               Metalness;
    uint				Bitmask;
};

Texture2D<float4>      DiffuseTexture      : register(t0);
Texture2D<float>       MetallicTexture     : register(t1);
Texture2D<float>       RoughnessTexture    : register(t2);
Texture2D<float>       OcclusionTexture    : register(t3);
Texture2D<float3>      EmissiveTexture     : register(t4);
Texture2D<float3>      NormalTexture       : register(t5);

TextureCube       	   SkyboxTexture   	   : register(t6);
Texture2D<float>       ShadowTexture   	   : register(t7);
TextureCube			   SkyboxPrefilter	   : register(t8);
Texture2D			   brdfLUT			   : register(t9);

SamplerState    AnisotropicSampler  : register(s0);
SamplerState    LinearSampler  		: register(s1);
SamplerState    ClampSampler  		: register(s2);
SamplerState    CubemapSampler  	: register(s3);

struct In
{
	float3x3 vTBN        	: TANGENT0;
    float2 vTexcoord        : TEXCOORD0;
    float3 vPixelWorldPos   : POSITION0;
	float4 vPixelLightSpacePos   : POSITION1;
    float4 vPosition        : SV_POSITION;
};

struct PsOut
{
    float4 Color: SV_Target0;
    float  Shadow: SV_Target1;
};

void GetTextureSamples(
	in float2 texcoord,
	out float4 diffuse,
	out float3 emissive,
	out float metalness,
	out float roughness,
	out float occlusion)
{
	[branch]	// Diffuse
	if(Bitmask & (1 << 0))
	{
        diffuse = DiffuseTexture.Sample(AnisotropicSampler, texcoord) * float4(Diffuse, 1.0f);
        diffuse.rgb = GammaDecode(diffuse.rgb);
    }
	else
	{
		diffuse = float4(Diffuse, 1.0f);
    }

	[branch]	// Metalness
	if(Bitmask & (1 << 1))
	{
        metalness = step(MetallicTexture.Sample(AnisotropicSampler, texcoord), 0.5f);
    }
	else
	{
		metalness = step(Metalness, 0.5f);
	}

	[branch]	// Roughness
	if(Bitmask & (1 << 2))
	{
        roughness = RoughnessTexture.Sample(AnisotropicSampler, texcoord);
    }
	else
	{
		roughness = Roughness;
	}

	[branch]	// Occlusion
	if(Bitmask & (1 << 3))
	{
        occlusion = OcclusionTexture.Sample(AnisotropicSampler, texcoord);
    }
	else
	{
		occlusion = 1.0f;
	}

	[branch]	// Emissive
	if(Bitmask & (1 << 4))
	{
        emissive = EmissiveTexture.Sample(AnisotropicSampler, texcoord);
    }
	else
	{
		emissive = Emissive;
	}
}

float GetShadow(float4 fragPosLightSpace, float3 N, float3 L)
{
	// const float bias = 0.002f;
    float2 coords = float2(0.0f,0.0f);
	float currentDepth = (fragPosLightSpace.z / fragPosLightSpace.w);

	if(currentDepth > 1.0f)
	return 0.0f;
	// Tansition to [-1,1] -> [0,1]
	// And flip y due to DirectX notation
	coords.x = (fragPosLightSpace.x / fragPosLightSpace.w) * 0.5f + 0.5f;
	coords.y = (-fragPosLightSpace.y / fragPosLightSpace.w) * 0.5f + 0.5f;

	float bias = max(0.008f * (1.0f - dot(N, L)), 0.0001f);  
    const float dim = 8192.0f; // Constant shadow map

	const float texelSize = 1.0f / dim;
	float shadow = 0.0f;

	[branch]
	if(PreciseShadows)
	{
		[unroll]
		for(int x = 0; x < 5; ++x)
		{
			float xoffs = 0.5f * x - 1.0f;
			[unroll]
			for(int y = 0; y < 5; ++y)
			{
				float yoffs = 0.5f * y - 1.0f;
				float pcfDepth = ShadowTexture.Sample(ClampSampler, coords + float2(xoffs, yoffs) * texelSize.xx);
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
			}
		}
		shadow /= 25.0f;
		return (1 - shadow);
	}
	else
	{
		[unroll]
		for(int x = -1; x <= 1; ++x)
		{
			[unroll]
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = ShadowTexture.Sample(ClampSampler, coords + float2(x,y) * texelSize.xx);
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
			}
		}
		shadow /= 9.0f;
		
		return (1 - shadow);
	}

}

uint GetMipMapLevel(TextureCube tex)
{
    uint mipLevel = 0;
    uint width, height;
    tex.GetDimensions(0, width, height, mipLevel);
    return mipLevel;
}

float3 GetImageBasedLighting(PBRParameters params, float3 diffuse, float metallic, float roughness, float3 V, float3 N)
{
    float3 irradiance = SkyboxTexture.Sample(CubemapSampler, N).rgb;
	
    float3 diff = diffuse * irradiance;
    float3 prefilteredColor = SkyboxPrefilter.SampleLevel(CubemapSampler, reflect(-V, N), roughness * GetMipMapLevel(SkyboxTexture)).rgb;
    float2 brdf = brdfLUT.Sample(ClampSampler, float2(max(dot(N, V), 0.f), roughness)).xy;

    float3 specular = prefilteredColor * (params.Fresnel /** brdf.x + brdf.y*/);
	
    float3 Kd = (float3) 1.f - params.Fresnel;
    Kd *= (1.0f - metallic);
	
    float3 ambient = (Kd * diff) + specular;
    ambient *= diffuse;
	
    return ambient;
}

bool IsTextureNormalEnabled()
{
	return Bitmask & (1 << 5); // Normal
}

float3 TwoChannelNormalX2(float2 normal)
{
    float2 xy = 2.0f * normal - 1.0f;
    float z = sqrt(1 - dot(xy, xy));
    return float3(xy.x, xy.y, z);
}

float3 GetNormal(
	in float2 texcoord,
	in float3x3 tbn)
{

	float3 sampledNormal = (2.0f * NormalTexture.Sample(LinearSampler, texcoord)) - 1.0f;
	sampledNormal = normalize(mul(sampledNormal, tbn));

	return sampledNormal;
}

PsOut main(in In input)
{
	float4 diffuse;
	float3 emissive;
	float metalness;
	float roughness;
	float occlusion;

	GetTextureSamples(
		input.vTexcoord,
		diffuse,
		emissive,
		metalness,
		roughness,
		occlusion
	);

	float3 N = float3(0.0f, 0.0f, 0.0f);

	[branch]			// Normal
	if (IsTextureNormalEnabled())
	{
		N = GetNormal(input.vTexcoord, input.vTBN);
	}
	else
	{
		N = normalize(input.vTBN[2]);
	}

	float3 V = normalize(CameraPosition - input.vPixelWorldPos);	// Camera dir
	float3 P = input.vPixelWorldPos;								// Pixel pos
    float3 L = normalize(SunPos); // Light dir

	PBRParameters params = ComputePBRParameters(
		diffuse.xyz,
		metalness,
		roughness
	);

	float3 suncolor = SunColor * GetDirectionalLightAttenuation();

    float3 color = ComputePBRColor(
		params,
		V,
		N,
		L,
		suncolor
	);

    float3 iblcolor = GetImageBasedLighting(params, diffuse.xyz, metalness, roughness, V, N);
    color += iblcolor;

    PsOut output;
    output.Color = ToneMap(float4(color, 1.0f));
	output.Color.rgb = GammaEncode(output.Color.rgb);
	
	float shadow = GetShadow(input.vPixelLightSpacePos, N, L);
	
	// Lift shadows
    output.Shadow = shadow + IBLIntensity * 2.0f;
    clamp(output.Shadow, 0.0f, 1.0f);

    return output;
}