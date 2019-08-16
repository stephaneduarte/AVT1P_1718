#include <iostream>
#include <sstream>
#include <math.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <chrono>

#include "windows.h"

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "basic_geometry.h"
#include "TGA.h"

#include "Camera.h"
#include "OrthoCamera.h"
#include "PerspectiveCamera.h"
#include "Cheerio.h"
#include "Light.h"
#include "Orange.h"
#include "Butter.h"
#include "Particle.h"
#include "GameObject.h"
#include "Car.h"

#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICLES  400
#define CAPTION "Micro Machines"
#define NUM_TEXTURES 13
#define NUM_LIVES 3
#define isqrt(x)        (int)((double)(x))

int     xMouse = 0, yMouse = 0;
int     xFlare = 10;
int     yFlare = 10;

const int numflares = 6;
float flaresColor[4] = { 1.0f, 1.0f, 1.0f, 0.5f };
float flaresPos[numflares] = { 0.0f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };
float flaresScale[numflares] = { 1.0f, 0.25f, 0.5f, 0.5f, 0.25f, 0.75f };

//Cameras
int _camera; // come�a com a ortogonal
int _oldcamera = 1; //camera de antes da pausa

OrthoCamera* _orthoCamera;
const int ORTHOCAMERAID = 1;
PerspectiveCamera* _perspCamera;
const int PERSPECTIVECAMERAID = 2;
PerspectiveCamera* _perspCarCamera;
const int PERSPECTIVECARCAMERAID = 3;

//Window config
int WindowHandle = 0;
int WinX = 700, WinY = 700;

// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
char s[32];
unsigned int FrameCount = 0;

//Shader
VSShaderLib shader;

struct MyMesh mesh[12];
int objId=0; //id of the object mesh - to be used as index of mesh: mesh[objID] means the current mesh


//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint lDir_uniformId;

GLint tex_loc, tex_loc1, tex_loc2;
GLint texMode_uniformId;

GLuint TextureArray[NUM_TEXTURES];


//Lights
std::vector<Light*> _lights;

//Tamanho mesa de jogo
const int TABLESIZE = 100;

//Object ID
const int TABLEID = 0;
const int CHEERIOID = 1;
const int ORANGEID = 2;
const int CARID = 3;
const int WHEELID = 4;
const int BUTTERID = 5;
const int STENCILOBJID = 6;
const int QUADID = 7;
const int PARTICLEID = 8;
const int HUDID = 9;
const int MAINSCREENID = 10;
const int LENSFLAREID = 11;

//Objects
Car* _car;
Butter* _butter;
std::vector<GameObject*> _go;
std::vector<Particle*> _p;


//Other variables
double _oldtime;
double _time;
bool paused = FALSE;
int _score;
bool stencil = TRUE;
bool lensFlare = TRUE;
bool snow = FALSE;
bool night = FALSE;
int iniciated = FALSE;
int exists = 0;
int orange_angle = 0;
float particleColor[4];

//Timer variables
std::chrono::time_point<std::chrono::system_clock> m_StartTime;
std::chrono::time_point<std::chrono::system_clock> m_EndTime;
bool m_bRunning = false;

//FOG
GLint skyColor;
float fogColor[3] = { 0.0f,0.0f,0.0f };
GLint fogMode;
bool fogStatus = false;


//Sounds
bool sound = false;

//-----------------------------------------------------------------------------------------------
//
//             TENTATIVA DE OBJETOS
//

#pragma comment(lib,"assimp.lib")
#pragma comment(lib,"devil.lib")
#pragma comment(lib,"glew32.lib")
// include DevIL for image loading
#include <IL\il.h>
// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"

// Information to render each assimp node
struct MyObjMesh {

	GLuint vao;
	GLuint texIndex;
	GLuint uniformBlockIndex;
	struct Material mat;
	int numFaces;
};
std::vector<struct MyObjMesh> myObjMeshes; //Guarda os obj

										   // Create an instance of the Importer class
Assimp::Importer importer;

// the global Assimp scene object
const aiScene* scene = NULL;

// scale factor for the model to fit in the window
float scaleFactor;

// The sampler uniform for textured models
// we are assuming a single texture so this will
//always be texture unit 0
GLuint texUnit = 0;

// images / texture
// map image filenames to textureIds
// pointer to texture Array
std::map<std::string, GLuint> textureIdMap;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void get_bounding_box_for_node(const aiNode* nd,
	aiVector3D* min,
	aiVector3D* max)

{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n], min, max);
	}
}

void get_bounding_box(aiVector3D* min, aiVector3D* max)
{

	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode, min, max);
}

bool Import3DFromFile(const std::string& pFile)
{

	scene = importer.ReadFile(pFile, aiProcessPreset_TargetRealtime_Quality);

	// If the import failed, report it
	if (!scene)
	{
		printf("%s\n", importer.GetErrorString());
		return false;
	}

	// Now we can access the file's contents.
	printf("Import of scene %s succeeded.", pFile.c_str());

	aiVector3D scene_min, scene_max, scene_center;
	get_bounding_box(&scene_min, &scene_max);
	float tmp;
	tmp = scene_max.x - scene_min.x;
	tmp = scene_max.y - scene_min.y > tmp ? scene_max.y - scene_min.y : tmp;
	tmp = scene_max.z - scene_min.z > tmp ? scene_max.z - scene_min.z : tmp;
	scaleFactor = 1.f / tmp;

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}


//// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
//void Color4f(const aiColor4D *color)
//{
//	glColor4f(color->r, color->g, color->b, color->a);
//}

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void color4_to_float4(const aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

void genVAOsAndUniformBuffer(const aiScene *sc) {

	struct MyObjMesh aMesh;
	struct Material aMat;
	GLuint buffer;

	// For each mesh
	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		const aiMesh* mesh = sc->mMeshes[n];

		// create array with faces
		// have to convert from Assimp format to array
		unsigned int *faceArray;
		faceArray = (unsigned int *)malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];

			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
			faceIndex += 3;
		}
		aMesh.numFaces = sc->mMeshes[n]->mNumFaces;

		// generate Vertex Array for mesh
		glGenVertexArrays(1, &(aMesh.vao));
		glBindVertexArray(aMesh.vao);

		// buffer for faces
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

		// buffer for vertex positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
			glVertexAttribPointer(VERTEX_COORD_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
			glEnableVertexAttribArray(NORMAL_ATTRIB);
			glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
		}

		// buffer for vertex texture coordinates
		if (mesh->HasTextureCoords(0)) {
			float *texCoords = (float *)malloc(sizeof(float) * 2 * mesh->mNumVertices);
			for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {

				texCoords[k * 2] = mesh->mTextureCoords[0][k].x;
				texCoords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;

			}
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(TEXTURE_COORD_ATTRIB);
			glVertexAttribPointer(TEXTURE_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);
		}

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// create material uniform buffer
		aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];

		aiString texPath;	//contains filename of texture
		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
			//bind texture
			unsigned int texId = textureIdMap[texPath.data];
			aMesh.texIndex = texId;
			aMat.texCount = 1;
		}
		else
			aMat.texCount = 0;

		float c[4];
		set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
		aiColor4D diffuse;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
			color4_to_float4(&diffuse, c);
		memcpy(aMat.diffuse, c, sizeof(c));

		set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
		aiColor4D ambient;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
			color4_to_float4(&ambient, c);
		memcpy(aMat.ambient, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D specular;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
			color4_to_float4(&specular, c);
		memcpy(aMat.specular, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D emission;
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
			color4_to_float4(&emission, c);
		memcpy(aMat.emissive, c, sizeof(c));

		float shininess = 0.0;
		unsigned int max;
		aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
		aMat.shininess = shininess;

		//OI?
		memcpy(aMesh.mat.diffuse, aMat.diffuse, sizeof(aMat.diffuse));
		memcpy(aMesh.mat.ambient, aMat.ambient, sizeof(aMat.ambient));
		memcpy(aMesh.mat.specular, aMat.specular, sizeof(aMat.specular));
		memcpy(aMesh.mat.emissive, aMat.emissive, sizeof(aMat.emissive));
		aMesh.mat.shininess = aMat.shininess;

		glGenBuffers(1, &(aMesh.uniformBlockIndex));
		glBindBuffer(GL_UNIFORM_BUFFER, aMesh.uniformBlockIndex);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(aMat), (void *)(&aMat), GL_STATIC_DRAW);

		myObjMeshes.push_back(aMesh);
	}
}

