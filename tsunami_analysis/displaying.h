#ifndef DISPLAYING_H
#define DISPLAYING_H
#define WALK_SPEED 0.005
#define TURN_SPEED 0.01
#include "declaration.h"
struct crop_area{
	GLdouble z_near, z_far;
};

struct coord_gl
{
	GLdouble x, y;
};
struct rotation
{
	GLfloat r, phi, teta;
};
struct point_gl
{
	GLdouble x, y, z;
};
struct scale_coord{
	GLdouble x, z, y;
};
struct scale_move{
	GLdouble dx1, dx2, dz, alfa, dy1, dy2;
};
struct strips_parameters{ 
	double zoom; int NC;
};

struct Data_gl_axis{
	GLdouble axis_font_size;
	std::wstring looking_at;
	point_gl step_font;
	coord_gl step_axis;
	double ratio;
};

extern Data_gl_axis gl_data;
extern scale_coord s_rot[2];
extern strips_parameters s_param;
extern double dScaleColorBar;
extern scale_move m_c_s;
extern point_gl from, at, ort;
extern rotation rot;//, rot_ort;
extern crop_area crop_a;
extern GLFT_Font font;
extern int size_window_w, size_window_h;
//void WindowDump();
void draw_font();
void displaygl();
void init_rendering();
void set_color();
void draw_axis();
GLdouble get_max_all_h();
coord_gl get_coord_gl(GLdouble x, GLdouble y);
 
#endif