#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "trackball.h"
#include "sphere.h"

//*************************************
// global constants
static const char* b_vert_shader_path = "../bin/shaders/circ.vert";
static const char* b_frag_shader_path = "../bin/shaders/circ.frag";

uint				NUM_TESS = 72;		//w initial tessellation factor of the sphere as a polygon

Texture	b_image;

//*******************************************************************
// common structures
struct camera
{
	vec3	eye = vec3(-10.0f, 2.0f, 5.0f);
	vec3	at = vec3(0);
	vec3	up = vec3(0, 1.0f, 0);
	mat4	view_matrix = mat4::rotate(vec3(0, 0, 1), PI / 2) * mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect_ratio;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};

struct light_t
{
	vec4	position = vec4(0.0f, 0.0f, 0.0f, 1.0f);   // directional light
	vec4	ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4	diffuse = vec4(0.9f, 0.9f, 0.9f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	float	shininess = 1000.0f;
};

//*************************************
// OpenGL objects
GLuint	background = 0;	// ID holder for GPU background
GLuint	b_vertex_buffer = 0;	// ID holder for vertex buffer
GLuint	quad_vert_buffer = 0;
GLuint	index_buffer = 0;	// ID holder for index buffer

//*************************************
// global variables
float	t = 0.0f;						// current simulation parameter
float	speed_scale = 2.0f;
uint	b_solid_color = 0;			// use sphere's color?
bool	b_index_buffer = true;			// use index buffering?
bool	b_wireframe = false;

bool	shading = false;
auto	spheres = std::move(create_spheres());
struct { bool add = false, sub = false; operator bool() const { return add || sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit sphere
std::vector<vertex>	unit_sphere_vertices;	// host-side vertices
trackball tb = trackball(speed_scale);
camera cam;
light_t light;
material_t material;

GLuint	b_SRC = 0;


std::vector<vertex> create_sphere_vertices(uint N)
{
	std::vector<vertex> v;
	for (uint k = 0; k <= N; k++)
	{
		float lngi = PI * 2.0f * k / float(N), lns = sin(lngi), lnc = cos(lngi);
		for (uint i = 0; i <= N / 2; i++)
		{
			float lati = PI * 1.0f * i / float(N / 2), lac = cos(lati), las = sin(lati);
			v.push_back({ vec3(las * lnc, las * lns ,lac), vec3(las * lnc, las * lns ,lac), vec2(lngi / (2 * PI), 1.0f - lati / PI) });
		}
	}
	return v;
}

void update_b_vertex_buffer(const std::vector<vertex>& vertices, uint N)
{
	// clear and create new buffers
	if (b_vertex_buffer)	glDeleteBuffers(1, &b_vertex_buffer);	b_vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if (b_index_buffer)
	{
		std::vector<uint> indices;

		for (uint k = 0; k < N + 2; k++)
		{
			uint n = N / 2;
			for (uint i = 0; i < n; i++)
			{
				indices.push_back(k * n + i);
				indices.push_back(k * n + i + 1);
				indices.push_back((k + 1) * n + i + 1);
				indices.push_back(k * n + i + 1);
				indices.push_back((k + 1) * n + i + 2);
				indices.push_back((k + 1) * n + i + 1);
			}
		}

		// generation of vertex buffer: use vertices as it is
		glGenBuffers(1, &b_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, b_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	}
}


bool b_user_init()
{
	if (!(background = cg_create_program(b_vert_shader_path, b_frag_shader_path))) { glfwTerminate(); return 1; }

	unit_sphere_vertices = std::move(create_sphere_vertices(NUM_TESS));
	update_b_vertex_buffer(unit_sphere_vertices, NUM_TESS);

	glActiveTexture(GL_TEXTURE0);

	b_image.window_size = ivec2(1024,512);

	b_image.load("../bin/images/2k_sun.jpg");
	b_image.load("../bin/images/2k_mercury.jpg");
	b_image.load("../bin/images/2k_venus.jpg");
	b_image.load("../bin/images/2k_earth.jpg");
	b_image.load("../bin/images/2k_mars.jpg");
	b_image.load("../bin/images/2k_jupiter.jpg");
	b_image.load("../bin/images/2k_saturn.jpg");
	b_image.load("../bin/images/2k_uranus.jpg");
	b_image.load("../bin/images/2k_neptune.jpg");

	return true;
}


void b_update()
{
	t = float(glfwGetTime()) * 0.4f;
	// update projection matrix
	cam.aspect_ratio = 1024 / float(512);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dnear, cam.dfar);

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation(background, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, b_solid_color);
	uloc = glGetUniformLocation(background, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(background, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);


	glUniform4fv(glGetUniformLocation(background, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(background, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(background, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(background, "Is"), 1, light.specular);

	// setup material properties
	glUniform4fv(glGetUniformLocation(background, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(background, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(background, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(background, "shininess"), material.shininess);

	glUniform1i(glGetUniformLocation(background, "shading"), shading);
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (b_vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, b_vertex_buffer);
	if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	// bind vertex attributes to your shader background
	cg_bind_vertex_attributes(background);

	int i = 0;
	// render two spheres: trigger shader background to process vertex data
	for (auto& s : spheres)
	{
		// per-sphere update
		s.update(t);

		// update per-sphere uniforms
		GLint uloc;
		uloc = glGetUniformLocation(background, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, s.model_matrix);

		glBindTexture(GL_TEXTURE_2D, b_image.texture[i]);
		glUniform1i(glGetUniformLocation(background, "TEX"), 0);


		glUniform1i(glGetUniformLocation(background, "is_not_star"), s.type);
		// per-sphere draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, (NUM_TESS + 1) * (NUM_TESS + 1) * 3, GL_UNSIGNED_INT, nullptr);
		i++;
	}

}

void background_render()
{
	glUseProgram(background);
	
	b_user_init();
	b_update();
	draw();
}