// Render Assimp Model

void recursive_render(const aiScene *sc, const aiNode* nd)
{
	// Get node transformation matrix
	aiMatrix4x4 m = nd->mTransformation;
	// OpenGL matrices are column major
	m.Transpose();

	// save model matrix and apply node transformation
	pushMatrix(MODEL);

	float aux[16];
	memcpy(aux, &m, sizeof(float) * 16);
	multMatrix(mMatrix[MODEL], aux);

	//send matrices
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {
		/*// bind material uniform
		glBindBufferRange(GL_UNIFORM_BUFFER, materialUniLoc, myMeshes[nd->mMeshes[n]].uniformBlockIndex, 0, sizeof(struct MyMaterial));*/
		// bind texture
		glBindTexture(GL_TEXTURE_2D, myObjMeshes[nd->mMeshes[n]].texIndex);
		// bind VAO
		glBindVertexArray(myObjMeshes[nd->mMeshes[n]].vao);
		// draw
		glDrawElements(GL_TRIANGLES, myObjMeshes[nd->mMeshes[n]].numFaces * 3, GL_UNSIGNED_INT, 0);

	}
	// draw all children
	for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
		recursive_render(sc, nd->mChildren[n]);
	}
	popMatrix(MODEL);
}

void createObj() {
	Import3DFromFile("slaight.obj");
	genVAOsAndUniformBuffer(scene);
}

// ------------------------------------------------------------
//
// Stencil
//

void createStencilObject() {
	float amb[] = { 0.2f, 0.2f, 0.0f, 1.0f };
	float diff[] = { 0.7f, 0.7f, 0.0f, 1.0f };
	float spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 1000.0f;
	int texcount = 0;
	objId = STENCILOBJID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;

	createCube();
}

void drawStencilObject() {

	objId = STENCILOBJID;

	pushMatrix(MODEL);
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	translate(MODEL, 0.5f, -0.5f, 0.5f);
	scale(MODEL, 0.5f, 0.5f, 0.5f);
	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_NEVER, 0x1, 0x1);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);

	glBindVertexArray(mesh[objId].vao);
	glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	popMatrix(MODEL);
}

// ------------------------------------------------------------
//
// Particle
//

void updateParticles(float delta_t) {
	for (int i = 0; i < _p.size(); i++) {
		_p.at(i)->update(delta_t);
	}
}

void iterate(int value) {
	float h = 0.125f;

	if (snow) {
		updateParticles(h);
		glutPostRedisplay();
		glutTimerFunc(33, iterate, 0);
	}
}


// ------------------------------------------------------------
//
// Timer
//


void start(){
	m_StartTime = std::chrono::system_clock::now();
	m_bRunning = true;
}

void stop(){
	m_EndTime = std::chrono::system_clock::now();
	m_bRunning = false;
}

double elapsedMilliseconds(){
	std::chrono::time_point<std::chrono::system_clock> endTime;

	if (m_bRunning)
		endTime = std::chrono::system_clock::now();
	else
		endTime = m_EndTime;

	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
}

double elapsedSeconds() { return elapsedMilliseconds() / 1000.0; }


// ------------------------------------------------------------
//
// Lens Flare 
//

void createLensFlare() {
	float amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float emissive[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess = 1000.0f;
	int texcount = 0;
	objId = LENSFLAREID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;
	createCube();
}

void drawLensFlare() {

	GLint loc;
	objId = LENSFLAREID;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); //para desaparecer o fundo preto das texturas
	glUniform1i(texMode_uniformId, 1);

	WinX = glutGet(GLUT_WINDOW_WIDTH);
	WinY = glutGet(GLUT_WINDOW_HEIGHT);
	int cx = WinX / 2;
	int cy = WinY / 2;

	// Compute how far off-center the flare source is.
	int maxflaredist = isqrt(cx*cx + cy*cy);
	int flaredist = isqrt((xFlare - cx)*(xFlare - cx) + (yFlare - cy)*(yFlare - cy));
	flaredist = (maxflaredist - flaredist);
	int flaremaxsize = (int)(WinX * 0.3); //0.3 constant for fMaxSize
	int flarescale = (int)(WinX * 0.3); //0.3 constant for fScale

										// Destination is opposite side of centre from source
	int dx = cx + (cx - xFlare);
	int dy = cy + (cy - yFlare);


	for (int i = 0; i < numflares; i++) {
		if (i == 0 || i == numflares - 1)
			glUniform1i(tex_loc, 6);
		else if (i == 1 || i == numflares - 2)
			glUniform1i(tex_loc, 7);
		else
			glUniform1i(tex_loc, 8);


		// Position is interpolated along line between start and destination.
		int px = flaresPos[i] * xFlare + (1.0f - flaresPos[i]) * dx;
		int py = (1.0f - flaresPos[i])*yFlare + flaresPos[i] * dy;

		// Piece size are 0 to 1; flare size is proportion of
		// screen width; scale by flaredist/maxflaredist.
		int width = (int)((flaredist*flarescale*flaresScale[i]) / maxflaredist);

		// Width gets clamped, to allows the off-axis flares
		// to keep a good size without letting the elements get
		// too big when centered.
		if (width > flaremaxsize)
			width = flaremaxsize;

		// Flare elements are square (round) so we'll use same value for width and height
		int height = width;
		int alpha = flaredist / maxflaredist;
		if (width < 1) continue;

		flaresColor[3] = alpha;		

		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, mesh[objId].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, mesh[objId].mat.specular);


		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, mesh[objId].mat.shininess);
		pushMatrix(MODEL);
		pushMatrix(PROJECTION);
		pushMatrix(VIEW);

		loadIdentity(VIEW);
		loadIdentity(PROJECTION);
		ortho(0, WinX, 0, WinY, 0, 1);
		translate(MODEL, px - width / 2, py - height / 2, 0);
		scale(MODEL, width, height, 0);

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(mesh[objId].vao);
		glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
		popMatrix(VIEW);
		popMatrix(PROJECTION);
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}


