varying float LightIntensity;
varying vec3 MCposition;

float snoise(vec3 v);

void main(void)
{
	const vec3 SkinColor1 = vec3(0.9, 0.7, 0.2);
	const vec3 SkinColor2 = vec3(0.2, 0.5, 0.2);

	// const float base_freq = 0.1; // SPIDER
	// const float base_freq = 1.0; // COW
	// const float base_freq = 0.005; // DINO
	// const float base_freq = 0.0004; // CAT
	// const float base_freq = 0.5; // COW
	// const float base_freq = 0.5; // HORSE
	const float base_freq = 1.7; // DRAGON
	// const float base_freq = 1.5; // CANE
	// const float base_freq = 0.05; // HAND
	// const float base_freq = 1.5; // RINO

	float noise = snoise(MCposition * base_freq) * 8.0 + snoise(MCposition * base_freq*2.0) * 4.0 + snoise(MCposition * base_freq*4.0) * 2.0 + snoise(MCposition * base_freq * 8.0);
	noise = (noise / 8.0 + 1.0) / 2.0;
	// noise = noise * noise;
	 noise -= 0.3;
	 noise *= 2.0;
	// noise = noise * noise;
	float intensity = fract(noise * 3.14159265);
	vec3 color = vec3(0.0, 0.0, 0.0);
	
	intensity = clamp(intensity, 0.0, 1.0);
	if ( abs(intensity - 0.5) > 0.1 )
		discard;
	
	if (LightIntensity > 0)
		color = SkinColor1 * LightIntensity;
	else
		color = SkinColor2 * -LightIntensity;
	
	gl_FragColor = vec4(color, 1.0);
}
