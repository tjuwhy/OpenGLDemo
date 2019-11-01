#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float fi;

// texture sampler
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;


uniform int index;


void main()
{
	if(index == 0){	
		FragColor = texture(texture1, TexCoord);
	} else if(index == 1){
		FragColor = texture(texture2, TexCoord) * fi;
	} else if(index == 2){
		FragColor = texture(texture3, TexCoord) * fi;
	} else if(index == 3){
		FragColor = texture(texture4, TexCoord) * fi;
	} else if(index == 4){
		FragColor = texture(texture5, TexCoord) * fi;
	}
}