// ------------------------------------------------------------
//
// HUD
//

void createHUD() {
	float amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float emissive[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;
	objId = HUDID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;
	createCube();
}

void drawHUD() {

	GLint loc;
	objId = HUDID;
	int livesLeft = _car->getLifes();


	for (int i = 0; i < NUM_LIVES; i++) {
		if (i < livesLeft) {
			glUniform1i(texMode_uniformId, 1);
			glUniform1i(tex_loc, 11);
		}
		else {
			glUniform1i(texMode_uniformId, 1);
			glUniform1i(tex_loc, 12);
		}
		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, mesh[objId].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, mesh[objId].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, mesh[objId].mat.shininess);
		pushMatrix(MODEL);
		pushMatrix(PROJECTION);
		pushMatrix(VIEW);

		loadIdentity(VIEW);
		loadIdentity(PROJECTION);

		ortho(-10, 10, -10, 10, 0, 10);
		translate(MODEL, 9 - i, 8.3, 0);

		//translate(MODEL, -1.5, -0.75, 0);
		scale(MODEL, 1, 1.7, 0);

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(mesh[objId].vao);
		glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
		popMatrix(VIEW);
		popMatrix(PROJECTION);
	}
}


// ------------------------------------------------------------
//
// MainScreen
//

void createMainScreen() {
	float amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float emissive[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float shininess = 1000.0f;
	int texcount = 0;
	objId = MAINSCREENID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;
	createQuad(TABLESIZE, TABLESIZE);
}

void drawMainScreen() {
	GLint loc;
	objId = MAINSCREENID;

	if (_car->getLifes() > 0)
		glUniform1i(tex_loc, 9); //Pause
	else
		glUniform1i(tex_loc, 10); //Game Over
	glUniform1i(texMode_uniformId, 1);
	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, mesh[objId].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, mesh[objId].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, mesh[objId].mat.shininess);
	pushMatrix(MODEL);

	translate(MODEL, 50, 10, 50);
	rotate(MODEL, 90, 0.0, -1.0, 0.0);
	rotate(MODEL, 90, -1.0, 0.0, 0.0);
	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	glBindVertexArray(mesh[objId].vao);
	glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	popMatrix(MODEL);
}

// ------------------------------------------------------------
//
// Billboards
//

void  crossProduct(float *a, float *b, float *res) {

	res[0] = a[1] * b[2] - b[1] * a[2];
	res[1] = a[2] * b[0] - b[2] * a[0];
	res[2] = a[0] * b[1] - b[0] * a[1];
}

// returns a . b
float dotProduct(float *a, float *b) {

	float res = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];

	return res;
}

// Normalize a vec3
void normalize(float *a) {

	float mag = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	a[0] /= mag;
	a[1] /= mag;
	a[2] /= mag;
}


void billboardCylindrical(float objPosX, float objPosY, float objPosZ) {

	float lookAt[3], objToCamProj[3], upAux[3];
	float angleCosine;


	if (_camera == 2) {
		camX = -5;
		camZ = -5;
	}

	if (_camera == 3) {
		camX = _car->getPositionX() - 10 * _car->getDirX();
		camZ = _car->getPositionZ() - 10 * _car->getDirZ();
	}

	// objToCamProj is the vector in world coordinates from the
	// local origin to the camera projected in the XZ plane
	objToCamProj[0] = camX - objPosX;
	objToCamProj[1] = 0;
	objToCamProj[2] = camZ - objPosZ;

	// This is the original lookAt vector for the object
	// in world coordinates
	lookAt[0] = 0;
	lookAt[1] = 0;
	lookAt[2] = 1;


	// normalize both vectors to get the cosine directly afterwards
	normalize(objToCamProj);

	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.

	crossProduct(lookAt, objToCamProj, upAux);

	// compute the angle
	angleCosine = dotProduct(lookAt, objToCamProj);

	// perform the rotation. The if statement is used for stability reasons
	// if the lookAt and objToCamProj vectors are too close together then
	// |angleCosine| could be bigger than 1 due to lack of precision
	if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
		rotate(MODEL, acos(angleCosine) * 180 / 3.14, upAux[0], upAux[1], upAux[2]);

}


// ------------------------------------------------------------
//
// Objects
//


void createTable() {
	float amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diff[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float spec[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;
	objId = TABLEID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;
	createQuad(TABLESIZE, TABLESIZE);
}

void drawTable() {
	GLint loc;
	objId = TABLEID;

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, mesh[objId].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, mesh[objId].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, mesh[objId].mat.shininess);
	pushMatrix(MODEL);

	translate(MODEL, 50, 0, 50);
	rotate(MODEL, 90, -1.0, 0.0, 0.0);

	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh


	glBindVertexArray(mesh[objId].vao);
	glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	popMatrix(MODEL);
}

void createCheerios() {
	float cheerios_posX[] = { 5.0f, 12.5f, 20.0f, 27.5f, 35.0f, 42.5f, 50.0f, 57.5f, 65.0f, 72.5f, 80.0f, 87.5f, 95.0f, // linha de baixo 1
		27.5f, 35.0f, 42.5f, 50.0f, 57.5f, 65.0f, 72.5f, //linha de baixo 2
		5.0f, 12.5f, 20.0f, 27.5f, 35.0f, 42.5f, 50.0f, 57.5f, 65.0f, 72.5f, 80.0f, 87.5f, 95.0f, // linha de cima 1
		27.5f, 35.0f, 42.5f, 50.0f, 57.5f, 65.0f, 72.5f, //linha de cima 2
		5.0f, 5.0f,5.0f,5.0f,5.0f,5.0f,5.0f,5.0f,5.0f,5.0f,5.0f, // coluna 1
		95.f, 95.f,95.f,95.f,95.f,95.f,95.f,95.f,95.f,95.f,95.f, // coluna 4
		27.5f,27.5f,27.5f,27.5f,27.5f, // coluna 2
		72.5f,72.5f,72.5f,72.5f,72.5f }; // coluna 3

	float cheerios_posZ[] = { 5.0f,  5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f,
		27.5f, 27.5f, 27.5f, 27.5f, 27.5f, 27.5f, 27.5f,
		95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f, 95.0f,
		72.5f, 72.5f, 72.5f, 72.5f, 72.5f, 72.5f, 72.5f,
		12.5f, 20.0f, 27.5f, 35.0f, 42.5f, 50.0f, 57.5f, 65.0f, 72.5f, 80.0f, 87.5f,
		12.5f, 20.0f, 27.5f, 35.0f, 42.5f, 50.0f, 57.5f, 65.0f, 72.5f, 80.0f, 87.5f,
		35.0f, 42.5f, 50.0f, 57.5f, 65.0f,
		35.0f, 42.5f, 50.0f, 57.5f, 65.0f };

	for (int i = 0; i < 72; i++) {
		_go.push_back(new Cheerio(cheerios_posX[i], 1.0f, cheerios_posZ[i]));
	}

	float amb[] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
	float diff[] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
	float spec[] = { 0.774597f, 0.458561f, 0.200621f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float shininess = 76.8f;
	int texcount = 0;
	objId = CHEERIOID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;
	createTorus(2.0f, 0.8f, 50, 50);
}

void drawCheerios() {
	GLint loc;
	objId = CHEERIOID;

	for (int i = 0; i < _go.size(); i++) {
		if (_go.at(i)->getName() == "Cheerio") {
			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, mesh[objId].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, mesh[objId].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, mesh[objId].mat.shininess);
			pushMatrix(MODEL);

			translate(MODEL, _go.at(i)->getPositionX(), 1.0, _go.at(i)->getPositionZ());

			// send matrices to OGL
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			// Render mesh


			glBindVertexArray(mesh[objId].vao);
			glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			popMatrix(MODEL);
		}
	}
}

void createOranges() {
	float oranges_posX[] = { 15, 85, 85 };
	float oranges_posZ[] = { 85, 85, 15 };
	for (int i = 0; i < 3; i++) {
		_go.push_back(new Orange(oranges_posX[i], 1.5, oranges_posZ[i]));
	}

	float amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 12.8f;
	int texcount = 0;
	objId = ORANGEID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;

	createSphere(3.0f, 20);
}

void drawOranges() {
	GLint loc;
	objId = ORANGEID;

	for (int i = 0; i < _go.size(); i++) {
		if (_go.at(i)->getName() == "Orange") {
			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, mesh[objId].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, mesh[objId].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, mesh[objId].mat.shininess);
			pushMatrix(MODEL);

			translate(MODEL, _go.at(i)->getPositionX(), 3, _go.at(i)->getPositionZ());
			rotate(MODEL, orange_angle++, 0.5, 0, 0.5);

			// send matrices to OGL
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			// Render mesh


			glBindVertexArray(mesh[objId].vao);
			glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			popMatrix(MODEL);
		}
	}
}

