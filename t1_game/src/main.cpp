#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

#include "game_object.h"
#include "collision.h"

//*******************************************************************
// forward declarations for freetype text
void text_init();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color);

enum game_state
{
	GAME_MENU,
	GAME_STAGE0,
	GAME_STAGE1,
	GAME_WIN,
	GAME_FAIL
};

//*******************************************************************
// global constants
static const char* window_name = "cg-invader";
static const char* vert_shader_path = "../bin/shaders/circ.vert";
static const char* frag_shader_path = "../bin/shaders/circ.frag";

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
float	move = 0.1f;


//******************************************************************
// game_menu
background_t	menu_background_image;

//*******************************************************************
// player attribute
background_t	background;
uint	stage;
player_t	player;
std::vector<creature_t>	creatures;

//*******************************************************************
// stage 1

bool user_init()
{
	glLineWidth(1.0f);
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	stage = 0;

	text_init();

	//game_menu
	menu_background_image.init();

	//stage0
	game_object_init();

	background.init();
	player.init();

	//stage2

	creature_t creat;
	creatures.push_back(creat);
	for(auto& c: creatures) c.init();

	return true;
}

void update()
{
	game_update(window_size);

	for (auto& c : creatures)
		change_state(player, c);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = (float)sin(glfwGetTime());
	
	background.render();
	player.render(t);
	for (auto& c : creatures) c.render();

	glfwSwapBuffers(window);
}

void menu_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	menu_background_image.render();

	render_text(window_name, 100, 100, 1.0f, vec4(0.5f, 0.8f, 0.2f, 1.0f));
	render_text("I love Computer Graphics!", 100, 125, 0.5f, vec4(0.7f, 0.4f, 0.1f, 0.8f));
	render_text("Blinking text here", 100, 155, 0.6f, vec4(0.5f, 0.7f, 0.7f, 1.0f));

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
	player.control(key, action);
	if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();

	if (key == GLFW_KEY_RIGHT)
	{
		player.moving(vec3(move,0,0));
	}
	if (key == GLFW_KEY_UP)
	{
		player.moving(vec3(0,move,0));
	}
	if (key == GLFW_KEY_DOWN)
	{
		player.moving(vec3(0,-move,0));
	}
	if (key == GLFW_KEY_LEFT)
	{
		player.moving(vec3(-move,0,0));
	}
	if (key == GLFW_KEY_Z)
	{
		if(action == GLFW_PRESS) player.hit_on = 1;
		else if (action == GLFW_RELEASE) player.hit_on = 0;
	}
	if (key == GLFW_KEY_SPACE)
	{
		stage %= game_state();
		if (action == GLFW_PRESS) stage += 1;
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
		switch (stage)
		{
		case GAME_MENU:
			glfwPollEvents();
			menu_render();
			break;
		case GAME_STAGE0:
			glfwPollEvents();	// polling and processing of events
			update();
			render();
			break;
		case GAME_STAGE1:
			glfwPollEvents();	// polling and processing of events
			update();
			render();
			break;
		case GAME_WIN:
			break;
		case GAME_FAIL:
			break;
		}			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
