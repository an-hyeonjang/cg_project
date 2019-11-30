#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

#include "state_render.h"

//*******************************************************************
// forward declarations for freetype text
void text_init();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color);
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, float t, GLFWwindow* window);


//*******************************************************************
// global constants
static const char* window_name = "CG invader";
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


//******************************************************************
// game_menu
background_t	menu_background_image;

//*******************************************************************
// player attribute
background_t	background;
uint	stage;
player_t	player;
player_t	player1;
std::vector<creature_t>	creatures;

enum game_state
{
	GAME_MENU,
	GAME_STAGE0,
	GAME_STAGE01,
	GAME_STAGE1,
	GAME_OVER,
	GAME_WIN
};

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
	player1.init();

	//stage2
	creatures = create_creatures(2, 0.5f);

	return true;
}

void update()
{
	float t = (float)glfwGetTime();
	game_update(window_size);

	for (auto& c : creatures)
		change_state(player, c);
}

void state0_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = (float)glfwGetTime();

	background.render();
	text_box_render();
	
	player.render(t);
	for (auto& c : creatures)
		c.render_circle();

	render_text("I am undergraduated student!!", 180, 60, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("I really want to graduate in this semester with good grade", 180, 90, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("I heard CG class is the best for it", 180, 120, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));

	glfwSwapBuffers(window);
}

void state01_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = (float)glfwGetTime();

	player.render(t);

	glfwSwapBuffers(window);
}

void stage1_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	float t = (float)glfwGetTime();
	
	background.render();
	//for (auto& c : creatures) c.render();
	player.render(t);

	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
	else if (key == GLFW_KEY_H || key == GLFW_KEY_F1) {}
	
	player.control(key, action);
	player.control_axis(key, action);

	if (key == GLFW_KEY_ENTER)
	{
		stage %= 4;
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
	
	//menu_render(window_name, window);
	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
	{
		switch (stage)
		{
		case GAME_MENU:
			glfwPollEvents();
			menu_render(window_name, window);
			//glfwWaitEvents();
			break;
		case GAME_STAGE0:
			glfwPollEvents();
			update();
			state0_render();
			break;
		case GAME_STAGE01:
			glfwPollEvents();
			update();
			state01_render();
			break;
		case GAME_STAGE1:
			glfwPollEvents();
			update();
			stage1_render();
			break;
		}
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