void createButter() {

	float butters_posX[] = { 62, 25 };
	float butters_posZ[] = { 14, 78 };

	for (int i = 0; i < 2; i++) {
		_go.push_back(new Butter(butters_posX[i], 3, butters_posZ[i]));
	}

	float amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float spec[] = { 0.0225f, 0.0225f, 0.0225f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 12.8f;
	int texcount = 0;
	objId = BUTTERID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;

	createCube();
}

void drawButter() {

	GLint loc;
	objId = BUTTERID;

	for (int i = 0; i < _go.size(); i++) {
		if (_go.at(i)->getName() == "Butter") {
			// send the material
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
			glUniform4fv(loc, 1, mesh[objId].mat.ambient);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
			glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
			glUniform4fv(loc, 1, mesh[objId].mat.specular);
			loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
			glUniform1f(loc, mesh[objId].mat.shininess);
			pushMatrix(MODEL);

			translate(MODEL, _go.at(i)->getPositionX(), 0, _go.at(i)->getPositionZ());
			rotate(MODEL, 90, 0, 90, 0);
			translate(MODEL, -3, 0, -2);
			scale(MODEL, 6, 2, 4);

			// send matrices to OGL
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			// Render mesh


			glBindVertexArray(mesh[objId].vao);
			glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			popMatrix(MODEL);
		}
	}

}

void createCar() {
	float amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diff[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float spec[] = { 0.0225f, 0.0225f, 0.0225f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 12.8f;
	int texcount = 0;
	objId = CARID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;

	createCube();

	_car = new Car(NUM_LIVES);
	_go.push_back(_car);
}

void drawCar() {
	GLint loc;
	objId = CARID;

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, mesh[objId].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, mesh[objId].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, mesh[objId].mat.shininess);
	pushMatrix(MODEL);

	translate(MODEL, _car->getPositionX(), _car->getPositionY(), _car->getPositionZ());
	rotate(MODEL, _car->getRotY() * 180 / PI, 0, -1, 0);
	translate(MODEL, -3.5, 0, -1.5);
	scale(MODEL, 7, 3, 3);

	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);



	glBindVertexArray(mesh[objId].vao);
	glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	popMatrix(MODEL);
}

void createWheels() {
	float amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diff[] = { 0.454902f, 0.454902f, 0.454902f, 1.0f };
	float spec[] = { 0.0225f, 0.0225f, 0.0225f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 12.8f;
	int texcount = 0;
	objId = WHEELID;

	memcpy(mesh[objId].mat.ambient, amb, 4 * sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff, 4 * sizeof(float));
	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;

	createTorus(1.0f, 0.7f, 50, 50);
}

void drawWheels() {
	GLint loc;
	objId = WHEELID;

	float wheels_posX[] = { -2.0f, 2.0f, 2.0f, -2.0f };
	float wheels_posZ[] = { -1.7f, -1.7f, 1.7f, 1.7f };

	for (int i = 0; i < 4; i++) {
		// send the material
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
		glUniform4fv(loc, 1, mesh[objId].mat.ambient);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
		glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, mesh[objId].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, mesh[objId].mat.shininess);
		pushMatrix(MODEL);

		translate(MODEL, _car->getPositionX() + wheels_posX[i] * _car->getDirX() - wheels_posZ[i] * _car->getDirZ(), 0.5, _car->getPositionZ() + wheels_posZ[i] * _car->getDirX() + wheels_posX[i] * _car->getDirZ());
		rotate(MODEL, _car->getRotY() * 180 / PI, 0, -1, 0);
		rotate(MODEL, 90, 1, 0, 0);
		translate(MODEL, -0.5, 0, -0.5);

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);



		glBindVertexArray(mesh[objId].vao);
		glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
	}
}

void drawObjCar() {
	pushMatrix(MODEL);
	translate(MODEL, _car->getPositionX(), _car->getPositionY(), _car->getPositionZ());
	rotate(MODEL, _car->getRotY() * 180 / PI, 0, -1, 0);
	translate(MODEL, 0.5, 3, 1.1);
	scale(MODEL, 0.01f, 0.01f, 0.01f);
	recursive_render(scene, scene->mRootNode);
	popMatrix(MODEL);
}

