#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

//*******************************************************************
// global constants
static const char*	window_name = "cgbase - separable blur";
static const char*	vert_shader_path = "../bin/shaders/blur.vert";
static const char*	frag_shader_path = "../bin/shaders/blur.frag";
static const char*	image_path = "../bin/images/Lena.jpg";

//*******************************************************************
// include stb_image with the implementation preprocessor definition
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//*******************************************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2( 512, 512 );	// initial window size

//*******************************************************************
// OpenGL objects
GLuint	program	= 0;				// ID holder for GPU program
GLuint  vertex_buffer = 0;
GLuint	fbo = 0, rbo = 0;					// framebuffer objects
GLuint	SRC = 0, SEC = 0, THR = 0;
GLuint	PASS_NUMBER = 0;	// texture objects

struct camera
{
	vec3	eye = vec3(0.0f, 0.0f, -2.0f);
	vec3	at = vec3(0);
	vec3	up = vec3(0, 0.0f, 0);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect_ratio;
	float	dnear = 0.1f;
	float	dfar = 100.0f;
	mat4	projection_matrix;
};

//*******************************************************************
// global variables
int		frame=0;	// index of rendering frames
ivec2	image_size;
struct { bool left=false, right=false, up=false, down=false; operator bool() const { return left||right||up||down;} } b;
camera	cam;

//*******************************************************************
void update()
{
}

void draw_quad()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes( program );

	// render quad vertices
	glDrawArrays( GL_TRIANGLES, 0, 6 );
}

void render()
{
	float t = float(glfwGetTime());
	glUseProgram( program );
	cam.aspect_ratio = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dnear, cam.dfar);
	
	mat4 rotate_ = mat4::rotate(vec3(0,0,1), PI/4*t);
	cam.view_matrix = mat4::translate(vec3(0, 0, -3)) * mat4::rotate(vec3(1, 0, 0), -PI / 4);

	PASS_NUMBER = 1;
	glUniform1i(glGetUniformLocation(program, "pass_n"), PASS_NUMBER);

	// horizontal blur phase using render-to-texture
	glBindFramebuffer( GL_FRAMEBUFFER, fbo );												// bind frame buffer object
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SEC, 0 );	// attach texture to frame buffer object
	glUniform1i( glGetUniformLocation( program, "TEX"), 0 );
	glBindTexture( GL_TEXTURE_2D, SRC );
	glUniform2fv( glGetUniformLocation( program, "texel_offset" ), 1, vec2(1.0f/image_size.x, 1.0f / image_size.y) );
	draw_quad();

	PASS_NUMBER = 2;
	glUniform1i(glGetUniformLocation(program, "pass_n"), PASS_NUMBER);

	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, cam.view_matrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, cam.projection_matrix);

	glBindFramebuffer(GL_FRAMEBUFFER, rbo);												// bind frame buffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, THR, 0);	// attach texture to frame buffer object
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);
	glBindTexture(GL_TEXTURE_2D, SEC);
	glUniform2fv(glGetUniformLocation(program, "texel_offset"), 1, vec2(1.0f / image_size.x, 1.0f / image_size.y));
	draw_quad();

	PASS_NUMBER = 3;
	glUniform1i(glGetUniformLocation(program, "pass_n"), PASS_NUMBER);

	glUniformMatrix4fv(glGetUniformLocation(program, "rotate"), 1, GL_TRUE, rotate_);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);													// unbind frame buffer object: render to the default frame buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);						// detach texture from frame buffer object
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);
	glBindTexture(GL_TEXTURE_2D, THR);
	glUniform2fv(glGetUniformLocation(program, "texel_offset"), 1, vec2(1.0f / image_size.x, 1.0f / image_size.y));
	draw_quad();

	// read-back: uncomment this when you need read back the frame buffer
	//static uchar* buffer = (uchar*) malloc( image_size.x*image_size.y*3 );
	//glReadPixels( 0, 0, image_size.x, image_size.y, GL_RGB, GL_UNSIGNED_BYTE, buffer );
	
	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2( width, height );
	glViewport( 0, 0, width, height );
	
	// update texture size
	void update_render_target_textures( int width, int height );
	update_render_target_textures( width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press left/right to increase/decrease horizontal blur\n" );
	printf( "- press up/down to increase/decrease vertical blur\n" );
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_LEFT)		b.left	= true;
		else if(key==GLFW_KEY_RIGHT)	b.right	= true;
		else if(key==GLFW_KEY_UP)		b.up	= true;
		else if(key==GLFW_KEY_DOWN)		b.down	= true;
	}
	else if(action==GLFW_RELEASE)
	{
		if(key==GLFW_KEY_LEFT)			b.left	= false;
		else if(key==GLFW_KEY_RIGHT)	b.right	= false;
		else if(key==GLFW_KEY_UP)		b.up	= false;
		else if(key==GLFW_KEY_DOWN)		b.down	= false;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
}

void motion( GLFWwindow* window, double x, double y )
{
}

void update_render_target_textures( int width, int height )
{
	if(SEC) glDeleteTextures( 1, &SEC );
	if (THR) glDeleteTextures(1, &THR);

	
	// since we are using render-to-texture, we resize the textures by the window size
	glGenTextures( 1, &SEC);
	glBindTexture( GL_TEXTURE_2D, SEC );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8 /* GL_RGB for legacy GL */, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	
	glGenTextures(1, &THR);
	glBindTexture(GL_TEXTURE_2D,THR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8 /* GL_RGB for legacy GL */, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable( GL_TEXTURE_2D );
	glActiveTexture( GL_TEXTURE0 );

	// load and flip an image
	int comp; int &width=image_size.x, &height=image_size.y;
	unsigned char* pimage0 = stbi_load( image_path, &width, &height, &comp, 3 ); if(comp==1) comp=3; /* convert 1-channel to 3-channel image */
	int stride0 = width*comp, stride1 = (stride0+3)&(~3);	// 4-byte aligned stride
	unsigned char* pimage = (unsigned char*) malloc( sizeof(unsigned char)*stride1*height );
	for( int y=0; y < height; y++ ) memcpy( pimage+(height-1-y)*stride1, pimage0+y*stride0, stride0 ); // vertical flip
	stbi_image_free( pimage0 ); // release the original image

	// create corners and vertices
	vertex corners[4];
	corners[0].pos=vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos=vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos=vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos=vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	// generation of vertex buffer is the same, but use vertices instead of corners
	glGenBuffers( 1, &vertex_buffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// create a frame buffer object for render-to-texture
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glGenFramebuffers(1, &rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// create a src texture (lena texture)
	glGenTextures( 1, &SRC );
	glBindTexture( GL_TEXTURE_2D, SRC );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8 /* GL_RGB for legacy GL */, image_size.x, image_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, pimage );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	
	// release the new image
	free( pimage );

	// update render target textures by the window size
	update_render_target_textures( window_size.x, window_size.y );
	
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
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();
	}
	
	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
