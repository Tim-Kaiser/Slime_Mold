#version 460
in vec2 vUv;
in vec3 vertexOut;

uniform sampler2D particleMap;
uniform sampler2D trailMap;
out vec4 fragCol;



void main(){
	vec4 particleCol = texture(particleMap, vUv);
	vec4 trailCol = texture(trailMap, vUv);

	vec4 col = particleCol + trailCol;

	fragCol = clamp(vec4(0.), col, vec4(1.));
	//fragCol = particleCol;
}