#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;
out float fi;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoord = texCoord; 
 	vec4 center = model * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	gl_Position = model * vec4(position, 1.0f);
	vec3 v1 = normalize(center.xyz);
	vec3 v2 = normalize(-vec3(gl_Position.x - center.x,gl_Position.y - center.y,gl_Position.z - center.z));
	fi = max(0, dot(v1,v2))*0.95 + 0.05;
    gl_Position = projection * view * gl_Position;
	
}
