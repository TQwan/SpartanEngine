/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// = INCLUDES ======
#include "BRDF.hlsl"
//==================

float4 mainPS(Pixel_PosUv input) : SV_TARGET
{
    float2 uv       = input.uv;
    float3 color    = 0.0f;
    
    // Sample from textures
    float4 sample_normal    = tex_normal.Sample(sampler_point_clamp, uv);
    float4 sample_material  = tex_material.Sample(sampler_point_clamp, uv);
    float3 light_volumetric = tex_lightVolumetric.Sample(sampler_point_clamp, uv).rgb;   
    float depth             = tex_depth.Sample(sampler_point_clamp, uv).r;
    float2 sample_ssr       = tex_ssr.Sample(sampler_point_clamp, uv).xy;
    float4 sample_hbao      = tex_hbao.Sample(sampler_point_clamp, uv);
    float3 camera_to_pixel  = get_view_direction(depth, uv);

    // Post-process samples
    int mat_id = round(sample_normal.a * 65535);
    
    // Volumetric lighting
    color += light_volumetric;
    
    [branch]
    if (mat_id == 0)
    {
        color += tex_environment.Sample(sampler_bilinear_clamp, direction_sphere_uv(camera_to_pixel)).rgb;
        color *= clamp(g_directional_light_intensity / 5.0f, 0.01f, 1.0f);
        return float4(color, 1.0f);
    }
    else
    {
        // Sample from textures
        float4 sample_albedo    = tex_albedo.Sample(sampler_point_clamp, uv);
        float3 light_diffuse    = tex_light_diffuse.Sample(sampler_point_clamp, uv).rgb;
        float3 light_specular   = tex_light_specular.Sample(sampler_point_clamp, uv).rgb;

        // Create material
        Material material;
        material.albedo     = sample_albedo.rgb;
        material.roughness  = sample_material.r;
        material.metallic   = sample_material.g;
        material.emissive   = sample_material.b;
        material.F0         = lerp(0.04f, material.albedo, material.metallic);

        // Light - Image based
        float3 diffuse_energy       = 1.0f;
        float3 reflective_energy    = 1.0f;
        float3 light_ibl_specular   = Brdf_Specular_Ibl(material, sample_normal.xyz, camera_to_pixel, tex_environment, tex_lutIbl, diffuse_energy, reflective_energy);
        float3 light_ibl_diffuse    = Brdf_Diffuse_Ibl(material, sample_normal.xyz, tex_environment) * diffuse_energy; // Tone down diffuse such as that only non metals have it

        // Light - Bounce (diffuse)
        float3 light_bounce = 0.0f;
        #if INDIRECT_BOUNCE
        light_bounce += sample_hbao.rgb * material.albedo;
        #endif
        
        // Light - SSR
        float3 light_reflection = 0.0f;
        [branch]
        if (g_ssr_enabled && all(sample_ssr))
        {
            float fade = 1.0f - material.roughness; // fade with roughness as we don't have blurry screen space reflections yet
            
            // Reflection
            light_reflection = saturate(tex_frame.Sample(sampler_bilinear_clamp, sample_ssr).rgb);
            light_reflection *= fade * reflective_energy;

            // Bounce (specular)
            #if INDIRECT_BOUNCE
            light_bounce += tex_light_specular.Sample(sampler_point_clamp, sample_ssr).rgb * fade;
            #endif
        }
        
        // Light - Emissive
        float3 light_emissive = material.emissive * material.albedo * 50.0f;

        // Light - Ambient
        float3 light_ambient = saturate(g_directional_light_intensity * 0.01f);
		#if INDIRECT_BOUNCE
		light_ambient *= sample_hbao.a;
		#else
		light_ambient *= MultiBounceAO(sample_hbao.a, sample_albedo.rgb);
        #endif
		
        // Modulate with ambient light
        light_reflection    *= light_ambient;
        light_ibl_diffuse   *= light_ambient;
        light_ibl_specular  *= light_ambient;
    
        // Combine and return
        color += light_diffuse + light_ibl_diffuse + light_specular + light_ibl_specular + light_reflection + light_emissive + light_bounce;
        return float4(color, sample_albedo.a);
    }
}