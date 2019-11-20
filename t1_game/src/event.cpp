#include	"cgmath.h"
#include	"cgut.h"

#include	"trackball.h"

camera cam;
trackball tb;
ivec2		window_size = ivec2(1024, 576);

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


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{

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