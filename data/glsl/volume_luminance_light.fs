// This shader maps the volume value to the transfer function plus computes the
// gradient and lighting on the fly. This shader is to be used with IF_LUMINANCE
// image volumes.

#define ALPHA_THRESHOLD 0.1

varying vec3 position;  // in object space

uniform sampler3D volume_texunit;
uniform sampler1D trfunc_texunit;
uniform float     trfunc_delta;
uniform vec3      light_position; // in object space
uniform vec3      eye_position;   // in object space
uniform float     val_threshold;
uniform vec3      gradient_delta;

vec3 blinn_phong(vec3 N, vec3 V, vec3 L)
{
	// you might want to put this into a bunch or uniforms
	vec3 Ka = vec3(0.2,0.2,0.2); // ambient
	vec3 Kd = vec3(0.8,0.8,0.8); // diffuse
	vec3 Ks = vec3(0.2,0.2,0.2); // specular
	float n = 100.0; // shininess

	vec3 lightColor   = vec3(1.0, 1.0, 1.0);
	vec3 ambientLight = vec3(1.0, 1.0, 1.0);
	
	vec3 H = normalize(L+V);

	vec3 ambient = Ka * ambientLight;
	
	float diffuseLight = max(dot(L,N),0.0);
	vec3 diffuse = Kd * lightColor * diffuseLight;
	
	float specularLight = pow(max(dot(H,N), 0.0), n);
	if (diffuseLight <= 0.0) specularLight = 0.0;
	vec3 specular = Ks * lightColor * specularLight;
	
	return ambient + diffuse + specular;
}

void main(void)
{
	// sample the LUMINANCE value
	
	float val = texture3D(volume_texunit, gl_TexCoord[0].xyz ).x;
	
	// all the pixels whose val is less than val_threshold are discarded
	if (val < val_threshold)
		discard;
	
	// sample the transfer function
	
	// to properly sample the texture clamp bewteen trfunc_delta...1.0-trfunc_delta
	float clamped_val = trfunc_delta+(1.0-2.0*trfunc_delta)*val;
	vec4 rgba = texture1D(trfunc_texunit, clamped_val );

	// compute the gradient only if the pixel is visible "enough"
	if (rgba.a > ALPHA_THRESHOLD)
	{
		vec3 sample1, sample2;
		// gradient computation
		sample1.x = texture3D(volume_texunit, gl_TexCoord[0].xyz-vec3(gradient_delta.x,0.0,0.0) ).x;
		sample2.x = texture3D(volume_texunit, gl_TexCoord[0].xyz+vec3(gradient_delta.x,0.0,0.0) ).x;
		sample1.y = texture3D(volume_texunit, gl_TexCoord[0].xyz-vec3(0.0,gradient_delta.y,0.0) ).x;
		sample2.y = texture3D(volume_texunit, gl_TexCoord[0].xyz+vec3(0.0,gradient_delta.y,0.0) ).x;
		sample1.z = texture3D(volume_texunit, gl_TexCoord[0].xyz-vec3(0.0,0.0,gradient_delta.z) ).x;
		sample2.z = texture3D(volume_texunit, gl_TexCoord[0].xyz+vec3(0.0,0.0,gradient_delta.z) ).x;
		// compute illumination
		vec3 N = normalize(sample1-sample2);
		vec3 L = normalize(light_position - position);
		vec3 V = normalize(eye_position   - position);
		rgba.rgb *= blinn_phong(N,V,L);
	}

	gl_FragColor = rgba;
}
