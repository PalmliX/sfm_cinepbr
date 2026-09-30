#include "common_ps_fxc.h"
#include "common_flashlight_fxc.h"
#include "common_lightmappedgeneric_fxc.h"
#include "shader_constant_register_map.h"
#include "pbr_common_ps2_3_x.h"

const float4 cBaseColor : register(PSREG_SELFILLUMTINT);
#define g_f3Tint (cBaseColor.rgb)
#define g_f1Fullbright (cBaseColor.w)

const float4 cDiffuseModulation : register(PSREG_DIFFUSE_MODULATION);
const float4 cShadowTweaks : register(PSREG_ENVMAP_TINT__SHADOW_TWEAKS);

#if DUALLOBE
const float4 cDualLobeControls : register(PSREG_SELFILLUM_SCALE_BIAS_EXP);
#define g_f1DualLobe_RoughnessBias	(cDualLobeControls.x)
#define g_f1DualLobe_LerpFactor		(cDualLobeControls.y)
#endif

const float3 cAmbientCube[6]						: register(PSREG_AMBIENT_CUBE);

const float4 cNormalMapControls : register(PSREG_SHADER_CONTROLS_2);
#define g_f3NormalMapFlips (cNormalMapControls.xyz)
#define g_f1NormalMapFactor (cNormalMapControls.w)

const float4 cEyePos : register(PSREG_EYEPOS_SPEC_EXPONENT);
#define g_f3CameraPos (cEyePos.xyz)
#define g_f1EnvMapMips (cEyePos.w)

const float4 cFogParams : register(PSREG_FOG_PARAMS);

const float4 cFlashlightAttenuationFactors : register(PSREG_FLASHLIGHT_ATTENUATION);
const float4 cProjTexPos : register(PSREG_FLASHLIGHT_POSITION_RIM_BOOST);
#define g_f3ProjTexPosition (cProjTexPos.xyz)

const float4x4 xmFlashlightWorldToTexture : register(PSREG_FLASHLIGHT_TO_WORLD_TEXTURE);

PixelShaderLightInfo cLightInfo[3]					: register(PSREG_LIGHT_INFO_ARRAY);

#if PARALLAXOCCLUSION
const float4 cParallaxParms : register(PSREG_SHADER_CONTROLS);
#define g_f1ParallaxDepth (cParallaxParms.r)
#define g_f1ParallaxCenter (cParallaxParms.g)
#endif

#if UBERLIGHT
const float3 cSmoothEdge0 : register(PSREG_UBERLIGHT_SMOOTH_EDGE_0);
const float3 cSmoothEdge1 : register(PSREG_UBERLIGHT_SMOOTH_EDGE_1);
const float3 cSmoothOneOverWidth : register(PSREG_UBERLIGHT_SMOOTH_EDGE_OOW);
const float4 cShearRound : register(PSREG_UBERLIGHT_SHEAR_ROUND);
const float4 cAABB : register(PSREG_UBERLIGHT_AABB);
const float4x4 xmFlashlightWorldToLight : register(PSREG_UBERLIGHT_WORLD_TO_LIGHT);
#endif

#if PLANARREFLECTION
sampler Sampler_PlanarReflection : register(s11);
const float4 g_PlanarBlurScale : register(c45);
#endif

// --- CUSTOM REGISTERS ---
const float3 cEnvmapOffset : register(c70);

// Alias s10 safely without redefining the sampler slot
#define Sampler_FlakeNormal Sampler_ThicknessTexture

const float4 cPearlColorData : register(c71);
#define g_f3PearlColor (cPearlColorData.xyz)
#define g_f1PearlBlend (cPearlColorData.w)

const float4 cPearlParams : register(c72);
#define g_f1PearlTransition (cPearlParams.x)

const float4 cCarPaintData : register(c73);
#define g_f3CarPaintColor (cCarPaintData.xyz)
#define g_f1FlakeScale (cCarPaintData.w)

const float4 cCarPaintMode : register(c74);
#define g_f1CarPaintMode  (cCarPaintMode.x)
#define g_f1CarPaintGloss (cCarPaintMode.y)
#define g_f1HairMode      (cCarPaintMode.z) 

const float4 cHairData : register(c77);
#define g_f1HairGloss             (cHairData.x)
#define g_f1HairPrimaryBrightness (cHairData.y)
#define g_f1HairGrazingBrightness (cHairData.z)

const float4 cFlakeParams : register(c76);
#define g_f3FlakeParams (cFlakeParams.xyz)
// -----------------------------------

const float4 cVariousControls : register(PSREG_PBR_EXTRA_FACTORS);
#define g_f1EmissiveFactor (cVariousControls.x)
#define g_f1SpecularFactor (cVariousControls.y)
#define g_f1SSSIntensity (cVariousControls.z)
#define g_f1SSSPower (cVariousControls.w)

const float4 cSSSColor : register(PSREG_PBR_SSS_COLOR);
#define g_f3SSSColor (cSSSColor.rgb)

