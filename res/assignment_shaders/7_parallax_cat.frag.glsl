// This shaders base is from learnopengl.com/PBR/Lighting
// Parallax code is from learnopengl.com/Advanced-Lighting/Parallax-Mapping
#version 450 core

// constants
#define PI 3.14159265359

// layout
layout (location = 0) out vec4 frag_color;

// from vertex shader
in vec2 uv;
in vec3 frag_pos;
in mat3 tbn;

// textures
uniform sampler2D diff;
uniform sampler2D norm;
uniform sampler2D disp;

// camera
uniform vec3 camera_position;

// material props
uniform float metallic;
uniform float metallic2;
uniform float roughness;
uniform float roughness2;
uniform float ao;
uniform float ao2;
uniform float height_scale;
uniform float parallax_layers;

// lights
#define LIGHT_COUNT 1
uniform vec3 light_positions[LIGHT_COUNT];
uniform vec3 light_colors[LIGHT_COUNT];


float distribution_ggx(vec3 N, vec3 H, float roughness);
float geometry_schlick_ggx(float NdotV, float roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnel_schlick(float cosTheta, vec3 F0);
vec2 parallax_mapping(vec2 uv, vec3 V);

void main()
{		
    // direction towards camera, view direction
    vec3 V = tbn * normalize(camera_position - frag_pos);
    vec2 puv = parallax_mapping(uv, V);
    if (puv.x > 1.0 || puv.y > 1.0 || puv.x < 0.0 || puv.y < 0.0) discard;

    vec3 albedo = texture(diff,puv).xyz;
    vec3 N = normalize(texture(norm, puv).xyz * 2.0 - 1.0);

    // catagorize
    vec3 d = texture(diff, uv).rgb;
    float val = d.g - d.b;
    float is_grass = step(0.2, val);

    float c_metallic = metallic;
    float c_roughness = roughness;
    float c_ao = ao;
    if (is_grass > 0.5) {
        float c_metallic = metallic2;
        float c_roughness = roughness2;
        float c_ao = ao2;
    }

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, c_metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < LIGHT_COUNT; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(light_positions[i] - frag_pos);
        vec3 H = normalize(V + L);
        float distance    = length(light_positions[i] - frag_pos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = light_colors[i] * attenuation;        
        
        // cook-torrance brdf
        float NDF = distribution_ggx(N, H, c_roughness);        
        float G   = geometry_smith(N, V, L, c_roughness);      
        vec3 F    = fresnel_schlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - c_metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   
  
    vec3 ambient = vec3(0.03) * albedo * c_ao;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    frag_color = vec4(color, 1.0);
}  

float distribution_ggx(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float geometry_schlick_ggx(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometry_schlick_ggx(NdotV, roughness);
    float ggx1  = geometry_schlick_ggx(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec2 parallax_mapping(vec2 uv, vec3 V) {
    float layerDepth = 1.0 / parallax_layers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = V.xy * height_scale; 
    vec2 deltaTexCoords = P / parallax_layers;
    // get initial values
    vec2  currentTexCoords     = uv;
    float currentDepthMapValue = 1.0 - texture(disp, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = 1.0 - texture(disp, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(disp, prevTexCoords).r) - currentLayerDepth + layerDepth;
     
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);


    return currentTexCoords;
}
