// color control raycast example

varying vec3 frag_position;  // in object space

uniform sampler3D volume_texunit;
uniform sampler1D trfunc_texunit;
uniform float trfunc_delta;
uniform vec3 eye_position;      // camera position in object space
uniform vec3 eye_look;          // camera look direction in object space
uniform float sample_step;      // step used to advance the sampling ray
uniform float val_threshold;

void main(void)
{
	const float brightness = 10;
	vec3 ray_dir = -normalize(frag_position - eye_position);
	vec3 ray_pos = gl_TexCoord[0].xyz; // the current ray position
	vec3 pos111 = vec3(1.0, 1.0, 1.0);
	vec3 pos000 = vec3(0.0, 0.0, 0.0);

	vec4 frag_color = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 color;
	do
	{
		// note: 
		// - ray_dir * sample_step can be precomputed
		// - adjust ray to deal with non-cubic volumes.

		ray_pos += ray_dir * sample_step;

		// break out if ray reached the end of the cube.
		if (any(greaterThan(ray_pos,pos111)))
			break;

		if (any(lessThan(ray_pos,pos000)))
			break;

		float density = texture3D(volume_texunit, ray_pos).r;

		color.rgb = texture1D(trfunc_texunit, val_threshold + density).rgb;
		color.a   = density * sample_step * brightness;
		frag_color.rgb = frag_color.rgb * (1.0 - color.a) + color.rgb * color.a;
		
		// mic fixme: 
		// the color accumulation should be mitigated according to the sample step.
	}
	while(true);

	if (frag_color == vec4(0.0,0.0,0.0,0.0))
		discard;
	else
		gl_FragColor = vec4(frag_color.rgb,1.0);
}

// Have fun!