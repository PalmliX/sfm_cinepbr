### This repo started as a means to host a compiled version of @WhiteRedDragons fork of @ficool2 's SFM PBR Shader, however over time I couldn't resist vibe coding a bunch of extra features and enough changed that it became it's own thing and so it's now known as sfm_cinepbr, a PBR shader focused on high-end "cinematic" rendering features vs more cartoon type features.

## Fixes and new Features compared to Ficools' original workshop version
#### SSS *(Sub-Surface Scattering)*
#### Working POM *(Parallax Occlusion Mapping)*
#### Dual-Lobe Specular *(Also known as micro roughness, basically can make skin and similar materials look more realistic by blending two types of highlights together in the same material)*
#### SSAO now appears correctly
#### $nocull materials are now lit properly i.e. they don't become lit from behind.
#### Many additional material settings for things like normal map intensity, channel inverting, metallic/roughness bias, exponent and more!
## Fixes and new Features compared to @WhiteRedDragons fork
#### Car Paint mode with included paint speckle normal map.
#### Hair shading mode based on the Kajiya-Kay rendering model.
#### Render passes, such as material id, camera depth, world depth, world normals, UV, world position, as well as "holdout masks" options for translucency and emissives.
#### Overhauled envmaps to be more controllable within this faux PBR metalness/roughness workflow. Different options for masking the envmaps based on SFM dynamic lights (no more glow in the dark envmaps), and/or metalness only or roughness only. 
#### Alphatested materials are no longer transparent when SSAO is also enabled on the material. 
#### rt_camera support which is affected by normal and roughness maps, and masked by metalness. It replaces envmap reflections.
#### Envmap translation offset material commands, which allows animation of the envmap position in x,y,z in SFM to simulate movement, for example a car driving down a tunnel with the reflection of lights moving across the car surface.
#### *ATTENTION: These changes were all coded by AI*

# Install
## *You MUST be subscribed to Ficool2's Shader Loader: https://steamcommunity.com/sharedfiles/filedetails/?id=3669466136 You can still be subscribed to other PBR shaders since this uses a new shader name in the .vmt (Cinepbr)
#### 1) Download the latest zip from the releases page and extract it into your SFM usermod folder so you have usermod/addons and usermod/shaders when finished.
#### 2) Make sure the shader name used in the first line of your .vmt is "CinePBR"
#### 3) Run SFM and try one of the new shader features/parameters in your vmt file to see if it's working. 

### Metallic/Roughness vs Diffuse/Specular Workflow

This shader has removed the Diffuse/Specular Workflow option in favor of a different approach which is 100% Metallic/Roughness only.

## 🛠️ VMT Implementation

The shader name in the top line of the .vmt must be "CinePBR"

# VMT Parameters
### Texture Maps *(path to VTF)*
#### $BaseTexture - *Albedo/color in RGB and transparency mask in alpha.*
#### $BumpMap - *Normal map in RGB and height map for POM in alpha. Channels can be inverted via VMT command so no need to worry about normal map format (i.e. Green+-)*
#### $MRAOTexture - *Metalness in R, Roughness in G and Oclussion in B. If you don't have an oclussion map you can either use the blue channel of the normal map or fill it with white.*
#### $EmissionTexture - *Emissive texture; is a color texture, not a mask*
#### $ThicknessTexture - *Thickness texture used for SSS. Grayscale, black is thin, i.e. most amount of SSS glow, white is thick, i.e. least amount of SSS.*
#### $LightWarpTexture - *Not tested, I assume it works the same as the standard light warp*
#### $EnvMap - *Path to cubemap, leave same as original shader unless you want to change it of course*
#### $Compress - *Compression wrinklemap*
#### $BumpCompress - *Stretch bumpmap*
#### $Stretch - *Stretch wrinklemap*
#### $BumpStretch - *Compression bumpmap*

### --- Kajiya-Kay Hair shading ---
#### $Hair, SHADER_PARAM_TYPE_BOOL, "0", "Enable Hair Mode")
#### $HairGloss, SHADER_PARAM_TYPE_VEC3, "[1.0 1.0 1.0]", "Gloss level, Direct Angle Brightness, Grazing Angle Brightness")

### --- CAR PAINT & PEARLESCENCE ---
#### $CarPaint, SHADER_PARAM_TYPE_BOOL, "0", "Enable Car Paint Mode")
#### $CarPaintGlossFactor, SHADER_PARAM_TYPE_FLOAT, "1.0", "Glossiness of the clearcoat")
#### $CarPaintColor, SHADER_PARAM_TYPE_COLOR, "[0.5 0.5 0.5]", "Base color for Car Paint")
#### $CarPaintFlakeTexture, SHADER_PARAM_TYPE_TEXTURE, "models/carpaint/shared_flakes_normal", "")
#### $FlakeContrast, SHADER_PARAM_TYPE_FLOAT, "2.0", "Contrast curve for the metallic flakes")
#### $FlakeScale, SHADER_PARAM_TYPE_FLOAT, "50.0", "Scale of the metallic flakes")
#### $PearlColor, SHADER_PARAM_TYPE_COLOR, "[0 0 0]", "The grazing angle color for pearlescence")
#### $PearlTransition, SHADER_PARAM_TYPE_FLOAT, "2.0", "How sharply the pearl color blends in (1.0 to 5.0)")
#### $PearlBlendAmount, SHADER_PARAM_TYPE_FLOAT, "0.0", "Opacity of the pearl effect (0.0 to 1.0)")

### --- PLANAR REFLECTIONS ---
#### $PlanarReflection, SHADER_PARAM_TYPE_BOOL, "0", "Enable Planar Reflections")
#### $PlanarReflectionTexture, SHADER_PARAM_TYPE_TEXTURE, "_rt_camera", "Texture for planar reflection")
#### $PlanarReflectionBlurScale, SHADER_PARAM_TYPE_VEC2, "[1.0 1.0]", "X and Y Blur Scale for Planar Reflections")

