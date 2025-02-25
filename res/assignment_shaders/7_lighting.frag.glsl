// This shaders base is from learnopengl.com/PBR/Lighting
#version 450 core

// constants
#define PI 3.14159265359

// layout
layout (location = 0) out vec4 frag_color;

// from vertex shader
in vec2 uv;
in vec3 frag_pos;

// textures
uniform sampler2D diff;
uniform sampler2D norm;
uniform sampler2D disp;

// camera
uniform vec3 camera_position;

// material props
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
#define LIGHT_COUNT 1
uniform vec3 light_positions[LIGHT_COUNT];
uniform vec3 light_colors[LIGHT_COUNT];


float distribution_ggx(vec3 N, vec3 H, float roughness);
float geometry_schlick_ggx(float NdotV, float roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnel_schlick(float cosTheta, vec3 F0);

void main()
{		
    vec3 albedo = texture(diff,uv).xyz;

    // normal from texture
    vec3 N = normalize(texture(norm, uv).xyz * 2.0 - 1.0);
    // direction towards camera
    vec3 V = normalize(camera_position - frag_pos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
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
        float NDF = distribution_ggx(N, H, roughness);        
        float G   = geometry_smith(N, V, L, roughness);      
        vec3 F    = fresnel_schlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   
  
    vec3 ambient = vec3(0.03) * albedo * ao;
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