const float4 cMRAOMultiplier : register(PSREG_PBR_MRAOMULTIPLIER);
#define g_f3MRAOMultiplier (cMRAOMultiplier.xyz)

const float4 cMRAOBias : register(PSREG_PBR_MRAOBIAS);
#define g_f3MRAOBias (cMRAOBias.xyz)
#define g_f1MicroShadowFactor (cMRAOBias.w)

const float4 cMRAOExponent : register(PSREG_PBR_MRAOEXPONENT);
#define g_f3MRAOExponent (cMRAOExponent.xyz)
#define g_flEnvDlightFactor (cMRAOExponent.w)

const float4 cAlphaTestRef : register(c75);
#define g_f1AlphaTestReference (cAlphaTestRef.x)
#define g_f1AlphaToCoverage    (cAlphaTestRef.y)
#define g_f1MetalEnvMask       (cAlphaTestRef.z)

//==================================================================================================
// Samplers
//==================================================================================================

sampler Sampler_BaseColor : register(s0);
sampler Sampler_MRAOTexture : register(s1);
sampler Sampler_NormalTexture : register(s2);
#if WRINKLEMAP
sampler Sampler_Compress : register(s3);
sampler Sampler_Stretch : register(s4);
sampler Sampler_NormalCompress : register(s5);
sampler Sampler_NormalStretch : register(s6);
#endif
sampler Sampler_SSAO : register(s7);

#if EMISSIVE
sampler Sampler_EmissionTexture : register(s8);
#endif
sampler Sampler_Lightwarp : register(s9);
sampler Sampler_ThicknessTexture : register(s10);

#if !FLASHLIGHT
sampler Sampler_Lightmap : register(s13);
sampler Sampler_Envmap : register(s14);
#else
sampler Sampler_ProjTexCookie : register(s12);
sampler Sampler_RandRot : register(s13);
sampler Sampler_ShadowDepth : register(s14);
#endif

sampler Sampler_Envmap_Flashlight : register(s15);

struct PS_INPUT
{
	float2 vPos : VPOS;
	float4 WorldPos_ProjPosZ : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
	float4 LightAttenuations : TEXCOORD2;
	float4 ProjPosXYW_WrinkleWeight : TEXCOORD4;

	float4 LightmapTexCoord1And2 : TEXCOORD5;
	float4 LightmapTexCoord3 : TEXCOORD6;

	float3 Tangent : TANGENT;
	float3 Bitangent : BINORMAL;
	float3 Normal : NORMAL;

	float NoCullDirection : VFACE;
};

float ApplyMicroShadow(float ao, float3 N, float3 L, float shadow)
{
	float aperture = 2.0 * ao * ao;
	float microShadow = saturate(abs(dot(L, N)) + aperture - 1.0f);
	return shadow * microShadow;
}

// --- KAJIYA-KAY HAIR LIGHTING (STRICT PIGMENT TINTING) ---
float3 calculateHairLight(float3 f3LightDir, float3 f3LightColor, float3 f3ViewDir, float3 f3StrandDir, float3 f3BaseNormalWS, float3 f3DiffuseColor, float f1PrimaryGloss, float f1SecondaryGloss)
{
	float3 f3HalfDir = normalize(f3LightDir + f3ViewDir);

	// Scheuermann Tangent Shift: Primary shifts toward root (-), Secondary shifts toward tip (+)
	float3 t1 = normalize(f3StrandDir + (f3BaseNormalWS * -0.1f));
	float3 t2 = normalize(f3StrandDir + (f3BaseNormalWS * 0.15f));

	// 1. Primary Highlight
	float f1TdotH1 = dot(t1, f3HalfDir);
	float f1SinTH1 = sqrt(max(0.0f, 1.0f - f1TdotH1 * f1TdotH1));
	float f1DirAtten1 = smoothstep(-1.0f, 0.0f, dot(t1, f3HalfDir));
	float f1Spec1 = pow(max(0.0f, f1SinTH1), f1PrimaryGloss) * f1DirAtten1;

	// 2. Secondary Highlight
	float f1TdotH2 = dot(t2, f3HalfDir);
	float f1SinTH2 = sqrt(max(0.0f, 1.0f - f1TdotH2 * f1TdotH2));
	float f1DirAtten2 = smoothstep(-1.0f, 0.0f, dot(t1, f3HalfDir));
	float f1Spec2 = pow(max(0.0f, f1SinTH2), f1SecondaryGloss) * f1DirAtten2;

	// 3. Wrap Diffuse
	float f1Wrap = 0.5f;
	float f1WrapDiffuse = max(0.0f, (dot(f3BaseNormalWS, f3LightDir) + f1Wrap) / (1.0f + f1Wrap));
	float3 f3FinalDiffuse = f3DiffuseColor * f3LightColor * f1WrapDiffuse;

	// --- STRICT PIGMENT BINDING (NO PURE WHITE) ---
	// Instead of lerping with white (1.0), we lift the dark values of the diffuse color 
	// while preserving its exact hue, ensuring the highlight is 100% colored by the hair.
	float3 f3PigmentTint = max(f3DiffuseColor, dot(f3DiffuseColor, float3(0.299f, 0.587f, 0.114f)));

	// Apply smooth non-linear curves to the brightness dials
	float f1PrimaryCurve = g_f1HairPrimaryBrightness * g_f1HairPrimaryBrightness;
	float f1SecondaryCurve = g_f1HairGrazingBrightness;

	// The primary highlight is now completely driven by the hair's actual texture color
	float3 f3FinalPrimary = f1Spec1 * f3LightColor * f3PigmentTint * f1PrimaryCurve * 1.5f;
	float3 f3FinalSecondary = f1Spec2 * f3DiffuseColor * f3LightColor * f1SecondaryCurve;

	return f3FinalDiffuse + f3FinalPrimary + f3FinalSecondary;
}