### --- ENVMAP SPOOFING ---
#### $EnvmapOffsetX, SHADER_PARAM_TYPE_FLOAT, "0.0", "Offset X for Envmap translation")
#### $EnvmapOffsetY, SHADER_PARAM_TYPE_FLOAT, "0.0", "Offset Y for Envmap translation")
#### $EnvmapOffsetZ, SHADER_PARAM_TYPE_FLOAT, "0.0", "Offset Z for Envmap translation")


## Adjustment Parameters

#### $EnvDlightFactor - *New envmap feature. Float, 0 is off, i.e. old behavior with glow in the dark envmaps. 1 is 100% masked by SFM lights, or any blend in-between. Any values larger than 1 act as an  overdrive to increase envmap brightness but still 100% masked by SFM lights. I've used values around 10 or 15 for void maps for a dim cubemap*
#### $Model - *Boolean, must be set to 1 for models*
#### $AlphaTest - *Boolean, same as VertexLitGeneric - Enable or disable hard edge alpha transparency. Transparency mask must be in the Alpha channel of the basetexture*
#### $NormalMap_FlipR - *Boolean, inverts Normal Map Red Channel*
#### $NormalMap_FlipG - *Boolean, inverts Normal Map Green Channel*
#### $NormalMap_FlipB - *Boolean, inverts Normal Map Blue Channel*
#### $NormalMapFactor - *Float, Normal Map Intensity (yay!)*

##### *What follows is more or less a code dump with a little bit of cleanup, haven't had time to go through all of it yet but figured I should at least make it available as a quick resource. Will update later*
#### AlphaTestReference,		SHADER_PARAM_TYPE_FLOAT, "0" (required for clean alpha when using SSAO)
#### BumpFrame					SHADER_PARAM_TYPE_INTEGER, "0" "Frame number for $bumpmap")
#### UseEnvAmbient				SHADER_PARAM_TYPE_BOOL, "0" 
#### "Use the cubemaps to compute ambient light."
#### Parallax					SHADER_PARAM_TYPE_BOOL, "0" 
#### "Use Parallax Occlusion Mapping."
#### ParallaxDepth				SHADER_PARAM_TYPE_FLOAT, "0.0030", "Depth of the Parallax Map")
#### ParallaxCenter			SHADER_PARAM_TYPE_FLOAT, "0.5" 
#### "Center depth of the Parallax Map"
#### EmissiveFactor			SHADER_PARAM_TYPE_FLOAT, "1.0"
#### "Emissive factor"
#### SSSColor					SHADER_PARAM_TYPE_COLOR, "[1 1 1]" "Subsurface scattering color"
#### SSSIntensity				SHADER_PARAM_TYPE_FLOAT, "1.0" "SSS intensity")
#### SSSPowerScale				SHADER_PARAM_TYPE_FLOAT, "1.0"
#### "SSS power scale"
#### MRAOMultiplier - Vector 3 - "[1 1 1]"
These three parameters operate together to form a complete, real-time color-correction and level-adjustment system directly inside the shader. They allow you to tweak your Metalness, Roughness, and Ambient Occlusion values mathematically without ever having to re-open Photoshop or re-export your textures.
If you look at how the texture is unpacked in stdshaders/pbr_main.h, all three vectors are combined into a single master equation:
Final Value = saturate( MRAOMultiplier * pow(TextureValue, MRAOExponent) + MRAOBias )
#### MRAOBias -  Vector 3 - "[0 0 0]"
$MRAOBias (The Offset / Black Level)
This is applied last. It is a flat addition or subtraction across the entire texture, effectively moving the "Black Point" up or down.
Value of 0.0: No offset.
Value of 0.2: Adds 20% brightness to every single pixel. Pure black becomes dark gray.
Value of -0.2: Subtracts 20% brightness, crushing the darkest parts of your texture into pure black.
#### MRAOExponent - Vector 3 - "[1 1 1]"
(Gamma Curve)
This is the pow() part of the math, and it is applied to your texture first. It acts exactly like a Gamma adjustment.
Value of 1.0: The texture remains linear. No change.
Value > 1.0 (e.g., 2.0): Darkens the midtones. The dark areas stay dark, the bright areas stay bright, but everything in between gets pushed lower. This increases contrast.
Value < 1.0 (e.g., 0.5): Brightens the midtones. This decreases contrast and washes the map out slightly.
#### MicroShadowBias			SHADER_PARAM_TYPE_FLOAT 
#### (controls SSAO intensity)
#### DualLobe					SHADER_PARAM_TYPE_BOOL, ""
#### DualLobe_RoughnessBias	SHADER_PARAM_TYPE_FLOAT "-0.2"
#### DualLobe_LerpFactor		SHADER_PARAM_TYPE_FLOAT "0.5"

# SFM PBR

Plugin that adds a true PBR shader to Source Filmmaker, based on the PBR shader made by the [Zombie Master: Reborn](https://github.com/zm-reborn) team.

Check out the [Steam Workshop page](https://steamcommunity.com/sharedfiles/filedetails/?id=3671463307)!

Compared to ZMR's implementation, there is additional fixes for SFM compatibility, as well as new additions such as MRAO factor parameters.

This repository is a stripped version of the [Alien Swarm SDK](https://github.com/Nican/swarm-sdk).

## Building
	
To build the plugin, open the .sln in Visual Studio 2022 or newer and build. Place the compiled DLL into SFM's `addons` folder.

To build the shaders, run the `buildsfmshaders.bat` in `src/materialsystem/stdshaders`. Place the compiled FXC files into SFM's `shaders/fxc/` folder.
