#version 330

out vec4 colorOut;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

uniform Materials mat;

struct LightProperties {
	bool isEnabled;	// true to apply this light in this invocation
	bool isLocal;	// true for a point light or a spotlight,
			// false for a positional light

	bool isSpot;	// true if the light is a spotlight
	vec4 ambient;	// lights contribution to ambient light
	vec4 diffuse;
	vec4 specular;

	vec3 position_point;
	vec4 direction;
	vec4 position;	// location of light, if is Local is true,
			// otherwise the direction toward the light

	vec3 halfVector;	// direction of highlights for directional light
	vec3 coneDirection; 	// spotlight attributes

	float spotCosCutoff;
	float spotExponent;

	float constantAttenuation;	// local light attenuation coefficients
	float linearAttenuation;
	float quadraticAttenuation;

	// other properties you may desire
};

in Data {
	vec3 normal;
	vec3 eye;
	vec4 pos;
	vec2 tex_coord;
	vec3 lightDir;
	float visibility;
} DataIn;

const int nrLights = 9;
uniform LightProperties lights[nrLights];

uniform sampler2D texmap;
uniform sampler2D texmap1;
uniform sampler2D texmap2;
uniform int texMode;

//fog
uniform vec3 skyColor;
uniform int fogMode;

void main() {

	vec3 scatteredLight = vec3(0.0); // or, to a global ambient light
	vec3 reflectedLight = vec3(0.0);

 //Billboard things

 vec3 spec = vec3(0.0);
 vec4 specMulti = vec4(0.0);
 vec3 n = normalize(DataIn.normal);
 vec3 l = normalize(DataIn.lightDir);
 vec3 e = normalize(DataIn.eye);

 float intensity = max(dot(n,l), 0.0);
 if (intensity > 0.0) {
	 vec3 h = normalize(l + e);
	 float intSpec = max(dot(h,n), 0.0);
	 spec = mat.specular.rgb * pow(intSpec, mat.shininess);
	 specMulti = mat.specular * pow(intSpec, mat.shininess);
 }

	// loop over all the lights
	for (int light = 0; light < nrLights; light++) {
		if (!lights[light].isEnabled) continue;

		vec3 halfVector;
		vec3 lightDirection = vec3(lights[light].position);
		float attenuation = 1.0;

		if(lights[light].isLocal) {
			lightDirection = lightDirection - vec3(DataIn.pos);
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection / lightDistance;
			attenuation = 1.0 / (lights[light].constantAttenuation
					+ lights[light].linearAttenuation * lightDistance
					+ lights[light].quadraticAttenuation * lightDistance
					* lightDistance);

			if (lights[light].isSpot){
				float spotCos = dot(lightDirection, normalize(vec3(-lights[light].direction)));
				if (spotCos < lights[light].spotCosCutoff)
					attenuation = 0.0;
				else
					attenuation *= pow(spotCos, lights[light].spotExponent);
			}

			halfVector = normalize(lightDirection + DataIn.eye);
		}
		else {
			lightDirection = normalize(-lightDirection);
			halfVector = normalize((lightDirection + DataIn.eye) / 2);
		}

		float diffuse = max(0.0, dot(DataIn.normal, lightDirection)); //intensity
		float specular = max(0.0, dot(DataIn.normal, halfVector));
		if (diffuse == 0.0) specular = 0.0;
		else specular = pow(specular, mat.shininess);


		scatteredLight += lights[light].diffuse.rgb * mat.ambient.rgb * attenuation + lights[light].ambient.rgb * mat.diffuse.rgb * diffuse * attenuation;
		reflectedLight += lights[light].ambient.rgb * mat.specular.rgb * specular * attenuation;
	}

	vec3 rgb = min((mat.emissive.rgb + scatteredLight + reflectedLight), vec3(1.0));
	vec4 texel, texel2;

	if (texMode == 0) {       //phong color
		colorOut = vec4(rgb, mat.diffuse.a);
	}

	else if (texMode == 1) {  // 1 texture
		texel = texture(texmap, DataIn.tex_coord);
		colorOut = vec4(rgb, mat.diffuse.a) * texel;
	}

	else if (texMode == 2) {  // w/ blending
		texel = texture(texmap1, DataIn.tex_coord);
		colorOut = vec4(rgb, 0.3) * texel; //2nd argument is our blending factor
	}

	else if (texMode == 3)  {  // tree texture for billboard
		texel = texture(texmap2, DataIn.tex_coord);  		if(texel.a == 0.0) discard;
		else
			colorOut = vec4(max(intensity * texel.rgb + spec, 0.1*texel.rgb), texel.a);
	}

	else if (texMode == 4)  {  // particles
		texel = texture(texmap2, DataIn.tex_coord);  		if(texel.a == 0.0) discard;
		else
			colorOut = mat.diffuse * texel;
	}

	else if (texMode == 5){  //multitexturing
			texel = texture(texmap, DataIn.tex_coord);
			texel2  = texture(texmap2, DataIn.tex_coord);
			colorOut = max(intensity * texel * texel2 + specMulti, 0.1 * texel * texel2);

	}

	if (fogMode == 1){
		colorOut = mix(vec4(skyColor,1.0),colorOut,DataIn.visibility);
	}

}
