#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;
uniform vec4 l_dir;	   // camera space

in vec4 position;	// local space
in vec4 normal;		// local space
in vec4 texCoord;

const float density = 0.02;
const float gradient = 3;

out Data {
	vec3 normal;
	vec3 eye;
	vec4 pos;
	vec2 tex_coord;
	vec3 lightDir;
	float visibility;
} DataOut;

void main () {
	vec4 pos = m_viewModel * position;
	DataOut.pos = pos;

	DataOut.normal = normalize(m_normal * normal.xyz);
	DataOut.eye = normalize(vec3(-pos));
	DataOut.tex_coord = texCoord.st;
	DataOut.lightDir = vec3(l_dir - pos);

	gl_Position = m_pvm * position;
	float distance = length(pos.xyz);
	DataOut.visibility = exp(-pow((distance * density), gradient));
	DataOut.visibility = clamp(DataOut.visibility, 0.0, 1.0);
}