void createLights() {
	//DirectionalLight
	Light* _directionalLight = new Light(0);
	_directionalLight->setState(true);
	_directionalLight->setIsLocal(false);
	_directionalLight->setIsSpot(false);
	_directionalLight->setPosition(vec4(-10.0f, -10.0f, -10.0f, 0.0f));
	_directionalLight->setAmbient(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_directionalLight->setDiffuse(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	_directionalLight->setSpecular(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	_lights.push_back(_directionalLight);

	//Pointlights
	float _pointlight_posX[] = { 11.25f, 11.25f, 11.25f, 88.75f, 88.75f, 88.75f };
	float _pointlight_posZ[] = { 11.25f, 50.0f, 88.75f, 11.25f, 50.0f, 88.75f };
	for (int i = 0; i < 6; i++) {
		Light* _pointlight = new Light(i+1);
		_pointlight->setState(false);
		_pointlight->setIsSpot(false);
		_pointlight->setIsLocal(true);
		_pointlight->setPosition(vec4(_pointlight_posX[i], 5.0f, _pointlight_posZ[i], 1.0f));
		_pointlight->setAmbient(vec4(0.4f, 0.4f, 0.4f, 1.0f));
		_pointlight->setDiffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		_pointlight->setSpecular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
		_pointlight->setConstantAttenuation(0.2);
		_pointlight->setLinearAttenuation(0.4);
		_pointlight->setQuadraticAttenuation(0);
		_lights.push_back(_pointlight);
	}

	//Spotlights
	Light* _carleft_light = new Light(7);
	_carleft_light->setIsLocal(true);
	_carleft_light->setIsSpot(true);
	_carleft_light->setState(true);
	_carleft_light->setAmbient(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_carleft_light->setDiffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_carleft_light->setSpecular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_carleft_light->setConstantAttenuation(0.01);
	_carleft_light->setLinearAttenuation(0.01);
	_carleft_light->setQuadraticAttenuation(0.01);
	_carleft_light->setCutOff(70.0f);
	_carleft_light->setExponent(4.0f);
	_lights.push_back(_carleft_light);

	Light* _carright_light = new Light(8);
	_carright_light->setIsLocal(true);
	_carright_light->setIsSpot(true);
	_carright_light->setState(true);
	_carright_light->setAmbient(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_carright_light->setDiffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_carright_light->setSpecular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	_carright_light->setConstantAttenuation(0.01);
	_carright_light->setLinearAttenuation(0.01);
	_carright_light->setQuadraticAttenuation(0.01);
	_carright_light->setCutOff(70.0f);
	_carright_light->setExponent(4.0f);
	_lights.push_back(_carright_light);
}

void setupLights() {

	//Directional Light
	GLint loc;

	float res[4];
	float faux[4];
	faux[0] = _lights[0]->getPosition().getX();
	faux[1] = _lights[0]->getPosition().getY();
	faux[2] = _lights[0]->getPosition().getZ();
	faux[3] = _lights[0]->getPosition().getW();
	multMatrixPoint(VIEW, faux, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[0].position");
	glUniform4fv(loc, 1, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[0].ambient");
	faux[0] = _lights[0]->getAmbient().getX();
	faux[1] = _lights[0]->getAmbient().getY();
	faux[2] = _lights[0]->getAmbient().getZ();
	faux[3] = _lights[0]->getAmbient().getW();
	glUniform4fv(loc, 1, faux);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[0].diffuse");
	faux[0] = _lights[0]->getDiffuse().getX();
	faux[1] = _lights[0]->getDiffuse().getY();
	faux[2] = _lights[0]->getDiffuse().getZ();
	faux[3] = _lights[0]->getDiffuse().getW();
	glUniform4fv(loc, 1, faux);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[0].specular");
	faux[0] = _lights[0]->getSpecular().getX();
	faux[1] = _lights[0]->getSpecular().getY();
	faux[2] = _lights[0]->getSpecular().getZ();
	faux[3] = _lights[0]->getSpecular().getW();
	glUniform4fv(loc, 1, faux);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[0].isEnabled");
	glUniform1i(loc, _lights[0]->getState());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[0].isLocal");
	glUniform1i(loc, _lights[0]->getIsLocal());


	std::string fin = "";
	for (int i = 1; i < 7; i++) {
		Light* light = static_cast<Light*>(_lights[i]);

		fin = "lights[" + std::to_string(i) + "].isLocal";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		glUniform1i(loc, light->getIsLocal());

		fin = "lights[" + std::to_string(i) + "].isSpot";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		glUniform1i(loc, light->getIsSpot());

		fin = "lights[" + std::to_string(i) + "].isEnabled";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		glUniform1i(loc, light->getState());

		fin = "lights[" + std::to_string(i) + "].ambient";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		faux[0] = light->getAmbient().getX();
		faux[1] = light->getAmbient().getY();
		faux[2] = light->getAmbient().getZ();
		faux[3] = light->getAmbient().getW();
		glUniform4fv(loc, 1, faux);

		fin = "lights[" + std::to_string(i) + "].diffuse";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		faux[0] = light->getDiffuse().getX();
		faux[1] = light->getDiffuse().getY();
		faux[2] = light->getDiffuse().getZ();
		faux[3] = light->getDiffuse().getW();
		glUniform4fv(loc, 1, faux);

		fin = "lights[" + std::to_string(i) + "].specular";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		faux[0] = light->getSpecular().getX();
		faux[1] = light->getSpecular().getY();
		faux[2] = light->getSpecular().getZ();
		faux[3] = light->getSpecular().getW();
		glUniform4fv(loc, 1, faux);


		faux[0] = light->getPosition().getX();
		faux[1] = light->getPosition().getY();
		faux[2] = light->getPosition().getZ();
		faux[3] = light->getPosition().getW();
		multMatrixPoint(VIEW, faux, res);
		fin = "lights[" + std::to_string(i) + "].position_point";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		glUniform3fv(loc, 1, res);

		fin = "lights[" + std::to_string(i) + "].constantAttenuation";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		glUniform1f(loc, light->getConstantAttenuation());
		fin = "lights[" + std::to_string(i) + "].linearAttenuation";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		glUniform1f(loc, light->getLinearAttenuation());
		fin = "lights[" + std::to_string(i) + "].quadraticAttenuation";
		loc = glGetUniformLocation(shader.getProgramIndex(), fin.c_str());
		glUniform1f(loc, light->getQuadraticAttenuation());
	}

	_lights.at(7)->setPosition(vec4(_car->getPositionX() + 3.5f * _car->getDirX() + 0.4f * _car->getDirZ(), 0.5, _car->getPositionZ() - 0.4f * _car->getDirX() + 3.5f * _car->getDirZ(), 1.0f));
	_lights.at(8)->setPosition(vec4(_car->getPositionX() + 3.5f * _car->getDirX() - 0.4f * _car->getDirZ(), 0.5, _car->getPositionZ() + 0.4f * _car->getDirX() + 3.5f * _car->getDirZ(), 1.0f));
	_lights.at(7)->setDirection(vec4(_car->getDirX(), -0.2f, _car->getDirZ(), 0.0f));
	_lights.at(8)->setDirection(vec4(_car->getDirX(), -0.2f, _car->getDirZ(), 0.0f));

	Light* _carleft_light = static_cast<Light*>(_lights[7]);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].isEnabled");
	glUniform1i(loc, _carleft_light->getState());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].isLocal");
	glUniform1i(loc, _carleft_light->getIsLocal());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].isSpot");
	glUniform1i(loc, _carleft_light->getIsSpot());

	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].ambient");
	faux[0] = _carleft_light->getAmbient().getX();
	faux[1] = _carleft_light->getAmbient().getY();
	faux[2] = _carleft_light->getAmbient().getZ();
	faux[3] = _carleft_light->getAmbient().getW();
	glUniform4fv(loc, 1, faux);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].diffuse");
	faux[0] = _carleft_light->getDiffuse().getX();
	faux[1] = _carleft_light->getDiffuse().getY();
	faux[2] = _carleft_light->getDiffuse().getZ();
	faux[3] = _carleft_light->getDiffuse().getW();
	glUniform4fv(loc, 1, faux);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].specular");
	faux[0] = _carleft_light->getSpecular().getX();
	faux[1] = _carleft_light->getSpecular().getY();
	faux[2] = _carleft_light->getSpecular().getZ();
	faux[3] = _carleft_light->getSpecular().getW();
	glUniform4fv(loc, 1, faux);

	faux[0] = _carleft_light->getPosition().getX();
	faux[1] = _carleft_light->getPosition().getY();
	faux[2] = _carleft_light->getPosition().getZ();
	faux[3] = _carleft_light->getPosition().getW();
	multMatrixPoint(VIEW, faux, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].position");
	glUniform4fv(loc, 1, res);
	faux[0] = _carleft_light->getDirection().getX();
	faux[1] = _carleft_light->getDirection().getY();
	faux[2] = _carleft_light->getDirection().getZ();
	faux[3] = _carleft_light->getDirection().getW();
	multMatrixPoint(VIEW, faux, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].direction");
	glUniform4fv(loc, 1, res);

	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].spotCosCutoff");
	glUniform1f(loc, cos(_carleft_light->getCutOff()));
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].spotExponent");
	glUniform1f(loc, _carleft_light->getExponent());

	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].constantAttenuation");
	glUniform1f(loc, _carleft_light->getConstantAttenuation());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].linearAttenuation");
	glUniform1f(loc, _carleft_light->getLinearAttenuation());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[7].quadraticAttenuation");
	glUniform1f(loc, _carleft_light->getQuadraticAttenuation());


	Light* _carright_light = static_cast<Light*>(_lights[8]);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].isEnabled");
	glUniform1i(loc, _carright_light->getState());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].isLocal");
	glUniform1i(loc, _carright_light->getIsLocal());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].isSpot");
	glUniform1i(loc, _carright_light->getIsSpot());

	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].ambient");
	faux[0] = _carright_light->getAmbient().getX();
	faux[1] = _carright_light->getAmbient().getY();
	faux[2] = _carright_light->getAmbient().getZ();
	faux[3] = _carright_light->getAmbient().getW();
	glUniform4fv(loc, 1, faux);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].diffuse");
	faux[0] = _carright_light->getDiffuse().getX();
	faux[1] = _carright_light->getDiffuse().getY();
	faux[2] = _carright_light->getDiffuse().getZ();
	faux[3] = _carright_light->getDiffuse().getW();
	glUniform4fv(loc, 1, faux);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].specular");
	faux[0] = _carright_light->getSpecular().getX();
	faux[1] = _carright_light->getSpecular().getY();
	faux[2] = _carright_light->getSpecular().getZ();
	faux[3] = _carright_light->getSpecular().getW();
	glUniform4fv(loc, 1, faux);


	faux[0] = _carright_light->getPosition().getX();
	faux[1] = _carright_light->getPosition().getY();
	faux[2] = _carright_light->getPosition().getZ();
	faux[3] = _carright_light->getPosition().getW();
	multMatrixPoint(VIEW, faux, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].position");
	glUniform4fv(loc, 1, res);

	faux[0] = _carright_light->getDirection().getX();
	faux[1] = _carright_light->getDirection().getY();
	faux[2] = _carright_light->getDirection().getZ();
	faux[3] = _carright_light->getDirection().getW();
	multMatrixPoint(VIEW, faux, res);
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].direction");
	glUniform4fv(loc, 1, res);

	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].spotCosCutoff");
	glUniform1f(loc, cos(_carright_light->getCutOff()));
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].spotExponent");
	glUniform1f(loc, _carright_light->getExponent());

	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].constantAttenuation");
	glUniform1f(loc, _carright_light->getConstantAttenuation());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].linearAttenuation");
	glUniform1f(loc, _carright_light->getLinearAttenuation());
	loc = glGetUniformLocation(shader.getProgramIndex(), "lights[8].quadraticAttenuation");
	glUniform1f(loc, _carright_light->getQuadraticAttenuation());
}

