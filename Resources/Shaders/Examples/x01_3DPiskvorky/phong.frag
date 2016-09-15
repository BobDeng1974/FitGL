#version 430

in vec3 position;
in vec3 normal;
in vec2 tc;

uniform vec3 La = vec3(0.2);
uniform vec3 Ld = vec3(0.8);
uniform vec3 lightPosition = vec3(50,60,70);
uniform uint pick=-1;;
uniform uint id=0;

uniform vec4 diffuse = vec4(1);
layout(binding=0)uniform sampler2D diffuseTex;

out vec4 fragColor;

void main(){
    vec3 color = texture(diffuseTex,tc).xyz+diffuse.xyz;


    vec3 L = normalize(lightPosition-position);
    vec3 A = La*color;
    vec3 D = Ld*max(dot(L,normal),0)*color;

	vec3 H = vec3(0);
	if(id==pick) H=vec3(0,0.5,0);
    fragColor = vec4(A+D+H,diffuse.a);
}
