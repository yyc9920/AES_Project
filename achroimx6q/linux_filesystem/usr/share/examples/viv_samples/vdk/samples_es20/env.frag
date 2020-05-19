varying vec3 normal;

uniform samplerCube env;

void main()
{
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragColor = textureCube(env, normal);
}