void createSquare() {

	float spec[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 1000.0f;
	int texcount = 0;
	objId = QUADID;


	memcpy(mesh[objId].mat.specular, spec, 4 * sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive, 4 * sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;
	createQuad(10.0f,10.0f);

}

void drawSquare() {
	GLint loc;
	objId = QUADID;

	float pos_X [] = { 30.0f, 74.3f ,63.6f, 20.0f };
	float pos_Z [] = { 20.0f, 14.3f, 83.6f, 60.0f };

	for (int i = 0; i < 4; i++) {

		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
		glUniform4fv(loc, 1, mesh[objId].mat.specular);
		loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
		glUniform1f(loc, mesh[objId].mat.shininess);

		pushMatrix(MODEL);

		translate(MODEL, pos_X[i], 4.8f, pos_Z[i]);
		billboardCylindrical(pos_X[i], 4.8f, pos_Z[i]);

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);

		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);


		glBindVertexArray(mesh[objId].vao);
		glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		popMatrix(MODEL);
	}
}

void createParticles() {
	int texcount = 0;
	float v, theta, phi;
	float z = 0;

	objId = PARTICLEID;
	mesh[objId].mat.texCount = texcount;
	createQuad(0.6f, 0.6f);

	for (int i = 0; i < MAX_PARTICLES; i++) {
		for (int x = 0; x < (MAX_PARTICLES - 0.8*MAX_PARTICLES); x++) {
			v = 0.8*frand() + 0.2;
			phi = frand()*M_PI;
			theta = 2.0*frand()*M_PI;

			if (z < TABLESIZE)
				z += 10;
			else z = 0;

			x += 15;

			_p.push_back(new Particle(1.0f, 0.005f, 1.0f, 1.0f, 1.0f, x - 8 , 22.0f, z, v * cos(theta) * sin(phi), v * cos(phi), v * sin(theta) * sin(phi), 0.1f, -0.15f, 0.0f));
		}
	}
}

void drawParticles() {
		GLint loc;
		objId = PARTICLEID;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);  //Depth Buffer Read Only


		for (int i = 0; i < _p.size(); i++){
			if (_p.at(i)->getLife() > 0.0f) { /* so desenha as que ainda estao vivas */

				/* A vida da part�cula representa o canal alpha da cor. Como o blend est� activo a cor final � a soma da cor rgb do fragmento multiplicada pelo
				alpha com a cor do pixel destino */

				particleColor[0] = _p.at(i)->getRed();
				particleColor[1] = _p.at(i)->getGreen();
				particleColor[2] = _p.at(i)->getBlue();
				particleColor[3] = _p.at(i)->getLife();

				// send the material - diffuse color modulated with texture
				loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
				glUniform4fv(loc, 1, particleColor);

				pushMatrix(MODEL);
				translate(MODEL, _p.at(i)->getX(), _p.at(i)->getY(), _p.at(i)->getZ());
				billboardCylindrical(_p.at(i)->getX(), _p.at(i)->getY(), _p.at(i)->getZ());

				// send matrices to OGL
				computeDerivedMatrix(PROJ_VIEW_MODEL);
				glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
				glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
				computeNormalMatrix3x3();
				glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

				glBindVertexArray(mesh[objId].vao);
				glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
				popMatrix(MODEL);
			}
			else _p.erase(_p.begin() + i);

		}

		glDepthMask(GL_TRUE); //make depth buffer again writeable
}


