#version 150

in vec4 vPosition;
in vec4	Color;
in vec3	Normal;
in vec2	TexCoord0;

out vec4 			oColor;

void main()
{
	vec2 uv = TexCoord0;
	vec3 L = normalize(-vPosition.xyz);
	vec3 E = normalize(-vPosition.xyz);
	vec3 R = normalize(-reflect(L, Normal));

	// ambient term
	vec4 ambient = vec4(0.0, 0.0, 0.0, 1.0);

	// diffuse term with fake ambient occlusion
	float occlusion = 0.5 + 0.5*16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y);
	vec4 diffuse = Color * occlusion;
	diffuse *= max(dot(Normal,L), 0.0);
	diffuse = clamp(diffuse, 0.0, 1.0);

	// specular term
	vec4 specular = diffuse;
	specular *= pow(max(dot(R,E),0.0), 50.0);
	specular = clamp(specular, 0.0, 1.0);

	// write gamma corrected final color
	vec3 final = sqrt(ambient + diffuse + specular).rgb;
	oColor.rgb = final;

	// write luminance in alpha channel (required for optimal FXAA)
	const vec3 luminance = vec3(0.299, 0.587, 0.114);
	oColor.a = dot( final, luminance );
}