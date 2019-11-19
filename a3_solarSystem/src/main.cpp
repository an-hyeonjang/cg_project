#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "trackball.h"

#include "sphere.h"			// sphere class definition

//*************************************
// global constants
static const char*	window_name = "cgcirc";
static const char*	vert_shader_path = "../bin/shaders/circ.vert";
static const char*	frag_shader_path = "../bin/shaders/circ.frag";

static const uint	MIN_TESS = 3;		// minimum tessellation factor (down to a triangle)
static const uint	MAX_TESS = 256;		// maximum tessellation factor (up to 64 triangles)
uint				NUM_TESS = 72;		//w initial tessellation factor of the sphere as a polygon

//*************************************
// include stb_image with the implementation preprocessor definition
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//*******************************************************************
// common structures
struct camera
{
	vec3	eye = vec3(-10.0f , 0.0f, 0.0f);
	vec3	at = vec3(0);
	vec3	up = vec3(0, 1.0f, 0);
	mat4	view_matrix = mat4::rotate(vec3(0, 0, 1), PI / 2) *mat4::look_at(eye, at, up);

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
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2( 1024, 576 );	// initial window size

//*************************************
// OpenGL objects
GLuint	program			= 0;	// ID holder for GPU program
GLuint	vertex_buffer	= 0;	// ID holder for vertex buffer
GLuint	index_buffer	= 0;	// ID holder for index buffer

//*************************************
// global variables
int		frame = 0;						// index of rendering frames
float	t = 0.0f;						// current simulation parameter
float	speed_scale = 2.0f;
uint	b_solid_color = 0;			// use sphere's color?
bool	b_index_buffer = true;			// use index buffering?
bool	b_wireframe = false;

bool	shading = false;
int		MIN_QUANTIZATION = 0;
int		NUM_QUANTIZATION = 1;

auto	spheres = std::move(create_spheres());
struct { bool add=false, sub=false; operator bool() const { return add||sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit sphere
std::vector<vertex>	unit_sphere_vertices;	// host-side vertices
trackball tb = trackball(speed_scale);
camera cam;
light_t light;
material_t material;

//*************************************
//texture set
static std::vector<const char*> texture_path;
std::vector<GLuint> texture;
GLuint texture1;

//*************************************
void update()
{
	t = float(glfwGetTime()) * 0.4f;
	// update projection matrix
	cam.aspect_ratio = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect_ratio, cam.dnear, cam.dfar );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "b_solid_color" );		if (uloc > -1) glUniform1i(uloc, b_solid_color);
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );


	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