void createBillboardOr() {
	float pos_X[] = { 30.0f, 74.3f ,63.6f, 20.0f };
	float pos_Z[] = { 20.0f, 14.3f, 83.6f, 60.0f };
	for (int i = 0; i < 4; i++) {
		Orange* fakeOrange = new Orange(pos_X[i], 1.5, pos_Z[i]);
		fakeOrange->setName("FakeOrange");
		fakeOrange->setRadius(0.3f);
		_go.push_back(fakeOrange);
	}
}


// ------------------------------------------------------------
//
// Gameplay
//


void moveObjects() {
	_time = glutGet(GLUT_ELAPSED_TIME);
	double delta = _time - _oldtime;
	_oldtime = _time;
	if (paused) delta = 0;
	for (int i = 0; i < _go.size(); i++) {
		_go.at(i)->update(delta);
	}
	for (int e = 0; e < _go.size(); e++) {
		for (int i = e + 1; i < _go.size(); i++) {
			if (_go.at(e)->hasCollision(_go.at(i))) {
				_go.at(e)->treatCollision(_go.at(i));
				_go.at(i)->treatCollision(_go.at(e));
			}
		}
		_go.at(e)->setPosition();
		if (_go.at(e)->isDead()) {
			_go.erase(_go.begin() + e);
			e--;
		}
	}
}

void updateGame() {
	int _lives = _car->getLifes();
	if (_lives != 0) _score = _car->getScore();

	std::ostringstream oss;

	if (paused && _lives > 0) oss << CAPTION << ": GAME PAUSED - Press 'P' to CONTINUE";
	else if (_lives == 0) {
		paused = true;
		oss << CAPTION << " : GAME OVER - Press 'R' to RESTART | SCORE: " << _score;
	}
	else {
		oss << CAPTION  << " | SCORE: " << _score;
	}

	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
}

void resetGame() {
	_go.clear();
	_p.clear();
	createCheerios();
	createOranges();
	createCar();
	createWheels();
	createButter();
	createSquare();
	createHUD();
	createMainScreen();
	createLensFlare();
	createBillboardOr();
	paused = false;
	iniciated = false;
	snow = false;
	exists = false;
	lensFlare = false;
	_camera = 3;
}

void timer(int value)
{
    FrameCount = 0;
    glutTimerFunc(1000, timer, 0);
}

void refresh(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000/60, refresh, 0);
}



// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);

	/************** STENCIL BUFFER *****************/
	loadIdentity(PROJECTION);
	if (w <= h)
		ortho(-2.0, 2.0, -2.0*(GLfloat)h / (GLfloat)w,
			2.0*(GLfloat)h / (GLfloat)w, -10, 10);
	else
		ortho(-2.0*(GLfloat)w / (GLfloat)h,
			2.0*(GLfloat)w / (GLfloat)h, -2.0, 2.0, -10, 10);

	glUseProgram(shader.getProgramIndex());
	drawStencilObject();

	/************** STENCIL BUFFER *****************/

	// set the projection matrix
	ratio = (1.0f * w) / h;
	loadIdentity(PROJECTION);

}


// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {

	GLint loc;
	FrameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (night == false) {
		//glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
		fogColor[0] = 0.6f;
		fogColor[1] = 0.6f;
		fogColor[2] = 1.0f;
	}
	else {
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		fogColor[0] = 0.0f;
		fogColor[1] = 0.0f;
		fogColor[2] = 0.05f;
	}


	moveObjects();

	if (_camera == ORTHOCAMERAID) _orthoCamera->setCamera();
	else if (_camera == PERSPECTIVECAMERAID) _perspCamera->setCamera();
	else if (_camera == PERSPECTIVECARCAMERAID) {
		_perspCamera->updateCamera(_car->getPositionX() - 10 * _car->getDirX(), _car->getPositionY() + 10, _car->getPositionZ() - 10 * _car->getDirZ(),
			_car->getPositionX() + 10 * _car->getDirX(), _car->getPositionY() - 1, _car->getPositionZ() + 10 * _car->getDirZ(),
			0, 1, 0);
	}

	// use our shader
	glUseProgram(shader.getProgramIndex());

	//Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureArray[2]);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureArray[3]);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TextureArray[4]);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, TextureArray[5]);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, TextureArray[6]);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, TextureArray[7]);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, TextureArray[8]);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, TextureArray[9]);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, TextureArray[10]);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, TextureArray[11]);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, TextureArray[12]);

  //fog
	glUniform3fv(skyColor, 1, fogColor);
	if (!paused) glUniform1i(fogMode, fogStatus);
	else glUniform1i(fogMode, false);

	setupLights();

	glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glEnable(GL_CULL_FACE);

	glUniform1i(texMode_uniformId, 1);
	glUniform1i(tex_loc, 0);
	drawTable();

	/*glUniform1i(texMode_uniformId, 0);
	drawCar();
	drawWheels();*/

	glUniform1i(texMode_uniformId, 1);
	glUniform1i(tex_loc, 3);
	drawButter();

	glUniform1i(texMode_uniformId, 1);
	glUniform1i(tex_loc, 0);
	drawObjCar();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform1i(texMode_uniformId, 3);
	glUniform1i(tex_loc2, 2);
	drawSquare();

	drawHUD();

	if (snow) {
		glUniform1i(texMode_uniformId, 4);
		glUniform1i(tex_loc2, 4);
		drawParticles();
		if (iniciated == false)
			start();
			iniciated = true;
	}


	if (elapsedSeconds() < 5.0){}
	else {
		stop();
		createParticles();
		iniciated = false;
	}


	glUniform1i(texMode_uniformId, 0);

	/*** objects drawn in this section won't suffer the stencil effect ***/
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	drawCheerios();
	glDepthMask(FALSE);
	
	if (paused) {
		_camera = ORTHOCAMERAID;
		drawMainScreen(); 
	}
						  /**************/
	glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
	if (lensFlare && !paused && !night && !fogStatus) drawLensFlare();

	glDepthMask(TRUE);

	glUniform1i(texMode_uniformId, 2);
	glUniform1i(tex_loc1, 1);
	drawOranges(); //the transparent objects need to be drawn last

	updateGame();

	glBindTexture(GL_TEXTURE_2D, 0);
	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void keyUp(unsigned char key, int xx, int yy) {
	switch (key) {
		case 'w': case 'W': //seta cima
			_car->setAceleration(-_aceleration2);
			break;
		case 's': case 'S': //seta baixo
			_car->setAceleration(_aceleration2);
			break;
	}
}

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

		case 27:
			glutLeaveMainLoop();
			break;

		case '1':
			if (_camera != ORTHOCAMERAID) {
				_camera = ORTHOCAMERAID;
				renderScene();
			}
			break;

		case '2':
			if (_camera != PERSPECTIVECAMERAID) {
				_camera = PERSPECTIVECAMERAID;
				renderScene();
			}
			break;

		case '3':
			if (_camera != PERSPECTIVECARCAMERAID) {
				_camera = PERSPECTIVECARCAMERAID;
				renderScene();
			}
			break;

		case 's': case 'S':
			if (!paused) {
				if (_car->getSpeed() > 0) _car->setAceleration(-_aceleration3);
				else _car->setAceleration(-_aceleration);
			}
			break;

		case 'w': case 'W':
			if (!paused) {
				if (_car->getSpeed() < 0) _car->setAceleration(_aceleration3);
				else _car->setAceleration(_aceleration);
			}
			break;

		case 'd': case 'D':
			if (!paused) _car->increaseRotation(3);
			break;

		case 'a': case 'A':
			if (!paused) _car->increaseRotation(-3);
			break;

		case 'p': case 'P':
			if (!paused) _oldcamera = _camera;
			else _camera = _oldcamera; //trick to restablish old camera
			paused = !paused;
			break;

		case 'n': case 'N':
			_lights.at(0)->setState(!_lights.at(0)->getState());
			for (int i = 1; i < 7; i++) _lights.at(i)->setState(true);

			night = !night;
			if (night) glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
			else glClearColor(0.6f, 0.6f, 1.0f, 1.0f);
			break;

		case 'c': case 'C':
			for (int i = 1; i < 7;i ++) _lights.at(i)->setState(!_lights.at(i)->getState());
			break;

		case 'h': case 'H':
			_lights[7]->setState(!_lights[7]->getState());
			_lights[8]->setState(!_lights[8]->getState());
			break;

		case 'r': case 'R':
			resetGame();
			break;
		case 't': case 'T':
			stencil = !stencil;
			if (stencil) glEnable(GL_STENCIL_TEST);
			else glDisable(GL_STENCIL_TEST);
			break;

		case 'g': case 'G':
			snow = !snow;
			if (snow) {
				createParticles();
				glutTimerFunc(0, iterate, 0);  //timer for particle system
				exists = 1;
			}
			else
				if (exists) {
					_p.clear();
					exists = 0;
				}
			break;
		case 'f': case 'F':
				fogStatus = !fogStatus;
				break;

		case 'm': case 'M':
			sound = !sound;
			if (sound) {
				PlaySound("sounds/music.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
			}
			else 
				PlaySound(NULL, NULL, SND_APPLICATION);
			break;
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON) {
			tracking = 1;
			lensFlare = TRUE;
			// Scale mouse coordinates to compensate for window size.
			xFlare += (xx - xMouse)*WinX / glutGet(GLUT_WINDOW_WIDTH);
			yFlare += (yy - yMouse)*WinY / glutGet(GLUT_WINDOW_HEIGHT);
		
			// Clamping -- wouldn't be needed in fullscreen mode.
			if (xFlare >= WinX)
				xFlare = WinX - 1;
			if (xFlare < 0)
				xFlare = 0;
			if (yFlare >= WinY)
				yFlare = WinY - 1;
			if (yFlare < 0)
				yFlare = 0;
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			tracking = 2;
			lensFlare = false;
		}
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) 
		tracking = 0;
	// Remember last mouse position.
	xMouse = xx;
	yMouse = yy;
}

