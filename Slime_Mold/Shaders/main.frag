#version 460
in vec2 vUv;
in vec3 vertexOut;

uniform sampler2D tex;
out vec4 fragCol;

void main(){
	vec4 texCol = texture(tex, vUv);
	fragCol = texCol;
}