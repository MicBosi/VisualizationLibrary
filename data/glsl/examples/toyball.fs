varying vec4 ECposition; // surface position in eye coordinates
varying vec4 ECballCenter; // ball center in eye coordinates

void main()
{
	const vec4 LightDir      = vec4(0.577, 0.577, 0.577, 0.0); // light direction, should be normalized.
	const vec4 HVector       = vec4(0.325, 0.325, 0.888, 0.0); // reflection vector for infinite light.
	const vec4 BallCenter    = vec4(0.0, 0.0, 0.0, 1.0);     // ball center in modeling coordinates
	const vec4 SpecularColor = vec4(0.4, 0.4, 0.4, 60.0);
	const vec4 Red    = vec4(0.6, 0.0, 0.0, 1.0);
	const vec4 Blue   = vec4(0.0, 0.3, 0.6, 1.0);
	const vec4 Yellow = vec4(0.6, 0.5, 0.0, 1.0);
	
	// half spaces used to define the star
	/*const*/ vec4 HalfSpace0 = vec4(1.0, 0.0, 0.0, 0.2);
	/*const*/ vec4 HalfSpace1 = vec4(0.309016994, 0.951056516, 0.0, 0.2);
	/*const*/ vec4 HalfSpace2 = vec4(-0.809016994, 0.587785252, 0.0, 0.2);
	/*const*/ vec4 HalfSpace3 = vec4(-0.809016994, -0.587785252, 0.0, 0.2);
	/*const*/ vec4 HalfSpace4 = vec4(0.309016994, -0.951056516, 0.0, 0.2);
	
	HalfSpace0.xyz = gl_NormalMatrix * HalfSpace0.xyz;
	HalfSpace1.xyz = gl_NormalMatrix * HalfSpace1.xyz;
	HalfSpace2.xyz = gl_NormalMatrix * HalfSpace2.xyz;
	HalfSpace3.xyz = gl_NormalMatrix * HalfSpace3.xyz;
	HalfSpace4.xyz = gl_NormalMatrix * HalfSpace4.xyz;
	
	const float InOrOutInit = -3.0;
	const float StripeWidth = 0.5;
	const float FWidth = 0.005;
	
	vec4 normal;
	vec4 p;
	vec4 surfColor;
	float intensity;
	vec4 distance;
	float inorout;
	
	p.xyz = normalize(ECposition.xyz - ECballCenter.xyz);
	p.w = 1.0;
	
	inorout = InOrOutInit;
	
	distance[0] = dot(p, HalfSpace0);
	distance[1] = dot(p, HalfSpace1);
	distance[2] = dot(p, HalfSpace2);
	distance[3] = dot(p, HalfSpace3);
	
	distance = smoothstep(-FWidth, FWidth, distance);
	
	inorout += dot(distance, vec4(1.0));
	
	distance.x = dot(p, HalfSpace4);
	distance.y = StripeWidth - abs(p.z);
	distance = smoothstep(-FWidth, FWidth, distance);
	inorout += distance.x;
	
	inorout = clamp(inorout, 0.0, 1.0);
	
	surfColor = mix(Yellow, Red, inorout);
	surfColor = mix(surfColor, Blue, distance.y);
	
	// normal = point on surface for sphere at (0,0,0)
	normal = p;
	
	// Per-fragment diffuse lighting
	intensity = 0.2; // ambient
	intensity += 0.8 * clamp(dot(LightDir, normal), 0.0, 1.0);
	surfColor *= intensity;

	// Per-framgment specular lighting
	intensity = clamp(dot(HVector, normal), 0.0, 1.0);
	intensity = pow(intensity, SpecularColor.a);
	
	surfColor += SpecularColor * intensity;

	gl_FragColor = vec4(surfColor.rgb, 1.0);
	
	
}
