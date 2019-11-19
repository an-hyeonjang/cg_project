#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "trackball.h"

//*************************************
// global constants
static const char*	window_name = "cgbase - shading";
static const char*	vert_shader_path = "../bin/shaders/shading.vert";
static const char*	frag_shader_path = "../bin/shaders/shading.frag";
static const char* stenf_shader_path = "../bin/shaders/stencil.frag";
static const char*	mesh_vertex_path = "../bin/mesh/head.vertex.bin";	// http://graphics.cs.williams.edu/data/meshes.xml
static const char*	mesh_index_path	= "../bin/mesh/head.index.bin";		// http://graphics.cs.williams.edu/data/meshes.xml

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 50, -100, 10 );
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 0, 1 );
	mat4	view_matrix = mat4::look_at( eye, at, up );
		
	float	fovy = PI/4.0f; // must be in radian
	float	aspect_ratio;
	float	dNear = 1.0f;
	float	dFar = 1000.0f;
	mat4	projection_matrix;
};

struct light_t
{
	vec4	position = vec4( 1.0f, -1.0f, 1.0f, 0.0f );   // directional light
    vec4	ambient  = vec4( 0.2f, 0.2f, 0.2f, 1.0f );
    vec4	diffuse  = vec4( 0.8f, 0.8f, 0.8f, 1.0f );
    vec4	specular = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
};

struct material_t
{
	vec4	ambient  = vec4( 0.2f, 0.2f, 0.2f, 1.0f );
    vec4	diffuse  = vec4( 0.8f, 0.8f, 0.8f, 1.0f );
    vec4	specular = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	float	shininess = 1000.0f;
};

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2( 720, 480 );	// initial window size

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program
GLuint	stencil = 0;

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
bool	stencil_option = false;	// mouse left click turns on outlining
bool	stencil_methods = true;	// decides whether methods to use in outlining
uint	NUM_QUANTI = 4;
uint	MIN_QUANTI = 1;

//*************************************
// scene objects
mesh*		pMesh = nullptr;
camera		cam;
trackball	tb;
light_t		light;
material_t	material;

void	stencil_render_linewidth();
void	stencil_render_scale();

//*************************************
void update()
{
	// update projection matrix
	cam.aspect_ratio = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect_ratio, cam.dNear, cam.dFar );

	// build the model matrix for oscillating scale
	float t = float(glfwGetTime());
	float scale	= 1.0f+float(cos(t*1.5f))*0.05f;
	mat4 model_matrix = mat4::scale( scale, scale, scale );


	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	uloc = glGetUniformLocation( program, "model_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, model_matrix );

	// setup light properties
	glUniform4fv( glGetUniformLocation( program, "light_position" ), 1, light.position );
	glUniform4fv( glGetUniformLocation( program, "Ia" ), 1, light.ambient );
	glUniform4fv( glGetUniformLocation( program, "Id" ), 1, light.diffuse );
	glUniform4fv( glGetUniformLocation( program, "Is" ), 1, light.specular );

    // setup material properties
	glUniform4fv( glGetUniformLocation( program, "Ka" ), 1, material.ambient );
	glUniform4fv( glGetUniformLocation( program, "Kd" ), 1, material.diffuse );
	glUniform4fv( glGetUniformLocation( program, "Ks" ), 1, material.specular );
	glUniform1f( glGetUniformLocation( program, "shininess" ), material.shininess );

	glUniform1ui(glGetUniformLocation(program, "qti"), NUM_QUANTI);
}

void render_0()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// notify GL that we use our own program and buffers
	glUseProgram(program);
	if(pMesh&&pMesh->vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, pMesh->vertex_buffer);
	if(pMesh&&pMesh->index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);
	
	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes( program );
	
	// render vertices: trigger shader programs to process vertex data
	glDrawElements(GL_TRIANGLES, pMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);
	if (stencil_option) stencil_render_linewidth();
	else glfwSwapBuffers(window);
}