	glUniform1i(glGetUniformLocation(program, "shading"), shading);
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program and buffers
	glUseProgram( program );
	if(vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	if(index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes( program );

	int i = 0;
	// render two spheres: trigger shader program to process vertex data
	for( auto& s : spheres )
	{
		// per-sphere update
		s.update(t);

		// update per-sphere uniforms
		GLint uloc;
		uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, s.model_matrix );
		
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);

		
		glUniform1i(glGetUniformLocation(program, "is_not_star"), s.type);
		// per-sphere draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, (NUM_TESS + 1) * (NUM_TESS + 1) * 3, GL_UNSIGNED_INT, nullptr);
		i++;
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press 'd' to toggle between solid color and texture coordinates\n" );
	printf( "- press 's' to toggle between texture coordinate color and texture\n");
	printf( "- press 'w' to toggle wireframe\n" );
	printf( "\n" );
}

std::vector<vertex> create_sphere_vertices( uint N )
{
	std::vector<vertex> v; 
	for( uint k=0; k <= N; k++ )
	{
		float lngi = PI * 2.0f * k / float(N), lns = sin(lngi), lnc = cos(lngi);
		for ( uint i=0; i <= N/2; i++ )
		{
			float lati = PI * 1.0f * i / float(N/2), lac = cos(lati), las = sin(lati);
			v.push_back({ vec3(las * lnc, las * lns ,lac), vec3(las * lnc, las * lns ,lac), vec2(lngi / (2* PI), 1.0f - lati / PI)});
		}
	}
	return v;
}

void update_vertex_buffer( const std::vector<vertex>& vertices, uint N )
{
	// clear and create new buffers
	if(vertex_buffer)	glDeleteBuffers( 1, &vertex_buffer );	vertex_buffer = 0;
	if(index_buffer)	glDeleteBuffers( 1, &index_buffer );	index_buffer = 0;

	// check exceptions
	if(vertices.empty()){ printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if(b_index_buffer)
	{
		std::vector<uint> indices;

		for( uint k = 0; k < N+2; k++ )
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
		glGenBuffers( 1, &vertex_buffer );
		glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers( 1, &index_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices.size(), &indices[0], GL_STATIC_DRAW );
	}
}

void update_tess()
{
	uint n = NUM_TESS; if (b.add) n++; if (b.sub) n--;
	if (n == NUM_TESS || n<MIN_TESS || n>MAX_TESS) return;

	unit_sphere_vertices = create_sphere_vertices(NUM_TESS = n);
	update_vertex_buffer(unit_sphere_vertices, NUM_TESS);
	printf("> NUM_TESS = % -4d\r", NUM_TESS);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT)))	b.add = true;
		else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) b.sub = true;
		else if (key == GLFW_KEY_D)
		{
			++b_solid_color %= 3;
			switch (b_solid_color)
			{
			case 0: printf("> using vec4(tc.xy,0,1)\n"); break;
			case 1: printf("> using vec4(tc.xxx,1)\n"); break;
			case 2: printf("> using vec4(tc.yyy,1)\n"); break;
			}
		}
		else if (key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT)))
		{
			if (!shading) return;
			NUM_QUANTIZATION++;
			printf("> using cel shading .. Quatization Number = % -4d\r", NUM_QUANTIZATION);
		}
		else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS)
		{
			if (!shading || NUM_QUANTIZATION <= MIN_QUANTIZATION) return;
			NUM_QUANTIZATION--;
			printf("> using cel shading .. Quatization Number = % -4d\r", NUM_QUANTIZATION);
		}
		else if (key == GLFW_KEY_S)
		{
			shading = !shading;
			NUM_QUANTIZATION = 1;
			printf("\n> using %s mode\n", shading ? "texture" : "tex coordinate");
		}
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
		else if (key == GLFW_KEY_D)
		{
			++b_solid_color %= 3;
			switch (b_solid_color)
			{
			case 0: printf("> using vec4(tc.xy,0,1)\n"); break;
			case 1: printf("> using vec4(tc.xxx,1)\n"); break;
			case 2: printf("> using vec4(tc.yyy,1)\n"); break;
			}
		}
		else if (key == GLFW_KEY_HOME)
			cam.view_matrix = mat4::rotate(vec3(0, 0, 1), PI / 2) * mat4::look_at(cam.eye, cam.at, cam.up);
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT)))	b.add = false;
		else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) b.sub = false;
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
	vec2 npos = vec2(float(pos.x) / float(window_size.x - 1), float(pos.y) / float(window_size.y - 1));
	if (action == GLFW_PRESS)
	{
		tb.button = button;
		tb.mods = mods;
		tb.begin(cam.view_matrix, npos.x, npos.y);
	}
	else if (action == GLFW_RELEASE)	tb.end();
	
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking()) return;
	vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
	if (tb.button == GLFW_MOUSE_BUTTON_LEFT)
		cam.view_matrix = tb.t_update(npos.x, npos.y);
	if (tb.button == GLFW_MOUSE_BUTTON_MIDDLE || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods & GLFW_MOD_CONTROL)))
		cam.view_matrix = tb.p_update(cam.eye, cam.at, cam.up, npos.x, npos.y);
	if (tb.button == GLFW_MOUSE_BUTTON_RIGHT || (tb.button == GLFW_MOUSE_BUTTON_LEFT && (tb.mods & GLFW_MOD_SHIFT)))
		cam.view_matrix = tb.z_update(npos.x, npos.y);
}

void texture_load(const char* texture_path, GLuint& texture) {

	int width, height, comp = 3;
	unsigned char* pimage0 = stbi_load(texture_path, &width, &height, &comp, 3); if (comp == 1) comp = 3; /* convert 1-channel to 3-channel image */
	int stride0 = width * comp, stride1 = (stride0 + 3) & (~3);	// 4-byte aligned stride
	unsigned char* pimage = (unsigned char*)malloc(sizeof(unsigned char) * stride1 * height);
	for (int y = 0; y < height; y++) memcpy(pimage + (height - 1 - y) * stride1, pimage0 + y * stride0, stride0); // vertical flip

	// create textures
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8 /* GL_RGB for legacy GL */, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pimage);

	// allocate and create mipmap
	int mip_levels = miplevels(window_size.x, window_size.y);
	for (int k = 1, w = width >> 1, h = height >> 1; k < mip_levels; k++, w = max(1, w >> 1), h = max(1, h >> 1))
		glTexImage2D(GL_TEXTURE_2D, k, GL_RGB8 /* GL_RGB for legacy GL */, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	// configure texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// release the new image
	free(pimage);
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests
	glEnable(GL_TEXTURE_2D);

	unit_sphere_vertices = std::move(create_sphere_vertices(NUM_TESS));
	update_vertex_buffer(unit_sphere_vertices, NUM_TESS);

	glActiveTexture(GL_TEXTURE0);

	texture_path.push_back("../bin/images/2k_sun.jpg");
	texture_path.push_back("../bin/images/2k_mercury.jpg");
	texture_path.push_back("../bin/images/2k_venus.jpg");
	texture_path.push_back("../bin/images/2k_earth.jpg");
	texture_path.push_back("../bin/images/2k_mars.jpg");
	texture_path.push_back("../bin/images/2k_jupiter.jpg");
	texture_path.push_back("../bin/images/2k_saturn.jpg");
	texture_path.push_back("../bin/images/2k_uranus.jpg");
	texture_path.push_back("../bin/images/2k_neptune.jpg");

	for (uint i = 0; i < texture_path.size(); i++)
	{
		texture.push_back(0);
		texture_load(texture_path[i], texture[i]);
	}

	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// initialization
	if(!glfwInit()){ printf( "[error] failed in glfwInit()\n" ); return 1; }

	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movements

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