// Track mouse motion while buttons are pressed

void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX = -xx + startX;
	deltaY = yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {
		/***********flares***************/
		xMouse = xx;
		yMouse = yy;

		xFlare = xx*WinX / glutGet(GLUT_WINDOW_WIDTH);
		yFlare = yy*WinY / glutGet(GLUT_WINDOW_HEIGHT);
		/********************************/

	}
}

// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for models
	shader.init();
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/directional.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/directional.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	//"position" e "normal" -> basic_geometry
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");

	glLinkProgram(shader.getProgramIndex());

	texMode_uniformId = glGetUniformLocation(shader.getProgramIndex(), "texMode"); // different modes of texturing
	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texmap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texmap1");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texmap2");
	skyColor = glGetUniformLocation(shader.getProgramIndex(), "skyColor");
	fogMode = glGetUniformLocation(shader.getProgramIndex(), "fogMode");

	printf("InfoLog for Hello World Shader\n%s\n\n", shader.getAllInfoLogs().c_str());

	return(shader.isProgramValid());
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void init(){

	//Objects
	createLights();
	createTable();
	createCheerios();
	createOranges();
	createCar();
	createWheels();
	createButter();
	createStencilObject();
	createSquare();
	createHUD();
	createMainScreen();
	createLensFlare();
	createBillboardOr();
	createObj();


	//Cameras
	_orthoCamera = new OrthoCamera(-10, TABLESIZE + 10, -10, TABLESIZE + 10, 0, 100);
	_perspCamera = new PerspectiveCamera(75.0f, 1.0f, 0.1f, 1000.0f); //NEAR NAO PODE SER 0
	_perspCarCamera = new PerspectiveCamera(75.0f, 1.0f, 0.1f, 1000.0f);
	_camera = 3;


	//Texture Object definition
	glGenTextures(NUM_TEXTURES, TextureArray);

	TGA_Texture(TextureArray, "textures/snow_floor.tga", 0);
	TGA_Texture(TextureArray, "textures/xmasBall.tga", 1);
	TGA_Texture(TextureArray, "textures/snowman.tga", 2);
	TGA_Texture(TextureArray, "textures/present.tga", 3);
	TGA_Texture(TextureArray, "textures/snowflake.tga", 4);
	TGA_Texture(TextureArray, "textures/road.tga", 5);
	TGA_Texture(TextureArray, "textures/Flare1.tga", 6);
	TGA_Texture(TextureArray, "textures/Flare2.tga", 7);
	TGA_Texture(TextureArray, "textures/Flare3.tga", 8);
	TGA_Texture(TextureArray, "textures/paused.tga", 9);
	TGA_Texture(TextureArray, "textures/gameover.tga", 10);
	TGA_Texture(TextureArray, "textures/star.tga", 11);
	TGA_Texture(TextureArray, "textures/dedstar.tga", 12);



	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.6f, 0.6f, 1.0f, 1.0f);

	glClearStencil(0x0);
	glEnable(GL_STENCIL_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

	std::srand(std::time(0));

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_STENCIL|GLUT_MULTISAMPLE);

	glutInitContextVersion (3, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	//glutIdleFunc(renderScene);

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutTimerFunc(0,timer,0);
	glutTimerFunc(0, refresh, 0);


//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));


	if (!setupShaders()) {
		return(1);
	}

	printf("Instructions:\n");
	printf("Use the w, a, s, d to navigate, 1, 2 & 3 to change cameras\n");
	printf("Lights: n for night mode, c for candles , h to switch car headlights\n");
	printf("Snow: press g");
	printf("Extra: t to activate stencil random square\n");
	printf("Press m for music\n\n");

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);

}
