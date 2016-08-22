#version 150

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in vec3		ciNormal;
in vec4		ciColor;

out vec4 vPosition;
out lowp vec4	Color;
out highp vec3	Normal;
out highp vec2	TexCoord0;

void main( void )
{
	vPosition 	= ciModelViewProjection * ciPosition;
    gl_Position	= ciModelViewProjection * ciPosition;
    Color 		= ciColor;
    TexCoord0	= ciTexCoord0;
    Normal		= ciNormalMatrix * ciNormal;
}
