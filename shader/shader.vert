OF_GLSL_SHADER_HEADER

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec3 localUp;

in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;

// output to frag shader
out vec3 vnorm;
out float vheight; // y coordinate of height for colour calculation

uniform float time;


void main()
{
     vec4 modifiedPosition = modelViewProjectionMatrix * position;
		gl_Position = modifiedPosition;
	
    vnorm = normalize((modelViewMatrix*vec4(normal.xyz,0)).xyz);

    vheight = dot(position.xyz, localUp);

}