float4 main(PS_INPUT i) : COLOR
{
	#if USEENVAMBIENT
		float3 EnvAmbientCube[6];
		setupEnvMapAmbientCube(EnvAmbientCube, Sampler_Envmap);
	#else
		#define EnvAmbientCube cAmbientCube
	#endif

	float3 f3WorldPos = i.WorldPos_ProjPosZ.xyz;

	float3 f3ProjPos = float3(i.ProjPosXYW_WrinkleWeight.xy, i.WorldPos_ProjPosZ.w);
	f3ProjPos.xy /= i.ProjPosXYW_WrinkleWeight.z;

	float3x3 xmTBN = float3x3(i.Tangent, i.Bitangent, i.Normal);
	float3 f3NormalVertex = i.Normal;
	float3 f3ViewDir = g_f3CameraPos - f3WorldPos;
	float3 f3HairStrandWS = i.Bitangent;

	#if PARALLAXOCCLUSION
		float3 f3ViewDirTS = worldToRelative(f3ViewDir, i.Tangent, i.Bitangent, f3NormalVertex);
		float2 f2TexCoord = parallaxCorrect(i.TexCoord, f3ViewDirTS, f3ViewDir, i.Normal, Sampler_NormalTexture, g_f1ParallaxDepth, g_f1ParallaxCenter);
	#else
		float2 f2TexCoord = i.TexCoord;
	#endif

		float4 f4BaseTexture = tex2D(Sampler_BaseColor, f2TexCoord);

		// ADD THIS BLOCK IMMEDIATELY AFTER SAMPLING THE BASE TEXTURE
	// ADD THIS BLOCK IMMEDIATELY AFTER SAMPLING THE BASE TEXTURE
	#if ALPHATEST
#		if WORLD_NORMAL
	// SSAO Pass: We must hard-clip the geometry so shadows don't pool on invisible pixels
		clip(f4BaseTexture.a - g_f1AlphaTestReference);
	#else
	// Color Pass: Check if A2C is enabled via our constant
		if (g_f1AlphaToCoverage < 0.5f)
		{
			// A2C is OFF: Perform a hard HLSL clip
			clip(f4BaseTexture.a - g_f1AlphaTestReference);
		}
		else
		{
			// A2C is ON: Preserve the smooth gradient for the hardware MSAA solver!
			// We still clip absolute zero (0.01) to save GPU cycles on totally empty space.
			clip(f4BaseTexture.a - 0.01f);
		}
		#endif
	#endif

		// --- NORMAL MAP & FLAKE BLENDING ---
		float4 f4NormalTS_raw = tex2D(Sampler_NormalTexture, f2TexCoord);
		float3 f3NormalTS = f4NormalTS_raw.xyz * 2.0f - 1.0f;
		float3 f3FlakeNormalTS = f3NormalTS;

		// Declare the variable in the main scope so the whole file can read it
		float rawFlakeMask = 0.0f;
		float f1CarPaintMask = 1.0f;

		if (g_f1CarPaintMode > 0.5f)
		{
			float4 f4Flakes = tex2D(Sampler_FlakeNormal, f2TexCoord * g_f1FlakeScale);
			float3 f3FlakesTS = f4Flakes.xyz * 2.0f - 1.0f;

			// Update the scoped variable instead of declaring a new one here
			rawFlakeMask = length(f3FlakesTS.xy);

			// 1. Apply the 0.8 midtone boost to the flakes directly
			f3FlakesTS.xy *= 0.8f;

			// 2. Reoriented Normal Mapping (RNM) Blend
			// This mathematically rotates the flakes to perfectly follow the curves of the base normal map
			float3 n1 = f3NormalTS;
			float3 n2 = f3FlakesTS;

			n1.z += 1.0f;
			n2.xy = -n2.xy;

			f3FlakeNormalTS = normalize(n1 * dot(n1, n2) / n1.z - n2);
		}

		float f1WrinkleAmount, f1StretchAmount, f1TextureAmount;
		#if WRINKLEMAP
			float f1WrinkleWeight = i.ProjPosXYW_WrinkleWeight.w;
			f1WrinkleAmount = saturate(-f1WrinkleWeight);
			f1StretchAmount = saturate(f1WrinkleWeight);
			f1TextureAmount = 1.0f - f1WrinkleAmount - f1StretchAmount;

			float3 f3WrinkleColor = tex2D(Sampler_Compress, f2TexCoord).rgb;
			float3 f3StretchColor = tex2D(Sampler_Stretch, f2TexCoord).rgb;
			f4BaseTexture.rgb = f1TextureAmount * f4BaseTexture.rgb + f1WrinkleAmount * f3WrinkleColor + f1StretchAmount * f3StretchColor;

			float3 f3WrinkleNormalTS = tex2D(Sampler_NormalCompress, f2TexCoord).xyz;
			float3 f3StretchNormalTS = tex2D(Sampler_NormalStretch, f2TexCoord).xyz;

			f3NormalTS = f1TextureAmount * f3NormalTS + f1WrinkleAmount * f3WrinkleNormalTS + f1StretchAmount * f3StretchNormalTS;
			f3FlakeNormalTS = f1TextureAmount * f3FlakeNormalTS + f1WrinkleAmount * f3WrinkleNormalTS + f1StretchAmount * f3StretchNormalTS;
		#endif

		f4BaseTexture.rgb *= g_f3Tint;

		float f1NoCullSign = sign(i.NoCullDirection);
		f3NormalTS *= f1NoCullSign * g_f3NormalMapFlips;
		f3NormalTS = lerp(float3(0.0f, 0.0f, 1.0f), f3NormalTS, g_f1NormalMapFactor);

		f3FlakeNormalTS *= f1NoCullSign * g_f3NormalMapFlips;
		f3FlakeNormalTS = lerp(float3(0.0f, 0.0f, 1.0f), f3FlakeNormalTS, g_f1NormalMapFactor);

		float3 f3NormalWS = normalize(mul(f3NormalTS, xmTBN));
		float3 f3FlakeNormalWS = normalize(mul(f3FlakeNormalTS, xmTBN));
		// -----------------------------------

		#if WORLD_NORMAL
			float fSSAODepth = i.LightmapTexCoord3.w;
			return float4(f3NormalWS, fSSAODepth);
		#endif

			// --- ENVMAP METALNESS MASK ---
			float4 f4MRAOTexture = tex2D(Sampler_MRAOTexture, f2TexCoord);
			f4MRAOTexture.rgb = saturate(g_f3MRAOMultiplier * pow(max(f4MRAOTexture.rgb, 0.0f), g_f3MRAOExponent) + g_f3MRAOBias);

			// Variable is assigned without the 'float' re-declaration
			f1CarPaintMask = f4MRAOTexture.a;
			float f1Metalness = f4MRAOTexture.r;

			// Dynamically mask the envmap based on user parameter
			float flEnvmapMask = 1.0f;
			if (g_f1MetalEnvMask > 0.5f && g_f1CarPaintMode < 0.5f) {
				flEnvmapMask = f1Metalness;
			}

			float3 f3DiffuseColor = (1.0f - f1Metalness) * f4BaseTexture.rgb;
			float3 f3SpecularColor = lerp(0.04f, f4BaseTexture.rgb, f1Metalness);
			float f1Roughness = max(0.02f, f4MRAOTexture.g);
			float f1AmbientOcclusion = f4MRAOTexture.b;
			float f1SecondaryRoughness = f1Roughness;

			float f1TotalSpecularFade = 1.0f;

				// --- CAR PAINT & LOBE ROUTING ---
				float3 f3Lobe1Diffuse = f3DiffuseColor;
				float3 f3Lobe1Specular = f3SpecularColor;
				float3 f3Lobe2Diffuse = f3DiffuseColor;
				float3 f3Lobe2Specular = f3SpecularColor;

				if (g_f1CarPaintMode > 0.5f) {
					// 1) Car Paint Color Fallback
					float3 paintColor = g_f3CarPaintColor;
					if (dot(paintColor, paintColor) < 0.0001f) {
						paintColor = f3DiffuseColor;
					}

					// LOBE 1: Gets the smooth paint diffuse and clearcoat specular
					f3Lobe1Diffuse = lerp(f3Lobe1Diffuse, paintColor, f1CarPaintMask);
					f3Lobe1Specular = lerp(f3Lobe1Specular, float3(0.04f, 0.04f, 0.04f), f1CarPaintMask);

					float clearcoatRoughness = lerp(0.3f, 0.01f, g_f1CarPaintGloss);
					f1Roughness = lerp(f1Roughness, clearcoatRoughness, f1CarPaintMask);

					// LOBE 2: Gets NO diffuse (this prevents the bumpy flake normal from creating black micro-shadows)
					f3Lobe2Diffuse = lerp(f3Lobe2Diffuse, float3(0.0f, 0.0f, 0.0f), f1CarPaintMask);

					// Flake mask calculation
					float flakeMask = smoothstep(g_f3FlakeParams.x, g_f3FlakeParams.y, rawFlakeMask);
					float3 flakeTint = lerp(float3(1.0f, 1.0f, 1.0f), paintColor, 0.8f);

					// Flake specular calculation
					float3 flakeSpecular = flakeTint * flakeMask * g_f3FlakeParams.z;
					flakeSpecular = min(flakeSpecular, 0.8f);

					// LOBE 2: Gets ONLY the flake specular
					f3Lobe2Specular = lerp(f3Lobe2Specular, flakeSpecular, f1CarPaintMask);

					f1SecondaryRoughness = lerp(f1SecondaryRoughness, 0.45f, f1CarPaintMask);

					// Mask the flake normals so they don't perturb bare metal
					f3FlakeNormalWS = lerp(f3NormalWS, f3FlakeNormalWS, f1CarPaintMask);
				}

				float f1PearlNdotV = max(0.0f, dot(f3NormalWS, normalize(g_f3CameraPos - f3WorldPos)));
				float f1PearlMask = pow(1.0f - f1PearlNdotV, g_f1PearlTransition) * g_f1PearlBlend * f1CarPaintMask;

				f3Lobe2Diffuse = lerp(f3Lobe2Diffuse, g_f3PearlColor, f1PearlMask);
				f3Lobe2Specular = lerp(f3Lobe2Specular, g_f3PearlColor * f3Lobe2Specular, f1PearlMask);

				// --- HAIR MODE OVERRIDES ---
				if (g_f1HairMode > 0.5f)
				{
					// 1. Wetness Darkening & EnvMap Overrides
					f3DiffuseColor *= lerp(1.0f, 0.4f, g_f1HairGloss);
					flEnvmapMask = lerp(0.01f, 0.08f, g_f1HairGloss);
					f1AmbientOcclusion = 1.0f; // Rely purely on SSAO for microshadows

					// 2. Derive Perturbed Strand Direction using the Normal Map X-channel
					float f1StrandJitter = f3NormalTS.x;
					float f1JitterStrength = lerp(0.15f, 0.02f, g_f1HairGloss);
					f3HairStrandWS = normalize(i.Bitangent + (i.Tangent * f1StrandJitter * f1JitterStrength));

					// 3. Nullify GGX Variables (Direct light is handled by Kajiya-Kay later)
					float hairF0 = lerp(0.01f, 0.08f, g_f1HairGloss);
					f3Lobe1Specular = float3(hairF0, hairF0, hairF0);
					f3Lobe2Specular = float3(0.0f, 0.0f, 0.0f);
					f3Lobe2Diffuse = float3(0.0f, 0.0f, 0.0f);
				}

				#if EMISSIVE
					float3 f3Emission = tex2D(Sampler_EmissionTexture, f2TexCoord).xyz * g_f1EmissiveFactor;
				#endif

				#if SUBSURFACESCATTERING
					float1 f1Thickness = tex2D(Sampler_ThicknessTexture, f2TexCoord).r;
				#endif

				f3ProjPos.y *= -1.0f;
				f3ProjPos.xy = f3ProjPos.xy * 0.5f + 0.5f;
				float f1SSAO = tex2Dlod(Sampler_SSAO, float4(f3ProjPos.xy, 0.0f, 0.0f)).r;
				f1AmbientOcclusion = min(f1AmbientOcclusion, f1SSAO);

				f3ViewDir = normalize(f3ViewDir);
				float f1NdotV = max(0.05f, dot(f3NormalWS, f3ViewDir));

				// --- ENVMAP TRANSLATION SPOOF ---
				float3 f3EnvWorldPos = f3WorldPos + cEnvmapOffset;
				float3 f3EnvViewDir = normalize(g_f3CameraPos - f3EnvWorldPos);
				float f1EnvNdotV = max(0.0f, dot(f3NormalWS, f3EnvViewDir));
				float3 f3Reflect = reflect(-f3EnvViewDir, f3NormalWS);
				// --------------------------------

				//==================================================================================================
				// Indirect Lighting
				//==================================================================================================

				float3 f3IndirectLighting = 0.0;

				#if !FLASHLIGHT
				{
					float3 f3DiffuseLighting = ambientLookup(f3NormalWS, EnvAmbientCube, f3NormalTS, i.LightmapTexCoord1And2, i.LightmapTexCoord3, Sampler_Lightmap, cDiffuseModulation);
					float3 f3AmbientLightingFresnelTerm = fresnelSchlickRoughness(f3Lobe1Specular, f1NdotV, f1Roughness);

					#if SPECULAR
						float3 f3DiffuseContributionFactor = 1.0f - f3AmbientLightingFresnelTerm;
					#else
						float3 f3DiffuseContributionFactor = lerp(1.0f - f3AmbientLightingFresnelTerm, 0.0f, f3Lobe1Specular);
					#endif

					float3 f3AmbientDiffuse = f3DiffuseContributionFactor * f3Lobe2Diffuse * f3DiffuseLighting;

					float4 f4ReflectUV = float4(f3Reflect, f1Roughness * g_f1EnvMapMips);
					float3 f3IndirectSpecular = ENV_MAP_SCALE * texCUBElod(Sampler_Envmap, f4ReflectUV).rgb;
					f3IndirectSpecular *= EnvBRDFApprox(f3Lobe1Specular, f1Roughness, f1EnvNdotV);

				#if DUALLOBE
					float f1Lobe2NdotV = max(0.001f, dot(f3FlakeNormalWS, f3EnvViewDir));
					float3 f3Lobe2Reflect = reflect(-f3EnvViewDir, f3FlakeNormalWS);
					float4 f4Lobe2ReflectUV = float4(f3Lobe2Reflect, f1SecondaryRoughness * g_f1EnvMapMips);
					float3 f3Lobe2EnvMap = ENV_MAP_SCALE * texCUBElod(Sampler_Envmap, f4Lobe2ReflectUV).rgb;

					if (g_f1CarPaintMode > 0.5f) {
						// Apply your contrast settings to the physical flakes
						float flakeMask = smoothstep(g_f3FlakeParams.x, g_f3FlakeParams.y, rawFlakeMask);

						// Fix grazing dampen so it doesn't crush the straight-on view to 30%.
						// (1.0 = center brightness, 1.5 = boosted edge brightness)
						float grazingDampen = lerp(1.0f, 1.5f, pow(1.0f - f1Lobe2NdotV, 5.0f));

						// EnvBRDFApprox now inherently handles the flake intensity, so we just apply the dampen.
						f3Lobe2EnvMap = f3Lobe2EnvMap * grazingDampen;
						f3Lobe2EnvMap *= EnvBRDFApprox(f3Lobe2Specular, f1SecondaryRoughness, f1Lobe2NdotV);

						// Multiply by the mask to prevent doubling metal reflections
						f3IndirectSpecular += (f3Lobe2EnvMap * f1CarPaintMask);
						f3IndirectSpecular = min(f3IndirectSpecular, 3.0f);
					}
				#endif

				#if PLANARREFLECTION
					float2 screenUV = f2TexCoord;
					screenUV += f3NormalWS.xy * 0.05f;
					float2 blurSpread = g_PlanarBlurScale.xy * f1Roughness * 0.005f;
					float3 f3PlanarColor = 0.0f;

					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV - blurSpread * 4.0).rgb * 0.00390625f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV - blurSpread * 3.0).rgb * 0.03125f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV - blurSpread * 2.0).rgb * 0.109375f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV - blurSpread * 1.0).rgb * 0.21875f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV).rgb * 0.2734375f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV + blurSpread * 1.0).rgb * 0.21875f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV + blurSpread * 2.0).rgb * 0.109375f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV + blurSpread * 3.0).rgb * 0.03125f;
					f3PlanarColor += tex2D(Sampler_PlanarReflection, screenUV + blurSpread * 4.0).rgb * 0.00390625f;

					f3IndirectSpecular = f3PlanarColor * EnvBRDFApprox(f3Lobe1Specular, f1Roughness, f1EnvNdotV);
				#endif

					// ---> OUR NEW MASK LINE GOES RIGHT HERE <---
					f3IndirectSpecular *= flEnvmapMask;

					float flBaseVisibility = saturate(1.0f - g_flEnvDlightFactor);
					f3IndirectLighting = (f3AmbientDiffuse + f3IndirectSpecular) * f1AmbientOcclusion * flBaseVisibility;
				}
				#endif

				//==================================================================================================
				// Direct Lighting
				//==================================================================================================

				float3 f3DirectLighting = 0.0;

				#if !FLASHLIGHT
				{
					for (uint n = 0; n < NUM_LIGHTS; ++n)
					{
						float3 f3LightDir = normalize(PixelShaderGetLightVector(f3WorldPos, cLightInfo, n));
						float3 f3LightColor = PixelShaderGetLightColor(cLightInfo, n) * GetAttenForLight(i.LightAttenuations, n);

						float f1MicroShadow = ApplyMicroShadow(f1AmbientOcclusion, f3NormalWS, f3LightDir, 1.0f);
						f3LightColor *= lerp(1.0f, f1MicroShadow, g_f1MicroShadowFactor);

						if (g_f1HairMode > 0.5f)
						{
							// Execute Kajiya-Kay Anisotropic Lighting
							float f1PrimaryGloss = lerp(20.0f, 80.0f, g_f1HairGloss);
							float f1SecondaryGloss = lerp(5.0f, 20.0f, g_f1HairGloss);

							float3 f3HairLight = calculateHairLight(f3LightDir, f3LightColor, f3ViewDir, f3HairStrandWS, f3NormalWS, f3DiffuseColor, f1PrimaryGloss, f1SecondaryGloss);

							// Soften highlights on the anti-aliased geometry edges
							f3DirectLighting += (f3HairLight * f4BaseTexture.a);
						}
						else
						{
							// Execute Standard GGX Lighting for all other materials
							float3 f3DirectAndSpecular = calculateLight(f3LightDir, f3LightColor, f3ViewDir,
								f3NormalWS, f3Lobe1Specular, f1Roughness, f1NdotV, f3Lobe1Diffuse, Sampler_Lightwarp);

							#if DUALLOBE
							float3 f3SecondaryDirectAndSpecular = calculateLight(f3LightDir, f3LightColor, f3ViewDir,
								f3FlakeNormalWS, f3Lobe2Specular, f1SecondaryRoughness, f1NdotV, f3Lobe2Diffuse, Sampler_Lightwarp);

							if (g_f1CarPaintMode > 0.5f) {
								f3DirectLighting += f3DirectAndSpecular + f3SecondaryDirectAndSpecular;
						}
							else {
								f3DirectLighting += lerp(f3DirectAndSpecular, f3SecondaryDirectAndSpecular, g_f1DualLobe_LerpFactor);
							}
							#else
							f3DirectLighting += f3DirectAndSpecular;
							#endif
					}

						#if SUBSURFACESCATTERING
						float3 f3SSSContribution = ComputeSubsurfaceScattering(f3NormalWS, f3LightDir, f3ViewDir,
							f1Thickness.r, g_f3SSSColor, g_f1SSSIntensity, g_f1SSSPower);
						f3DirectLighting += f3SSSContribution * f3LightColor;
						#endif
					}
				}
				#endif

				#if FLASHLIGHT
				{
					float4 flashlightSpacePosition = mul(float4(f3WorldPos, 1.0), xmFlashlightWorldToTexture);
					clip(flashlightSpacePosition.w);
					float3 vProjCoords = flashlightSpacePosition.xyz / flashlightSpacePosition.w;

					float3 delta = g_f3ProjTexPosition - f3WorldPos;
					float distSquared = dot(delta, delta);
					float dist = sqrt(distSquared);

					float3 flashlightColor = tex2D(Sampler_ProjTexCookie, vProjCoords.xy).rgb;
					flashlightColor *= cFlashlightColor.xyz;

					float fAtten = saturate(dot(cFlashlightAttenuationFactors.xyz, float3(1.0, 1.0 / dist, 1.0 / distSquared)));

					float flSafeShadowMask = 1.0f;

					#if FLASHLIGHTSHADOWS
						float flashlightShadow = DoFlashlightShadow(Sampler_ShadowDepth, Sampler_RandRot, vProjCoords, f3ProjPos, FLASHLIGHTDEPTHFILTERMODE, cShadowTweaks, true);
						float flashlightAttenuated = lerp(flashlightShadow, 1.0, cShadowTweaks.y);
						flashlightShadow = saturate(lerp(flashlightAttenuated, flashlightShadow, fAtten));
						flSafeShadowMask = flashlightShadow;

						flashlightColor *= flashlightShadow;
					#endif

					flashlightColor *= fAtten;

					#if UBERLIGHT
						float4 uberLightPosition = mul(float4(f3WorldPos.xyz, 1.0f), xmFlashlightWorldToLight).yzxw;
						flashlightColor *= uberlight(uberLightPosition.xyz, cSmoothEdge0, cSmoothEdge1,
										   cSmoothOneOverWidth, cShearRound.xy, cAABB, cShearRound.zw);
					#endif

					float farZ = cFlashlightAttenuationFactors.w;
					float endFalloffFactor = RemapValClamped(dist, farZ, 0.6 * farZ, 0.0, 1.0);

					float3 flashLightIntensity = flashlightColor * endFalloffFactor;
					float3 flashLightIn = normalize(delta);

					float f1MicroShadow = ApplyMicroShadow(f1AmbientOcclusion, f3NormalWS, flashLightIn, 1.0f);
					flashLightIntensity *= lerp(1.0f, f1MicroShadow, g_f1MicroShadowFactor);

					if (g_f1HairMode > 0.5f)
					{
						// Execute Kajiya-Kay Anisotropic Lighting for Flashlight
						float f1PrimaryGloss = lerp(20.0f, 80.0f, g_f1HairGloss);
						float f1SecondaryGloss = lerp(5.0f, 20.0f, g_f1HairGloss);

						float3 f3HairLight = max(0, calculateHairLight(flashLightIn, flashLightIntensity, f3ViewDir, f3HairStrandWS, f3NormalWS, f3DiffuseColor, f1PrimaryGloss, f1SecondaryGloss));
						f3DirectLighting += (f3HairLight * f4BaseTexture.a);
					}
					else
					{
						// Execute Standard GGX Lighting for Flashlight
						float3 f3DirectAndSpecular = max(0, calculateLight(flashLightIn, flashLightIntensity, f3ViewDir,
							f3NormalWS, f3Lobe1Specular, f1Roughness, f1NdotV, f3Lobe1Diffuse, Sampler_Lightwarp));

						#if DUALLOBE
						float3 f3SecondaryDirectAndSpecular = max(0, calculateLight(flashLightIn, flashLightIntensity, f3ViewDir,
							f3FlakeNormalWS, f3Lobe2Specular, f1SecondaryRoughness, f1NdotV, f3Lobe2Diffuse, Sampler_Lightwarp));

						if (g_f1CarPaintMode > 0.5f) {
							f3DirectLighting += f3DirectAndSpecular + f3SecondaryDirectAndSpecular;
						}
						else {
							f3DirectLighting += lerp(f3DirectAndSpecular, f3SecondaryDirectAndSpecular, g_f1DualLobe_LerpFactor);
						}
						#else
						f3DirectLighting += f3DirectAndSpecular;
						#endif
					}

					#if SUBSURFACESCATTERING
						float3 f3SSSContribution = ComputeSubsurfaceScattering(f3NormalWS, flashLightIn, f3ViewDir,
							f1Thickness, g_f3SSSColor, g_f1SSSIntensity, g_f1SSSPower);
						f3DirectLighting += f3SSSContribution * flashLightIntensity;
					#endif

						// --- Dlight Aware ENVMAP Feature ---
						float4 f4ReflectUV = float4(f3Reflect, f1Roughness * g_f1EnvMapMips);
						float3 f3DynamicEnvMap = ENV_MAP_SCALE * texCUBElod(Sampler_Envmap_Flashlight, f4ReflectUV).rgb;
						f3DynamicEnvMap *= EnvBRDFApprox(f3Lobe1Specular, f1Roughness, f1EnvNdotV);
						
					#if DUALLOBE
						float f1Lobe2NdotV = max(0.001f, dot(f3FlakeNormalWS, f3EnvViewDir));
						float3 f3Lobe2Reflect = reflect(-f3EnvViewDir, f3FlakeNormalWS);
						float4 f4Lobe2ReflectUV = float4(f3Lobe2Reflect, f1SecondaryRoughness * g_f1EnvMapMips);
						float3 f3Lobe2DynamicEnvMap = ENV_MAP_SCALE * texCUBElod(Sampler_Envmap_Flashlight, f4Lobe2ReflectUV).rgb;

						if (g_f1CarPaintMode > 0.5f) {
							// Apply your contrast settings to the physical flakes
							float flakeMask = smoothstep(g_f3FlakeParams.x, g_f3FlakeParams.y, rawFlakeMask);

							// Apply the corrected grazing dampen
							float grazingDampen = lerp(1.0f, 1.5f, pow(1.0f - f1Lobe2NdotV, 5.0f));

							// Rely on EnvBRDFApprox for the flake mask and intensity
							f3Lobe2DynamicEnvMap = f3Lobe2DynamicEnvMap * grazingDampen;
							f3Lobe2DynamicEnvMap *= EnvBRDFApprox(f3Lobe2Specular, f1SecondaryRoughness, f1Lobe2NdotV);

							f3DynamicEnvMap += (f3Lobe2DynamicEnvMap * f1CarPaintMask);
							f3DynamicEnvMap = min(f3DynamicEnvMap, 10.0f);
						}
					#endif

						float f1NdotL = saturate(dot(f3NormalWS, flashLightIn));
						float flLightLuminance = dot(flashLightIntensity, float3(0.299f, 0.587f, 0.114f));
						float flDynamicMask = saturate(flLightLuminance * f1NdotL);

						f3DynamicEnvMap *= (flDynamicMask * g_flEnvDlightFactor * f1AmbientOcclusion);

						// Apply the metalness mask to the flashlight's dynamic environment map
						f3DynamicEnvMap *= flEnvmapMask;

						f3DirectLighting += f3DynamicEnvMap;
						// -----------------------------------
						}
					#endif

					float3 f3CombinedLighting = f3DirectLighting + f3IndirectLighting;

				#if !FLASHLIGHT
					f3CombinedLighting = lerp(f3CombinedLighting, (f3DiffuseColor + f3SpecularColor) * f1AmbientOcclusion, g_f1Fullbright);
				#endif

					float f1FogFactor = CalcPixelFogFactor(PIXELFOGTYPE, cFogParams, g_f3CameraPos, f3WorldPos.xyz, f3ProjPos.z);
					float f1Alpha = f4BaseTexture.a;

					#if !FLASHLIGHT
						#if (WRITEWATERFOGTODESTALPHA && (PIXELFOGTYPE == PIXEL_FOG_TYPE_HEIGHT))
							f1Alpha = f1FogFactor;
						#endif

						bool bWriteDepthToAlpha = (WRITE_DEPTH_TO_DESTALPHA != 0) && (WRITEWATERFOGTODESTALPHA == 0);

						#if (EMISSIVE && !FLASHLIGHT)
							f3CombinedLighting += f3Emission;
						#endif
					#endif

					return FinalOutput(float4(f3CombinedLighting, f1Alpha), f1FogFactor, PIXELFOGTYPE, TONEMAP_SCALE_LINEAR, bWriteDepthToAlpha, f3ProjPos.z);
}