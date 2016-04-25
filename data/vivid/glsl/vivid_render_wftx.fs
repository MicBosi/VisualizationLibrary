#define LIGHTING_ALPHA_THRESHOLD 0.02

varying vec3 frag_position;     
uniform sampler3D volume_texunit;
uniform sampler3D gradient_texunit;
uniform sampler1D trfunc_texunit;
uniform float trfunc_delta;
uniform vec3 light_position[4]; 
uniform bool light_enable[4];   
uniform vec3 eye_position;      
uniform float val_threshold;
uniform vec3 gradient_delta; 
uniform bool precomputed_gradient; 

vec3 blinn_phong(vec3 N, vec3 V, vec3 L, int light)
{

	vec3 Ka = vec3(1.0, 1.0, 1.0);
	vec3 Kd = vec3(1.0, 1.0, 1.0);
	vec3 Ks = vec3(1.0, 1.0, 1.0);
	float shininess = 50.0;

	float diff_coeff = max(dot(L,N),0.0);

	vec3 H = normalize(L+V);
	float spec_coeff = pow(max(dot(H,N), 0.0), shininess);
	if (diff_coeff <= 0.0) 
		spec_coeff = 0.0;

	return  Ka * gl_LightSource[light].ambient.rgb + 
			Kd * gl_LightSource[light].diffuse.rgb  * diff_coeff + 
			Ks * gl_LightSource[light].specular.rgb * spec_coeff ;
}

void main(void)
{

	float val = texture3D(volume_texunit, gl_TexCoord[0].xyz ).r;

	if (val < val_threshold)
		discard;

	float clamped_val = trfunc_delta+(1.0-2.0*trfunc_delta)*val;
	vec4 color = texture1D(trfunc_texunit, clamped_val);
	vec3 color_tmp = vec3(0.0, 0.0, 0.0);

	if (color.a > LIGHTING_ALPHA_THRESHOLD)
	{
		vec3 N;
		if (precomputed_gradient)
		{
			
			N  = normalize( (texture3D(gradient_texunit, gl_TexCoord[0].xyz).xyz - vec3(0.5,0.5,0.5))*2.0 );
		}
		else
		{
			
			vec3 sample1, sample2;
			sample1.x = texture3D(volume_texunit, gl_TexCoord[0].xyz-vec3(gradient_delta.x,0.0,0.0) ).r;
			sample2.x = texture3D(volume_texunit, gl_TexCoord[0].xyz+vec3(gradient_delta.x,0.0,0.0) ).r;
			sample1.y = texture3D(volume_texunit, gl_TexCoord[0].xyz-vec3(0.0,gradient_delta.y,0.0) ).r;
			sample2.y = texture3D(volume_texunit, gl_TexCoord[0].xyz+vec3(0.0,gradient_delta.y,0.0) ).r;
			sample1.z = texture3D(volume_texunit, gl_TexCoord[0].xyz-vec3(0.0,0.0,gradient_delta.z) ).r;
			sample2.z = texture3D(volume_texunit, gl_TexCoord[0].xyz+vec3(0.0,0.0,gradient_delta.z) ).r;
			N  = normalize( sample1 - sample2 );
		}

		vec3 V  = normalize(eye_position - frag_position);
		for(int i=0; i<4; ++i)
		{
			if (light_enable[i])
			{
				vec3 L = normalize(light_position[i] - frag_position);
				color_tmp.rgb += color.rgb * blinn_phong(N,V,L,i);
			}
		}
	}
	else
		color_tmp = color.rgb;

	gl_FragColor = vec4(color_tmp,color.a);
}
