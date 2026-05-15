#version 430 core

in vec2 texCoord;
in vec3 camP;
in vec3 Norm;
in vec3 worldPos;

layout (location = 0) out vec4 accum;
layout (location = 1) out float reveal;

layout(std140) uniform material {
   vec3 diffuse;
   float opacity;
   float Metallic;
   float Roughness;
   int hasDiffuse;
   int diffuseHasOpacity;
};

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform sampler2D diffuseTexture;

vec3 lightPos = vec3(0.0, 15.0, 0.0);
vec3 lightPositions[3] = { vec3( 6.0,  8.0,  6.0), vec3(-6.0,  4.0,  8.0), vec3( 4.0,  5.0, -8.0)};
vec3 lightColor = vec3(300.0f);
const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
   //return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main(){
    //fragColor = vec4(0.2f, 0.5f, 0.3f, 1.0f);
    vec3 diffuseColor = diffuse;
    float finalOpacity = opacity;
    if(hasDiffuse != -1)
        diffuseColor = texture(diffuseTexture, texCoord).rgb;
    if(diffuseHasOpacity >= 0)
        finalOpacity = texture(diffuseTexture, texCoord).a;

    if(finalOpacity >= 0.95)
        discard;

//float metallic = min(Metallic, 0.95);
    //float roughness = max(Roughness, 0.05);
    float metallic = Metallic;
    float roughness = Roughness;

    vec3 N = normalize(Norm);
    vec3 V = normalize(camP - worldPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, diffuseColor, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // calculate per-light radiance
    for(int i = 0; i < 3; ++i){
    //vec3 L = normalize(lightPos - worldPos);
        vec3 L = normalize(lightPositions[i] - worldPos);
        vec3 H = normalize(V + L);
       // float distance = length(lightPos - worldPos);
       float distance = length(lightPositions[i] - worldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0, 0.0);
        
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);        

    // add to outgoing radiance Lo
    Lo += (kD * diffuseColor / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    float ao = 1.0f;
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * diffuseColor;
     const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo; 

    float weight = clamp(pow(min(1.0, finalOpacity * 10.0) + 0.01, 3.0) * 1e2 * 
                         pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

    //float weight = finalOpacity * max(0.01, 3000 * pow(1.0 - gl_FragCoord.z, 3.0));

    // store pixel color accumulation
    accum = vec4(color * finalOpacity, finalOpacity) * weight;
   //accum = vec4(0.3, 0.6, 0.2, finalOpacity);

    // store pixel revealage threshold
    reveal = finalOpacity;

    //accum = vec4(1,0,0,1);
    //reveal = 1.0;
}