void stencil_render_linewidth()
{
	update();

	glStencilFunc(GL_NOTEQUAL, 1, -1);
	glLineWidth(1);

	glUseProgram(stencil);
	if (pMesh && pMesh->vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, pMesh->vertex_buffer);
	if (pMesh && pMesh->index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);

	float t = float(glfwGetTime());
	glUniform1f(glGetUniformLocation(stencil, "t"), cos(t * 3.0f));

	cg_bind_vertex_attributes(stencil);

	glDrawElements(GL_LINES, pMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);
	glfwSwapBuffers(window);
}


void render_1()
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilMask(1);

	// notify GL that we use our own program and buffers
	glUseProgram(program);
	if (pMesh && pMesh->vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, pMesh->vertex_buffer);
	if (pMesh && pMesh->index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes(program);

	// render vertices: trigger shader programs to process vertex data
	glDrawElements(GL_TRIANGLES, pMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);
	if (stencil_option) stencil_render_scale();
	else glfwSwapBuffers(window);
}

void stencil_render_scale()
{
	update();

	glStencilFunc(GL_NOTEQUAL, 1, 1);
	glStencilMask(-1);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(stencil);
	if (pMesh && pMesh->vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, pMesh->vertex_buffer);
	if (pMesh && pMesh->index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->index_buffer);

	float t = float(glfwGetTime());
	float scale = 1.07f + float(cos(t * 1.5f)) * 0.05f;
	mat4 model_matrix = mat4::scale(scale, scale, scale);

	glUniformMatrix4fv(glGetUniformLocation(stencil, "model_matrix"), 1, GL_TRUE, model_matrix);
	glUniform1f(glGetUniformLocation(stencil, "t"), cos(t * 3.0f));

	cg_bind_vertex_attributes(stencil);

	glDrawElements(GL_TRIANGLES, pMesh->index_list.size(), GL_UNSIGNED_INT, nullptr);
	glStencilMask(0xFF);
	glEnable(GL_DEPTH_TEST);

	glfwSwapBuffers(window);
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
	printf("- press Home to reset camera\n");
	printf( "- press '+' or '-' to arrange Quantization Number\n" );
	printf("- press 's' to change stencil method\n");
	printf( "- please click your mouse left buttom\n");
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)					cam = camera();
		else if (key == GLFW_KEY_S)
		{
			stencil_methods = !stencil_methods;
			printf("> using %s methods\n", stencil_methods ? "scaling" : "line width");
		}
		else if (key == GLFW_KEY_KP_ADD || (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT)))
		{
			NUM_QUANTI++;

			printf("> Quatization Number = % -4d\r", NUM_QUANTI);
		}
		else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS)
		{
			if (NUM_QUANTI <= MIN_QUANTI) return;
			NUM_QUANTI--;
			printf("> Quatization Number = % -4d\r", NUM_QUANTI);
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = vec2( float(pos.x)/float(window_size.x-1), float(pos.y)/float(window_size.y-1) );
		if (action == GLFW_PRESS)
		{
			tb.begin(cam.view_matrix, npos.x, npos.y);
			stencil_option = !stencil_option;
		}
		else if (action == GLFW_RELEASE) {
			tb.end();
			stencil_option = !stencil_option;
		}
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.b_tracking) return;
	vec2 npos = vec2( float(x)/float(window_size.x-1), float(y)/float(window_size.y-1) );
	cam.view_matrix = tb.update( npos.x, npos.y );
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable( GL_STENCIL_TEST );							// turn on stencil tests
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// load the mesh
	pMesh = cg_load_mesh( mesh_vertex_path, mesh_index_path );
	if(pMesh==nullptr){ printf( "Unable to load mesh\n" ); return false; }

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
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!(stencil = cg_create_program(vert_shader_path, stenf_shader_path))) { glfwTerminate(); return 1; }

	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		if (!stencil_methods) render_0();			// per-frame render
		else render_1();
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
