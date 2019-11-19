#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

#include "sphere.h"			// sphere class definition


//*************************************
// global constants
static const char*	window_name = "cgcirc";
static const char*	vert_shader_path = "../bin/shaders/planet.vert";
static const char*	frag_shader_path = "../bin/shaders/planet.frag";

bool	shading = false;
int		MIN_QUANTIZATION = 0;
int		NUM_QUANTIZATION = 1;
uint	NUM_TESS = 72;		//w initial tessellation factor of the sphere as a polygon

struct light_t
{
	vec4	position = vec4(3.0f, 2.0f, 2.0f, 0.0f);   // directional light
	vec4	ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4	diffuse = vec4(0.9f, 0.9f, 0.9f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4	diffuse = vec4(0.35f, 0.4f, 0.75f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
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
float	t = 0.0f;			
uint	b_solid_color = 0;			// use sphere's color?
bool	is_rotate = false;
bool	b_wireframe = false;
auto	spheres = std::move(create_spheres());
struct { bool add=false, sub=false; operator bool() const { return add||sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit sphere
std::vector<vertex>	unit_sphere_vertices;	// host-side vertices
light_t	light;
material_t	material;

//*************************************
void update()
{
	// update global simulation parameter
	if (is_rotate) glfwSetTime(t);
	else t = float(glfwGetTime());

	// tricky aspect correction matrix for non-square window
	float aspect = window_size.x/float(window_size.y);

	mat4 aspect_matrix =
	{
		min(1 / aspect,1.0f), 0, 0, 0,
		0, min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 view_projection_matrix =
	{ 
		0,1,0,0,
		0,0,1,0,
		-1,0,0,1,
		0,0,0,1 
	};
	
	// update common uniform variables in vertex/fragment shaders
	GLint uloc;

	uloc = glGetUniformLocation(program, "aspect_matrix");				if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, aspect_matrix);
	uloc = glGetUniformLocation(program, "view_projection_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, view_projection_matrix);
	uloc = glGetUniformLocation( program, "b_solid_color" );			if(uloc>-1) glUniform1i( uloc, b_solid_color );
	
	// setup light properties
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
	glUniform1i(glGetUniformLocation(program, "qnti"), NUM_QUANTIZATION);
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program and buffers
	glUseProgram( program );
	if(index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes( program );

	// render two spheres: trigger shader program to process vertex data
	for( auto& s : spheres )
	{
		// per-sphere update
		s.update(t);

		// update per-sphere uniforms
		GLint uloc;
		uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, s.model_matrix );

		glDrawElements( GL_TRIANGLES, (NUM_TESS+1)*(NUM_TESS+1)*3, GL_UNSIGNED_INT, nullptr );
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
	printf( "- press 's' to toggle between shading and texture coordinates\n" );
	printf( " - press 'r' to rotate the sphere\n" );
	printf( "- press '+' or '-' to convert to cel shading or arrange quantizationz");
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

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
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
		else if (key == GLFW_KEY_S)
		{
			shading = !shading;
			NUM_QUANTIZATION = 1;
			printf("\n> using %s mode\n", shading ? "shading" : "texture");
		}
		else if (key == GLFW_KEY_R)
		{	
			is_rotate = !is_rotate;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT)))	b.add = false;
		else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) b.sub = false;
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		printf("> Left mouse button pressed at (%d, %d)\n", int(pos.x), int(pos.y));
	}
}

void motion(GLFWwindow* window, double x, double y)
{
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

	// define the position of four corner vertices
	unit_sphere_vertices = std::move(create_sphere_vertices(NUM_TESS));

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer(unit_sphere_vertices, NUM_TESS);

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
