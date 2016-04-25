varying vec3 frag_position;     
uniform sampler3D volume_texunit;
uniform sampler3D gradient_texunit;
uniform sampler1D trfunc_texunit;
uniform vec3 light_position[4]; 
uniform bool light_enable[4];   
uniform vec3 eye_position;      

uniform float sample_step;      
uniform float val_threshold;
uniform vec3 gradient_delta;    
uniform bool precomputed_gradient; 

vec3 blinn(vec3 N, vec3 V, vec3 L, int light)
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

vec4 computeFragColor(vec3 iso_pos)
{

	float val = texture3D(volume_texunit, iso_pos).r;

	vec3 N;
	if (precomputed_gradient)
	{
		
		N  = normalize( (texture3D(gradient_texunit, iso_pos).xyz - vec3(0.5,0.5,0.5))*2.0 );
	}
	else
	{
		
		vec3 sample1, sample2;
		sample1.x = texture3D(volume_texunit, iso_pos-vec3(gradient_delta.x,0.0,0.0) ).r;
		sample2.x = texture3D(volume_texunit, iso_pos+vec3(gradient_delta.x,0.0,0.0) ).r;
		sample1.y = texture3D(volume_texunit, iso_pos-vec3(0.0,gradient_delta.y,0.0) ).r;
		sample2.y = texture3D(volume_texunit, iso_pos+vec3(0.0,gradient_delta.y,0.0) ).r;
		sample1.z = texture3D(volume_texunit, iso_pos-vec3(0.0,0.0,gradient_delta.z) ).r;
		sample2.z = texture3D(volume_texunit, iso_pos+vec3(0.0,0.0,gradient_delta.z) ).r;
		N  = normalize( sample1 - sample2 );
	}

	vec3 V  = normalize(eye_position - frag_position);
	vec4 color = texture1D(trfunc_texunit, val);
	vec3 final_color /*= vec3(0.0, 0.0, 0.0)*/; 
	for( int i=0; i<4; i++ )
	{
		if (light_enable[i])
		{
			vec3 L = normalize(light_position[i] - frag_position);
			
			if (dot(L,N) < 0.0)
				L = -L;
			final_color = final_color + color.rgb * blinn(N, V, L, i);
		}
	}	

	return vec4(final_color, color.a);
}

void main(void)
{

	vec3 ray_dir = normalize(frag_position - eye_position);
	vec3 ray_pos = gl_TexCoord[0].xyz; 
	vec3 pos111 = vec3(1.0, 1.0, 1.0);
	vec3 pos000 = vec3(0.0, 0.0, 0.0);

	float val = texture3D(volume_texunit, gl_TexCoord[0].xyz ).r;
	bool sign_prev = val > val_threshold;
	vec3 prev_pos = ray_pos;
	do
	{
		
		
		

		prev_pos = ray_pos;
		ray_pos += ray_dir * sample_step;

		
		if (any(greaterThan(ray_pos,pos111)))
			break;

		if (any(lessThan(ray_pos,pos000)))
			break;

		val = texture3D(volume_texunit, ray_pos).r;
		bool sign_cur = val > val_threshold;
		if (sign_cur != sign_prev)
		{
			vec3 iso_pos = (prev_pos+ray_pos)*0.5;
			gl_FragColor = computeFragColor(iso_pos);
			return;
		}
	}
	while(true);

	discard;
}
