// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 diffuseColour;
    float4 ambientColour;
    float3 lightPosition;
    float innerConeAngle;
    float3 lightDirection;
    float outerConeAngle;
    float3 attenuation;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};


float4 computeSpotlightEffect(float3 lightPos, float3 lightDir, float3 pixelWorldPos, float3 surfaceNormal, float4 lightDiffuse, float innerAngle,
                               float outerAngle, float3 attenuationFactors)
{
	// Calculate vector from light source to current pixel
    float3 lightToPixelVector = pixelWorldPos - lightPos;
    float pixelDistance = length(lightToPixelVector);
    lightToPixelVector = normalize(lightToPixelVector);
	
	
	// The light intensity decreases = the distance increases
    float distanceFalloff = 1.0f / (attenuationFactors.x + attenuationFactors.y * pixelDistance +
	                                 attenuationFactors.z * pixelDistance * pixelDistance);
	
	// Calculate the angle between light direction and pixel direction
	// Using dot product which gives us the cosine of the angle
    float angleFromCenter = dot(lightToPixelVector, normalize(lightDir));
	
	
	// Between inner and outer: smooth transition
    float coneEffect = smoothstep(outerAngle, innerAngle, angleFromCenter);
	
	// Standard diffuse lighting calculation (Lambertian)
	// N dot L gives us how much the surface faces the light
    float3 directionToLight = -lightToPixelVector;
    float diffuseFactor = saturate(dot(surfaceNormal, directionToLight));
	
	// Combine all lighting factors:
	// Base color * surface angle * cone effect * distance falloff
    float4 outputColour = lightDiffuse * diffuseFactor * coneEffect * distanceFalloff;
	
	// Clamp values to valid color range [0,1]
    return saturate(outputColour);
}


// Calculate lighting intensity based on direction and normal. Combine with light colour.
//float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
//{
	//float intensity = saturate(dot(normal, lightDirection));
    //float4 colour = ambientColour + saturate(diffuse * intensity);
    //return colour;
//}

float4 main(InputType input) : SV_TARGET
{
    float4 texColor; // Color sampled from texture
    float4 spotlightColor; // Calculated spotlight contribution
    float4 baseAmbient; // Ambient lighting (always present)
    float4 finalOutput; // Final pixel color to output

    //texture at the current UV coordinates
    texColor = texture0.Sample(sampler0, input.tex);
   
    //Set base ambient lighting
    baseAmbient = ambientColour;
	 
    //Calculate spotlight effect for this pixel
    spotlightColor = computeSpotlightEffect(lightPosition, lightDirection, input.worldPosition, input.normal, diffuseColour, innerConeAngle,
	                                         outerConeAngle, attenuation);

	
    finalOutput = saturate(baseAmbient + spotlightColor) * texColor;
    return finalOutput;
}



