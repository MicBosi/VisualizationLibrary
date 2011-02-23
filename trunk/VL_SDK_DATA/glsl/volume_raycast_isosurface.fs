#define LIGHTING_ALPHA_THRESHOLD 0.1

varying vec3 frag_position;  // in object space

uniform sampler3D volume_texunit;
uniform sampler3D gradient_texunit;
uniform sampler1D trfunc_texunit;
uniform vec3 light_position[4]; // light positions in object space
uniform bool light_enable[4];   // light enable flags
uniform vec3 eye_position;      // camera position in object space
uniform vec3 eye_look;          // camera look direction in object space
uniform float sample_step;      // step used to advance the sampling ray
uniform float val_threshold;
uniform vec3 gradient_delta;    // for on-the-fly gradient computation
uniform bool precomputed_gradient; // whether the gradient has been precomputed or not

// computes a simplified lighting equation
vec3 blinn_phong(vec3 N, vec3 V, vec3 L, int light)
{
	// material properties
	// you might want to put this into a bunch or uniforms
	vec3 Ka = vec3(1.0, 1.0, 1.0);
	vec3 Kd = vec3(1.0, 1.0, 1.0);
	vec3 Ks = vec3(1.0, 1.0, 1.0);
	float shininess = 50.0;

	// diffuse coefficient
	float diff_coeff = max(dot(L,N),0.0);
	
	// specular coefficient
	vec3 H = normalize(L+V);
	float spec_coeff = pow(max(dot(H,N), 0.0), shininess);
	if (diff_coeff <= 0.0) 
		spec_coeff = 0.0;

	// final lighting model
	return  Ka * gl_LightSource[light].ambient.rgb + 
			Kd * gl_LightSource[light].diffuse.rgb  * diff_coeff + 
			Ks * gl_LightSource[light].specular.rgb * spec_coeff ;
}

vec4 computeFragColor(vec3 iso_pos)
{
	// compute lighting at isosurface point
	float val = texture3D(volume_texunit, iso_pos).r;

	vec4 color = texture1D(trfunc_texunit, val);
	vec3 color_tmp;

	// compute the gradient and lighting only if the pixel is visible "enough"
	vec3 N;
	if (precomputed_gradient)
	{
		// retrieve pre-computed gradient
		N  = normalize( (texture3D(gradient_texunit, iso_pos).xyz - vec3(0.5,0.5,0.5))*2.0 );
	}
	else
	{
		// on-the-fly gradient computation: slower but requires less memory (no gradient texture required).
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
	for(int i=0; i<4; ++i)
	{
		if (light_enable[i])
		{
			vec3 L = normalize(light_position[i] - frag_position);
			color_tmp.rgb += color.rgb * blinn_phong(N,V,L,i);
		}
	}

	return vec4(color_tmp,color.a);
}

void main(void)
{
	vec3 ray_dir = normalize(frag_position - eye_position);
	vec3 ray_pos = gl_TexCoord[0].xyz; // the current ray position
	vec3 pos111 = vec3(1.0, 1.0, 1.0);
	vec3 pos000 = vec3(0.0, 0.0, 0.0);
	
	// float adjusted_step = sample_step / dot(ray_dir, eye_look);
	
	float val = texture3D(volume_texunit, gl_TexCoord[0].xyz ).r;
	bool sign_prev = val > val_threshold;
	bool isosurface_found = false;
	vec3 prev_pos = ray_pos;
	vec4 frag_color = vec4(0.0, 0.0, 0.0, 0.0);
	do
	{
		// mic fixme: 
		// - ray_dir * sample_step can be precomputed
		// - adjust sample step based on ray_dir to achieve planar ray-uniformity
		// - adjust ray to deal with non-cubic volumes.
		prev_pos = ray_pos;
		// ray_pos += ray_dir * adjusted_step;
		ray_pos += ray_dir * sample_step;

		// break out if ray reached the end of the cube.
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

// Have fun!