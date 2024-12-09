OF_GLSL_SHADER_HEADER

uniform vec4 globalColor;

uniform float minHeight;
uniform float maxHeight;

in vec3 vnorm;
in float vheight;

out vec4 outputColor;
 
vec3 getGradientColor(float normalizedHeight) {
    vec3 deepBlue = vec3(0.0, 0.08, 0.4);    // Deep blue for water
    vec3 lightBlue = vec3(0.0, 0.5, 1.0);   // Light blue for shallow water
    vec3 sand = vec3(0.8, 0.7, 0.5);        // Sand color
    vec3 dirt = vec3(0.4, 0.3, 0.2);
    vec3 lightGrass = vec3(0.5, 0.8, 0.2);
    vec3 grass = vec3(0.13, 0.54, 0.13);       // Grass color
    vec3 rock = vec3(0.5, 0.5, 0.5);        // Rock (gray)
    vec3 snow = vec3(1.0, 1.0, 1.0);        // Snow (white)
    
    float waterLevel = 0.1;
    float sandLevel = 0.13;
    float dirtLevel = 0.15;
    float lightGrassLevel = 0.2;
    float grassLevel = 0.25;
    float rockLevel = 0.5;
    float snowLevel = 1.0;

    // Interpolate between colors based on height
   if (normalizedHeight < waterLevel) {
        // Interpolate between deep blue and light blue
        return mix(deepBlue, lightBlue, normalizedHeight / waterLevel);
    } else if (normalizedHeight < sandLevel) {
        // Strict cutoff for sand
        return sand;
    }
    // Smooth gradients for other terrain types
	if (normalizedHeight < dirtLevel) {
		return mix(sand, dirt, (normalizedHeight - sandLevel) / (dirtLevel - sandLevel));
	} else if (normalizedHeight < lightGrassLevel) {
		return mix(dirt, lightGrass, (normalizedHeight - dirtLevel) / (lightGrassLevel - dirtLevel));
	}else if (normalizedHeight < grassLevel) {
        // Interpolate between sand and grass
        return mix(lightGrass, grass, (normalizedHeight - lightGrassLevel) / (grassLevel - lightGrassLevel));
    } else if (normalizedHeight < rockLevel) {
        // Interpolate between grass and rock
        return mix(grass, rock, (normalizedHeight - grassLevel) / (rockLevel - grassLevel));
    } else {
        // Interpolate between rock and snow
        return mix(rock, snow, (normalizedHeight - rockLevel) / (snowLevel - rockLevel));
    }
}


void main()
{
	float normalizedHeight = clamp((vheight - minHeight) / (maxHeight - minHeight), 0.0, 1.0);

	vec3 color = getGradientColor(normalizedHeight);
	vec3 L = normalize(vec3(0.707,0.0,0.707));
	float diff = dot(L,vnorm);

    outputColor = vec4(color * diff + color * 0.2, 1.0);  // Add ambient term

}
