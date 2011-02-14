#define LIGHTING_ALPHA_THRESHOLD 0.1

varying vec3 frag_position;  // in object space

uniform sampler3D volume_texunit;
uniform sampler1D trfunc_texunit;
uniform float trfunc_delta;
uniform vec3 eye_position;      // camera position in object space
uniform float sample_step;      // step used to advance the sampling ray
uniform float val_threshold;

void main(void)
{
	vec3 ray_dir = normalize(frag_position - eye_position);
	vec3 ray_pos = gl_TexCoord[0].xyz; // the current ray position
	vec3 pos111 = vec3(1.0, 1.0, 1.0);
	vec3 pos000 = vec3(0.0, 0.0, 0.0);
	
	float max_val = 0.0;
	vec3 prev_pos = ray_pos;
	do
	{
		// mic fixme: 
		// - adjust ray to deal with non-cubic volumes.
		prev_pos = ray_pos;
		ray_pos += ray_dir * sample_step;

		// break out if ray reached the end of the cube.
		if (any(greaterThan(ray_pos,pos111)))
			break;

		if (any(lessThan(ray_pos,pos000)))
			break;

		max_val = max(max_val, texture3D(volume_texunit, ray_pos).r);
	}
	while(true);

	if (max_val >= val_threshold)
	{
		// to properly sample the texture clamp bewteen trfunc_delta...1.0-trfunc_delta
		float clamped_val = trfunc_delta+(1.0-2.0*trfunc_delta)*max_val;
		gl_FragColor = texture1D(trfunc_texunit, clamped_val);
	}
	else
	{
		discard;
	}
}

// Have fun!