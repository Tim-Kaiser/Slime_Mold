#version 460

layout (location=0)in vec3 vertex;
layout (location=1)in vec3 normal;
layout (location=2)in vec2 uv;

out vec2 vUv;
out vec3 vertexOut;

void main(){
	vUv = uv;
	vertexOut = vertex;
	gl_Position = vec4(vertexOut,  1.);
}