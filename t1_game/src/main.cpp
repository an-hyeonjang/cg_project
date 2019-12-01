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
static const char* src1 = "../bin/sounds/2.mp3";

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
float	score;
float	timer;

//******************************************************************
// game_menu
background_t	menu_background_image;

enum game_stage
{
	GAME_MENU,
	GAME_STAGE0,
	GAME_STAGE01,
	GAME_STAGE1,
	GAME_OVER,
	GAME_WIN
};

//*******************************************************************
// player attribute
background_t	background;
game_stage	stage;
player_t	player;
player_t	win_player;
player_t	fail_player;
std::vector<creature_t>	creatures;

bool user_init()
{
	glLineWidth(1.0f);
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//stage level
	stage = GAME_MENU;
	text_init();

	//game_menu
	menu_background_image.init();

	//stage0
	game_object_init(window_size);
	background.init();

	//player
	player.init();	//for real play
	win_player.init();	//for win screen
	fail_player.init();	//for game over screen

	return true;
}

void update()
{
	game_update(window_size);
}

void menu_render(const char* window_name)
{
	menu_background.init();

	//engine->play2D(back_src, true);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float a = (float)sin(glfwGetTime());
	menu_background.render();
	render_text(window_name, 600, 100, 2.0f, vec4(a, 0.6f, 0.8f, 1.0f));
	render_text("press Enter", 800, 150, 0.8f, vec4(1.0f, 1.0f, 0.0f, a));

	render_text("control key: up, down, left, right", 650, 400, 0.6f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
	render_text("attack key: z", 650, 430, 0.6f, vec4(1.0f, 1.0f, 1.0f, 1.0f));

	glfwSwapBuffers(window);
}

void stage0_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = (float)glfwGetTime();

	background.render();
	text_box_render("../bin/images/text_box1.png");
	
	render_text("I am CG invader and until UNDERGRADUATED student!!", 180, 60, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("I really want to graduate in this semester with good grade", 180, 90, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("I heard CG class is the best for it", 180, 120, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("continue.. press c", 200, 160, 0.5f, vec4(1.0f, 1.0f, 0.0f, sin(t)*5));

	player.render(t);

	glfwSwapBuffers(window);
}

void stage01_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = (float)glfwGetTime();

	background.render();
	text_box_render("../bin/images/text_box2.png");

	render_text("Hello student!!", 180, 60, 0.45f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("I am Doctor CG. ", 180, 90, 0.45f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("This class is very hard. ", 180, 120, 0.45f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("And you need some Prerequisites. ", 180, 150, 0.45f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("Are you really ready for this? ", 180, 180, 0.45f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("Yes or No, press[y/n]", 200, 220, 0.5f, vec4(1.0f, 1.0f, 0.0f, sin(t)*5));

	player.render(t);

	glfwSwapBuffers(window);
}

void stage1_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = timer - (float)glfwGetTime();

	std::string time = std::to_string(t);
	std::string remain = std::to_string(creatures.size());

	render_text("limit: "+ time, 100, 50, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));
	render_text("remain: " + remain, 100, 80, 0.5f, vec4(1.0f, 1.0f, 1.0f, 0.8f));

	background.render();

	for (auto& c : creatures)
	{
		c.update(t);
		wall_collision(c, wall);
		object_collision(c, creatures);
		c.render_circle();
	}

	player.render(t);

	if (t < 0) stage = GAME_OVER;
	if (change_state(player, creatures)) { stage = GAME_WIN; score = t; }

	glfwSwapBuffers(window);
}

void game_win_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = (float)glfwGetTime();
	float s = (float)sin(glfwGetTime()) * 5.0f;
	float c = (float)cos(glfwGetTime()) * 5.0f;

	render_text("Victory!", 320, 150, 2.5f, vec4(c, s, c, 1.0f));
	render_text("score: " + std::to_string(score), 350, 250, 1.0f, vec4(s * s, s, c * s, 1.0f));
	render_text("Yes or No, press[y/n]", 360, 280, 0.5f, vec4(s * s, s * s, s * s, s));

	win_player.render(t);
	win_player.position.x += sin(t)/200.0f;
	win_player.position.y += sin(t) / 200.0f;

	glfwSwapBuffers(window);
}
void game_over_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float t = (float)glfwGetTime();
	float s = (float)sin(t) * 5.0f;
	float c = (float)cos(t) * 5.0f;

	fail_player.normal.load("../bin/images/sprites/fail1.png");
	fail_player.normal.load("../bin/images/sprites/fail2.png");

	fail_player.render(t/50.0f);

	render_text("GAME OVER", 200, 150, 2.5f, vec4(c, s, c, 1.0f));
	render_text("continue?", 400, 250, 1.0f, vec4(s*s, s, c*s, 1.0f));
	render_text("Yes or No, press[y/n]", 400, 300, 0.5f, vec4(s * s, s * s, s * s, s));

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
	
	if (action == GLFW_PRESS)
	{
		if (stage == GAME_MENU)
			if (key == GLFW_KEY_ENTER) stage = GAME_STAGE0;
		if (stage == GAME_STAGE0)
			if (key == GLFW_KEY_C) stage = GAME_STAGE01;
		if (stage == GAME_STAGE01)
		{
			if (key == GLFW_KEY_Y) stage = GAME_STAGE1;
			else if (key == GLFW_KEY_N) stage = GAME_OVER;
		}
		if (stage == GAME_OVER)
			if (key == GLFW_KEY_Y) stage = GAME_STAGE1;
			else if (key == GLFW_KEY_N) glfwTerminate();
		if (stage == GAME_WIN)
			if (key == GLFW_KEY_Y) stage = GAME_STAGE1;
			else if (key == GLFW_KEY_N) glfwTerminate();
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
	engine->drop();
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
	
	top:
		switch (stage)
		{
		case GAME_MENU:
			for (frame = 0; !glfwWindowShouldClose(window); frame++)
			{
				glfwPollEvents();
				menu_render(window_name);
				if (stage != GAME_MENU) goto top;
			}
			break;
		case GAME_STAGE0:
			for (frame = 0; !glfwWindowShouldClose(window); frame++)
			{
				glfwPollEvents();
				update();
				stage0_render();
				if (stage != GAME_STAGE0) goto top;
			}
			break;
		case GAME_STAGE01:
			for (frame = 0; !glfwWindowShouldClose(window); frame++)
			{
				glfwPollEvents();
				update();
				stage01_render();
				if (stage != GAME_STAGE01) goto top;
			}
			break;
		case GAME_STAGE1:
			creatures = create_creatures(20, 0.2f);
			timer = (float)glfwGetTime() + 45.0f;
			for (frame = 0; !glfwWindowShouldClose(window); frame++)
			{
				glfwPollEvents();
				update();
				stage1_render();
				if (stage != GAME_STAGE1) goto top;
			}
			break;;
		case GAME_OVER:
			for (frame = 0; !glfwWindowShouldClose(window); frame++)
			{
				glfwPollEvents();
				update();
				game_over_render();
				if (stage != GAME_OVER) goto top;
			}
			break;
		case GAME_WIN:
			for (frame = 0; !glfwWindowShouldClose(window); frame++)
			{
				glfwPollEvents();
				update();
				game_win_render();
				if (stage != GAME_WIN) goto top;
			}
			break;
		}


	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
