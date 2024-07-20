/*#include "calculation.h"
#include "declaration.h"
#include "displaying.h"
#include "interface_GL.h"
#include "interface_nana.h"
#include <ctime>
#include <nana/gui/wvl.hpp>*/
//#define _CRT_SECURE_NO_WARNINGS

#include <ctime>

#include "declaration.h"
#include "interface_nana.h"
#include "interface_GL.h"
#include "calculation.h"
#include "displaying.h"
//#include "physics.h"
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <nana/gui/wvl.hpp>
#include <cstdlib>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
using namespace std;
using namespace nana;
const double pi = acos(-1.0);
int sch = -1;
GLFWwindow* windowgl;
void bar_chart_gl(){
	cout << "666666\n";
	clock_t be = clock();
	//read_dat(L"work.dat");
	//read_dat(p_dat);

	cerr << "I read work.dat " << clock() - be << " ms " << endl;

	at = { GLfloat( (gist_d.size_x - 1) / 2), GLfloat(0), GLfloat((gist_d.size_y - 1) / 2) };
		
	s_rot[0].x = at.x - 20;
	s_rot[0].z = at.z - 20;
	s_rot[1].x = at.x + 50;
	s_rot[1].z = at.z + 50;
	//be = clock();
	//read_brick_file(p_brick);
	//cerr << "I read brick2.txt " << clock() - be << " ms " << endl;

	be = clock();
	
	//read_array(p_barchart);
	cerr << "I read MVis_main.mtx " << clock() - be << " ms " << endl;

	be = clock();
	
	cerr << "I calculate components in " << clock() - be << " ms " << endl;
	cout << "!\n";
	
	glfwInit();

	windowgl = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tsunami analysis", NULL, NULL);
	if (!windowgl) { glfwTerminate(); return; }

	//GLFWmonitor a;
	glfwMakeContextCurrent(windowgl);
	//glfwSetKeyCallback (window, handleKeypress);
	//glfwSetWindowRefreshCallback (window, handleResize);
	glewInit();
	init_rendering();
	//init ();
	//setupScene ();

	//create_physics();
	//ground_length = 4; ground_width = 4;
	//create_ground(btVector3(0, 0, 0), ground_length, ground_width);
	//for (int i = 1; i < 10; i++) current_body = create_sphere(btVector3((double)rand() / RAND_MAX / 100, 2 * i, (double)rand() / RAND_MAX / 100), SPHERE_RADIUS, SPHERE_MASS, text_sphere[rand() % text_sphere.size()]);
	//current_body = create_sphere (btVector3 (0, 0, 0), SPHERE_RADIUS, SPHERE_MASS, text_sphere[rand () % text_sphere.size ()]);

	glfwSetFramebufferSizeCallback(windowgl, handle_resize);
	glfwSetWindowTitle(windowgl, "Tsunami analysis");
	handle_resize(windowgl, WINDOW_WIDTH, WINDOW_HEIGHT);
	//glfwSetWindowSizeCallback (handleResize); //callback function of GLFW to handle window resize
	glfwSetKeyCallback(windowgl, handle_keypress); //callback function to handle keypress
	glfwSetCursorPosCallback(windowgl, handle_cursor_pos);


	//int maxSimSubSteps = 1;
	//double dt = 0.0009765625;//1.0f / 420.f;
	//cout << "77777777\n";

	while (!glfwWindowShouldClose(windowgl)) {
		displaygl();
		glfwSwapBuffers(windowgl);

		glfwPollEvents();
		//Sleep (1);
		//system ("pause");
	}
}

void handle_keypress(GLFWwindow* window, int key, int scancodes, int action, int modes)
{
	cout << key << endl;
	//btVector3 look_to = (look_at - look_from);
	//look_to.setY(0);
	//look_to = look_to.rotate(btVector3(0, 1, 0), -pi * view_angle.getY() / 180);
	//look_to = look_to.normalize() * 20;
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_1://s
			if (s_param.NC < int(f_c_c.size()) - 1) s_param.NC++;
			cout << "NUMBER:" << s_param.NC << endl;
			cout << "Average:" << average[s_param.NC] << endl;
			cout << "Max:" << max_from_components[s_param.NC] << endl;
			break;
		case  GLFW_KEY_2://w
			if (s_param.NC > 0) s_param.NC--;
			cout << "Average:" << average[s_param.NC] << endl;
			cout << "Max:" << max_from_components[s_param.NC] << endl;
			//cout << "w" << endl;
			break;
		case 51://3
			if (s_param.zoom < 20) s_param.zoom+=0.2;
			break;
		case 52://4
			if (s_param.zoom > 1) s_param.zoom-=0.2;
			break;
		}
