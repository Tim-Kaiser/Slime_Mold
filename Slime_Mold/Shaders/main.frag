#version 460
in vec2 vUv;
in vec3 vertexOut;

uniform sampler2D particleMap;
uniform sampler2D trailMap;
out vec4 fragCol;


#define diffuse 0.2

void main(){
	vec4 particleCol = texture(particleMap, vUv);
	vec4 trailCol = texture(trailMap, vUv) * vec4(1.0, 0.,0.,1.);



	fragCol = max(particleCol, (trailCol));
}