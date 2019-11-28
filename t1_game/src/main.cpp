#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

#include "game_object.h"

//*******************************************************************
// global constants
static const char* window_name = "cg-invader";
static const char* vert_shader_path = "../bin/shaders/cg.vert";
static const char* frag_shader_path = "../bin/shaders/cg.frag";

static const char* image_path = "../bin/images/mech-unit-export1.png";

//*******************************************************************
// window objects
GLFWwindow* window = nullptr;
ivec2		window_size = ivec2(1024, 512);	// initial window size

//*******************************************************************
// OpenGL objects
GLuint	program = 0;	// ID holder for GPU program
GLuint	vertex_buffer = 0;	// ID holder for vertex buffer
GLuint	SRC = 0;


//*******************************************************************
// global variables
int		frame = 0;						// index of rendering frames
ivec2	image_size;
float	move = 0.5f;

//*******************************************************************
// player attribute
player quad(0.5f);
Texture tex;

bool user_init()
{

	// init GL states
	glLineWidth(1.0f);
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	tex.window_size = window_size;
	tex.load(image_path);

	static vertex vertices[] = { {vec3(-1,-1,0),vec3(0,0,1),vec2(0,0)}, {vec3(1,-1,0),vec3(0,0,1),vec2(1,0)}, {vec3(-1,1,0),vec3(0,0,1),vec2(0,1)}, {vec3(1,1,0),vec3(0,0,1),vec2(1,1)} }; // strip ordering [0, 1, 3, 2]

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &vertices[0], GL_STATIC_DRAW);


	return true;
}


void update()
{
	
	float aspect = window_size.x / float(window_size.y);
	mat4 aspect_matrix =
	{
		min(1 / aspect,1.0f), 0, 0, 0,
		0, min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};   
	glUniformMatrix4fv(glad_glGetUniformLocation(program, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, quad.model_matrix);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	cg_bind_vertex_attributes(program);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{	
	printf("work\n");
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();

		if (key == GLFW_KEY_RIGHT)
		{
			quad.update(vec3(move,0,0));
		}
		if (key == GLFW_KEY_UP)
		{
			quad.update(vec3(0,move,0));
		}
		if (key == GLFW_KEY_DOWN)
		{
			quad.update(vec3(0,-move,0));
		}
		if (key == GLFW_KEY_LEFT)
		{
			quad.update(vec3(-move,0,0));
		}

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


void user_finalize()
{
}

int main(int argc, char* argv[])
{
	// initialization
	if (!glfwInit()) { printf("[error] failed in glfwInit()\n"); return 1; }

	// create window and initialize OpenGL extensions
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback(window, reshape);	// callback for window resizing events
	glfwSetKeyCallback(window, keyboard);			// callback for keyboard events
	glfwSetMouseButtonCallback(window, mouse);	// callback for mouse click inputs
	glfwSetCursorPosCallback(window, motion);		// callback for mouse movements

	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
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
