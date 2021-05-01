#version 330 core

in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skyboxDay;
uniform samplerCube skyboxNight;
uniform float time;

void main()
{
	FragColor = mix(texture(skyboxDay, TexCoords), texture(skyboxNight, TexCoords), time);
}