//		int i;
		
	}
		switch (key) {
		case 90://z
			gl_data.axis_font_size += 0.0025;
			break;
		case 88://x
			gl_data.axis_font_size -= 0.0025;

			break;
		case 67://c
			crop_a.z_far += 5;
			break;
		case 86://v
			crop_a.z_far -= 5;
			break;
		case 57://9
			m_c_s.dy1--; m_c_s.dy2--;
			break;
		case 48://0
			m_c_s.dy1++; m_c_s.dy2++;
			break;
		case 73://i
			m_c_s.dz++;
			break;
		case 74://j
			m_c_s.dx1++;
			m_c_s.dx2++;
			break;
		case 75://k
			m_c_s.dz--;
			break;
		case 76://l
			m_c_s.dx1--;
			m_c_s.dx2--;
			break;
		case 85://u
			m_c_s.alfa += 0.05;
			break;
		case 79://o
			m_c_s.alfa -= 0.05;
			break;
		case  87://w
			at.z -= 2;
			//cout << "w" << endl;
			break;
		case  83://s
			at.z += 2;
			//cout << "s" << endl;
			break;
		case  65://a
			at.x -= 2;
			//cout << "a" << endl;
			break;
		case  68://d
			at.x += 2;
			//cout << "d" << endl;
			break;
		case  GLFW_KEY_EQUAL:
			if (rot.r > 5)
				rot.r -= 1;
			cout << rot.r << " " << rot.phi << " " << rot.teta << endl;
			break;
		case GLFW_KEY_MINUS:
			//if (rot.r < 100)
			rot.r += 1;
			cout << rot.r << " " << rot.phi << " " << rot.teta << endl;
			break;
		case GLFW_KEY_ESCAPE: //Escape key
			//at.x++;
			//at.z++; at.y++; at.x++;
			//at.x++; at.y++;
			//ort.x++; ort.y++;
			exit(0); //Exit the program
			break;
		case GLFW_KEY_UP:
			//if (rot.teta< pi / 2) 
			rot.teta += GLfloat(0.01);
			cout << rot.r << " " << rot.phi << " " << rot.teta << endl;
			break;
		case GLFW_KEY_DOWN:
			//if (rot.teta > 0.01)
			rot.teta -= GLfloat(0.01);
			cout << rot.r << " " << rot.phi << " " << rot.teta << endl;
			break;
		case GLFW_KEY_LEFT:
			rot.phi += GLfloat(0.01);
			cout << rot.r << " " << rot.phi << " " << rot.teta << " " << s_rot[0].z << " " << s_rot[1].z << endl;
			break;
		case GLFW_KEY_RIGHT:
			rot.phi -= GLfloat(0.01);
			cout << rot.r << " " << rot.phi << " " << rot.teta << " "  << s_rot[0].z << " " << s_rot[1].z << endl;
			break;
		}
	
}

void handle_resize(GLFWwindow* window, int width, int height)
{	
	//cout << "Hello\n";
	//cout << width << "|||||||||||||||||||||||||||" << height << endl;
	size_window_w = width, size_window_h = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	glLoadIdentity(); //reset the camera
	//gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, crop_a.z_near, crop_a.z_far);
	gluPerspective(20.0f, (GLfloat)width / (GLfloat)height, 1.0f, 10000.0f);
}

void handle_cursor_pos(GLFWwindow * window, double x, double y)
{
	static double xp = 0, yp = 0;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		//view_angle.setY(view_angle.getY() + x - xp);
	}
	xp = x, yp = y;
}
//z - 90, x - 88//

void init_glfw()
{

}