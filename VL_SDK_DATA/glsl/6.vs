// #SHADER <6> specular light separate + fog
void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vec3 N = normalize(gl_NormalMatrix * gl_Normal);
	vec3 V = gl_ModelViewMatrix * gl_Vertex;
	vec3 L = normalize(gl_LightSource[0].position - V.xyz);
	vec3 H = normalize(L + vec3(0.0,0.0,1.0));

	// compute diffuse equation
	float NdotL = dot(N,L);
	vec4 diffuse = gl_Color * vec4(max(0.0,NdotL));

	float NdotH = max(0.0, dot(N,H));
	vec4 specular = vec4(0.0);
	const float specularExp = 128.0;
	if (NdotL > 0.0)
	  specular = vec4(pow(NdotH, specularExp));

	// fog
	const float e = 2.71828;
	const float density = 0.03;
	float fogFactor = density * length(V);
	fogFactor *= fogFactor;
	fogFactor = clamp(pow(e, -fogFactor), 0.0, 1.0);
	const vec4 fogColor = vec4(0.5,1.0,0.5,1.0);
	gl_FrontColor = mix(fogColor, clamp(diffuse+specular,0.0,1.0), fogFactor);
}
