#include "plot.h"
#include "displaying.h"
#include "calculation.h"
#include "interface_GL.h"
#include "CImg.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

double max_h(int s);

GLFT_Font font;
std::vector<GLuint> text_sphere;
GLuint text_ground, map_texture, gist_texture;
//const double pi = acos(-1.0);
point_gl from = { GLfloat(10), GLfloat(10), GLfloat(0) }, 
		at = { GLfloat(gist_d.size_x / 2), GLfloat(0), GLfloat(gist_d.size_y / 2) }, 
		ort = { GLfloat(-10), GLfloat(10), GLfloat(0) };
rotation rot{ GLfloat(130), GLfloat(0), GLfloat(pi / 6.) };//, rot_ort{0, 0.25, 0.25};
scale_move m_c_s {-100, 100, 0, 0, 20, 40};
double dScaleColorBar = 1;
strips_parameters s_param{1 , 0};
crop_area crop_a{ 1.0f, 10000.0f };
Data_gl_axis gl_data = { 0.05, L"NORTH", { 0.5, 0.5, 5 }, {0.1, 0.1}, 1. };
//int NumberC = 0;
int result = 0; 
int size_window_w = WINDOW_WIDTH, size_window_h = WINDOW_HEIGHT;
void setup_scene();
struct vertex
{
	float position[3];
	float tex_coord[2];
	float normal[3];
};
struct RGBAColor{
	unsigned char R, G, B, A;
};
struct segment{
	nana::color clr[5];
	double seg[5];
};
segment tex_col;
scale_coord front_seg[5];

void front_seg_scale(){
	front_seg[0].x = s_rot[1].x;// , -35, -s_rot[1].z
	front_seg[0].z =-s_rot[1].z;
	double r;
	for (int i = 1; i <= 3; i++){//(abs(m_c_s.dx1) + abs(m_c_s.dx2)) *
		r =(tex_col.seg[i] / tex_col.seg[4]);
		r = r / (1-r);
		front_seg[i].x = (s_rot[1].x + s_rot[0].x*r)/(1+r);
		front_seg[i].z = -1*(s_rot[1].z + s_rot[0].z*r)/(1+r);
		//cout << front_seg[i].x << "  " << front_seg[i].z << endl;
		//system("pause");

	}
	front_seg[4].x = s_rot[0].x;
	front_seg[4].z =-s_rot[0].z;

}

void set_color()
{
	tex_col.clr[0] = { 248, 255, 148, 0 };
	tex_col.clr[1] = { 250, 142, 10, 0 };
	tex_col.clr[2] = { 209, 0, 17, 0 };
	tex_col.clr[3] = { 114, 0, 145, 0 };
	tex_col.clr[4] = { 70, 0, 3, 0 };

	//tex_col.color[4] = { 255, 255, 248, 0 };
	/*tex_col.color[4] = { 255, 189, 247, 0 };
	tex_col.color[3] = { 162, 250, 184, 0 };
	tex_col.color[2] = { 249, 255, 130, 0 };
	tex_col.color[1] = { 255, 220, 46, 0 };
	tex_col.color[0] = { 232, 21, 21, 0 };*/
	
	/*tex_col.color[0] = { 255, 255, 248, 0 };
	tex_col.color[1] = { 162, 250, 184, 0 };
	tex_col.color[2] = { 249, 255, 130, 0 };
	tex_col.color[3] = { 255, 220, 46, 0 };
	tex_col.color[4] = { 232, 21, 21, 0 };*/

/*	tex_col.color[0] = { 248, 255, 148, 0 };
	tex_col.color[1] = { 250, 142, 10, 0 };
	tex_col.color[2] = { 209, 0, 7, 0 };
	tex_col.color[3] = { 105, 0, 5, 0 };
	tex_col.color[4] = { 70, 0, 3, 0 };*/

	/*number_one[2].scale_c = { 222, 255, 248 };
	number_one[3].scale_c = { 162, 250, 184 };
	number_one[4].scale_c = { 249, 255, 130 };
	//number_one[4].scale_c = { 255, 214, 64 };
	number_one[5].scale_c = { 255, 220, 46 };
	number_one[6].scale_c = { 252, 5, 9 };
	number_one[7].scale_c = { 135, 14, 0 };*/
	
	/*
	tex_col.seg[0] = 0;
	tex_col.seg[1] = 3;
	tex_col.seg[2] = 6;
	tex_col.seg[3] = 16;*/
	int* col = new int[4];
	int st = int((max_h(s_param.NC) + 1 )/ 4);
	for (int i = 1; i < 6; i++){		
		col[i - 1] = int(double(int(max_h(s_param.NC) + 1))/4 * i);
		//st = 2 * st  - i;
	}
	cout << "MAX:" << max_h(s_param.NC) << endl;
/*	tex_col.seg[0] = 0;//0;
	tex_col.seg[1] = 0.4;//col[0];//3;
	tex_col.seg[2] = 0.8;//col[1];//6;
	tex_col.seg[3] = 1.2;//col[2];//1  0;
	tex_col.seg[4] = 1.6;//int(max_h(s_param.NC)) + 1;*/

	tex_col.seg[0] = 0;//0;
	tex_col.seg[1] = 0.25;//col[0];//3;
	tex_col.seg[2] = 0.5;//col[1];//6;
	tex_col.seg[3] = 0.75;//col[2];//1  0;
	tex_col.seg[4] = 1;

	/*tex_col.seg[0] = 0; 
	//0;
	tex_col.seg[1] = col[0];//3;
	tex_col.seg[2] = col[1];//6;
	tex_col.seg[3] = col[2];//0.8;//1  0;
	max_h(s_param.NC);
	tex_col.seg[4] = col [3];*///1;//int(max_h(s_param.NC)) + 1;//15; ----------------
}
RGBAColor get_color(double from_x, double from_start, double from_end, RGBAColor in_start, RGBAColor in_end){
	//return int(((of_x - of_start) / (of_end - of_start))*(in_end - in_start) + in_start);
	RGBAColor a;
	double fraction = (from_x - from_start) / (from_end - from_start);
	a.R = int(fraction * (in_end.R - in_start.R) + in_start.R);
	a.G = int(fraction * (in_end.G - in_start.G) + in_start.G);
	a.B = int(fraction * (in_end.B - in_start.B) + in_start.B);
	a.A = 255;
	return a;
}
void init_rendering()
{

	font.open("C:\\Windows\\Fonts\\arial.ttf", 72);
	//font.open("C:\\tmp\\PTSans\\PTC1.ttf", 72);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//glEnable( GL_TEXTURE_2D );
	GLfloat ambientLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glEnable(GL_LIGHT0);
	glEnable(GL_MULTISAMPLE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glEnable (GL_LIGHT1);

	// Assign created components to GL_LIGHT0
	//glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
	//glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	//glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
	//float specLight0[4] = { 0.0f, 10.0f, 0.0f, 1.0f };
	//glLightfv (GL_LIGHT0, GL_SPECULAR, specLight0);
	//glLightfv (GL_LIGHT0, GL_POSITION, light_position);
	//GLfloat dir[] = { -1.f, -1.0f, -1.0f, 1.0f };
	//glLightfv (GL_LIGHT0, GL_SPOT_DIRECTION, dir);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//shader = CShaderManager::GetInstance()->GetShader(VERTEX_SHADER_FILE_NAME, FRAGMENT_SHADER_FILE_NAME, NULL);
	//glUseProgram(0); //shader->GetProgram ()
	setup_scene();
}
GLuint create_texture_gist(){
	GLuint Texture;  //variable for texture
	glGenTextures(1, &Texture); //allocate the memory for texture
	glBindTexture(GL_TEXTURE_2D, Texture); //Binding the texture
	//struct RGBAColor { unsigned char R, G, B, A; };
	//color
	int  height;
	height = int(tex_col.seg[4] * 100);
	RGBAColor * data = new RGBAColor[height];
	//nana::color * data = new nana::color[height];
	nana::colormap cmap({ { tex_col.seg[0], tex_col.clr[0] }, { tex_col.seg[1], tex_col.clr[1] },
	{ tex_col.seg[2], tex_col.clr[2] }, { tex_col.seg[3], tex_col.clr[3] }, { tex_col.seg[4], tex_col.clr[4] } });
	double step = tex_col.seg[4] / height;
	double x = 0;
	int i = 0;
	for (double i = 0; i < tex_col.seg[4]; i += step) {
		data[int(i / step + 0.5)] = { (unsigned char)cmap.get_color(i).r(), (unsigned char)cmap.get_color(i).g(), (unsigned char)cmap.get_color(i).b() };
		//std::cout << i << " COLOR " << int(i / step) 
			//<< " R: " << cmap.get_color(i).r() << " G: " << cmap.get_color(i).g() << " B: " << cmap.get_color(i).b() 
			//<< " R: " << int(data[int(i / step)].R) << " G: " << int(data[int(i / step)].G) << " B: " << int(data[int(i / step)].B)  << std::endl;
	}
	//data[0] = get_color(0, 0, tex_col.seg[1], tex_col.color[0], tex_col.color[1]);
/*	for (i; i < int(tex_col.seg[1]*10); i++){		
		data[i] = get_color(x, 0, tex_col.seg[1], tex_col.color[0], tex_col.color[1]);
		x += step;
	}
	for (i; i < int(tex_col.seg[2] * 10); i++){
		data[i] = get_color(x, tex_col.seg[1], tex_col.seg[2], tex_col.color[1], tex_col.color[2]);
		x += step;
	}
	for (i; i < int(tex_col.seg[3] * 10); i++){
		 data[i] = get_color(x, tex_col.seg[2], tex_col.seg[3], tex_col.color[2], tex_col.color[3]);
		x += step;
	}
	for (i; i < int(tex_col.seg[4] * 10); i++){
		data[i] = get_color(x, tex_col.seg[3], tex_col.seg[4], tex_col.color[3], tex_col.color[4]);
		x += step;
	}
	printf("Colors:\n");
	for (i = 0; i < height; i++){
		printf("%d %d %d\n", data[i].R, data[i].G, data[i].B);
	}*/
	//data[0] = {0, 0, 0};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, height, 1,0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return Texture;

}

GLuint create_texture()
{
	GLuint Texture;  //variable for texture
	glGenTextures(1, &Texture); //allocate the memory for texture
	glBindTexture(GL_TEXTURE_2D, Texture); //Binding the texture
	//struct RGBAColor { unsigned char R, G, B, A; };
	int width, height;
	//width = src.width();
	//height = src.height();//
	width = gist_d.size_x;
	height = gist_d.size_y;
	RGBAColor * data = new RGBAColor[width * height];
	
	/*int left_line, top_line, right_line, bottom_line;
	if (gist_d.size_x >= size_x) { 
		bottom_line = gist_d.l_l.y;
		top_line = gist_d.t_l.y;
		left_line = gist_d.l_l.x;
		right_line = gist_d.t_l.y;
	}*/
	int y1 = gist_d.size_y - 1;
	int x1 = 0;
	for (int y = gist_d.l_l.y; y >= gist_d.t_r.y; y--) {
		for (int x = gist_d.l_l.x; x <= gist_d.t_r.x; x++) {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			  
			if (bottom[y][x] >= 0.) {
				r = 0; b = 0;
				if (bottom[y][x] >= 0. && bottom[y][x] < 100) g = 255;
				if (bottom[y][x] >= 100 && bottom[y][x] < 150) g = 235;// = { 0, 235, 0 };
				if (bottom[y][x] >= 150 && bottom[y][x] < 200) g = 220;// { 0, 220, 0 };
				if (bottom[y][x] >= 200 && bottom[y][x] < 250) g = 200; //{ 0, 200, 0 };
				if (bottom[y][x] >= 250 && bottom[y][x] < 300) g = 180;// { 0, 180, 0 };
				if (bottom[y][x] >= 300 && bottom[y][x] < 400) g = 160;// { 0, 160, 0 };
				if (bottom[y][x] >= 400 && bottom[y][x] < 500) g = 140;// { 0, 140, 0 };
				if (bottom[y][x] >= 500 && bottom[y][x] < 750) g = 120;// { 0, 120, 0 };
				if (bottom[y][x] >= 750 && bottom[y][x] < 1000) g = 100;// { 0, 100, 0 };
				if (bottom[y][x] >= 1000 && bottom[y][x] < 1250) g = 80;// { 0, 80, 0 };
				if (bottom[y][x] >= 1250 && bottom[y][x] < 1500) g = 60;// { 0, 60, 0 };
				if (bottom[y][x] >= 1500 && bottom[y][x] < 1750) g = 40;// { 0, 40, 0 };
				if (bottom[y][x] >= 1750 && bottom[y][x] < 2200) g = 20; // { 0, 20, 0 };
				if (bottom[y][x] >= 2200) g = 0; // = { 0, 0, 0 };
			}
			else {
				r = 0;
				g = 0;
				b = 255;
				if (bottom[y][x] > -5.5 && bottom[y][x] < 0.0) {
					r = 136;
					g = 180;
					b = 255;
				}
			}
			data[x1 + y1 * (gist_d.size_x)] = { r, g, b, 0 };
			x1++;
		}
		y1--;
		x1 = 0;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return Texture;
}


struct strip
{
	GLuint vbo; // vertex
	GLuint ibo; // index
	GLuint num; // num of indexes
	GLuint texture;
};

strip * sphere;


void setup_scene()
{
	//shader = CShaderManager::GetInstance ()->GetShader (VERTEX_SHADER_FILE_NAME, FRAGMENT_SHADER_FILE_NAME, NULL);

	vertex * sphere_vertex;
	int * sphere_index;
	int num;
	set_color();
	gist_texture = create_texture_gist();
	map_texture = create_texture();
	
	// set up sphere
	/*generate_sphere(30, sphere_vertex, sphere_index, num);

	sphere = new strip;
	sphere->vbo = 0;
	sphere->ibo = 0;
	glGenBuffers(1, &sphere->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, sphere->vbo);
	glBufferData(GL_ARRAY_BUFFER, num * sizeof (vertex), &sphere_vertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &sphere->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num * sizeof (int), &sphere_index[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	sphere->num = num;

	//sphere->texture;
	glBindTexture(GL_TEXTURE_2D, 0);*/
}

void draw_rectagle(double length, double width)
{
	GLfloat gray[] = { 0.75f, 0.75f, 0.75f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gray);
	
	glBegin(GL_POLYGON);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d(0.0f, 0.0f, 0.0f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d(0.0f, 0.0f, gist_d.size_y);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d(gist_d.size_x, 0.0f, gist_d.size_y);
	
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d(gist_d.size_x, 0.0f, 0.0f);

	glEnd();
	//glDisable(GL_COLOR_MATERIAL);
}

void DrawTriangleStrip()
{
	
	
	GLfloat spec[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat dif[] = { 0.588235f, 0.670588f, 0.729412f, 1.0f };
	GLfloat emis[] = { 0.0, 0.0, 0.0, 1.0 };
	glLineWidth(10);
	glBegin(GL_TRIANGLE_STRIP);
	
	int x, z;
	GLdouble y;
	x = f_c_c[s_param.NC][0].x;
	z = f_c_c[s_param.NC][0].y;
	//glColor4d(1.0, 1.0, 0.0, 1.0);
	y = 0;
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d(x - 1, y, z - 1);
	x = f_c_c[s_param.NC][0].x;
	z = f_c_c[s_param.NC][0].y;
	//glColor4d(0.0, 0.0, 0.0, 1.0);
	y = s_param.zoom*CComp[z][x];
	//glTexCoord1d()
	glTexCoord2f(float(y / (s_param.zoom*tex_col.seg[3])), 0.0f);
	glVertex3d(x - 1, y, z - 1);
	x = f_c_c[s_param.NC][1].x;
	z = f_c_c[s_param.NC][1].y;
	//glColor4d(1.0, 1.0, 0.0, 1.0);
	y = 0;
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d(x - 1, y, z - 1);
	int kkk = 0;
	for (int i = 1, j = 1; i < int(f_c_c[s_param.NC].size()); i++)//v_c_c[0].size(); i++)
	{

		x = f_c_c[s_param.NC][i].x;
		z = f_c_c[s_param.NC][i].y;
		//glColor4d(0.0, 0.0, 0.0, 1.0);
		double size_y = 0 ;
		//if (i % 2 == 0) size_y = s_param.zoom*CComp[z][x];
		size_y = s_param.zoom*CComp[z][x];
	/*	if (j / 2 == 1){
			j-=2;
			size_y = 0;
		}*/
		j++;
		y = size_y;//s_param.zoom*CComp[z][x];
		glTexCoord2f(float(y / (s_param.zoom * tex_col.seg[4])), 0.0f);
		//if (float(y / tex_col.seg[3]) > 0.5) kkk++;
		glVertex3d(x - 1, y, z - 1);
		//glColor4d(1.0, 1.0, 0.0, 1.0);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3d(x - 1, 0, z - 1);
	}
	//printf("kkk : %d\n", kkk);

	//glVertex2d(0.5, 0.5);

	/*glVertex3d(100, 00, 200);
	glVertex3d(100, 10, 200);
	glVertex3d(200, 0, 200);*/

	glEnd();
	//glDisable(GL_BLEND);
	//glDisable(GL_COLOR_MATERIAL);
	//glVertex2d(0.75, 0);

	//glEnd();

}

void DrawLineStrip()
{
	
	glLineWidth(1.4f);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ZERO);
	
	glBegin(GL_LINES);
	glColor3d(0.8, 1., 1.);
	//int NC = NumberC;
	int x, z;
	GLdouble y;
	for (int i = 0; i < int(f_c_c[s_param.NC].size()); i++){
		if (i % 2  == 0){
		x = f_c_c[s_param.NC][i].x;
		z = f_c_c[s_param.NC][i].y;
		y = s_param.zoom*CComp[z][x];
		glVertex3d(x - 1, y, z - 1);
		glVertex3d(x - 1, 0, z - 1);
		
		}
		//glVertex3d(x, y, z);
	}
	glEnd();
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	//glDisable (GL_BLEND);
	
}

coord_gl get_coord_gl(GLdouble x, GLdouble y){
	return{ ((GLdouble(x - gist_d.start_x) / GLdouble(gist_d.end_x - gist_d.start_x))*GLdouble(gist_d.size_x)), 
		GLdouble(gist_d.size_y) - ((GLdouble(y - gist_d.start_y) / GLdouble(gist_d.end_y - gist_d.start_y))*GLdouble(gist_d.size_y)) };
}

//	glDisable(GL_COLOR_MATERIAL);
//	glDisable(GL_BLEND);

scale_coord s_rot[2];
void scale_rotation(){
	scale_coord c;
	c.x = (s_rot[0].x + s_rot[1].x) / 2;
	c.z = (s_rot[0].z + s_rot[1].z) / 2;
	GLdouble dx = s_rot[0].x, dz = s_rot[0].z;
	/*s_rot[0].x = c.x + (dx - c.x) * cos(rot.phi) - (dz - c.z)* sin(rot.phi);
	s_rot[0].z = c.z + (dx - c.x) * sin(rot.phi) + (dz - c.z)* cos(rot.phi);*/
	s_rot[0].x = cos(m_c_s.alfa)*(dx - c.x) + sin(m_c_s.alfa)*(dz - c.z) + c.x;
	s_rot[0].z = cos(m_c_s.alfa)*(dz - c.z) - sin(m_c_s.alfa)*(dx - c.x) + c.z;
	dx = s_rot[1].x, dz = s_rot[1].z;
	s_rot[1].x = cos(m_c_s.alfa)*(dx - c.x) + sin(m_c_s.alfa)*(dz - c.z) + c.x;
	s_rot[1].z = cos(m_c_s.alfa)*(dz - c.z) - sin(m_c_s.alfa)*(dx - c.x) + c.z;

	//s_rot.x = at.x - 20;
	//s_rot.y
}


void Draw_scale_color(){

	double dKoefXY = std::abs((m_c_s.dx1 - m_c_s.dx2) / (m_c_s.dy2 - m_c_s.dx1));

	s_rot[0].x = at.x + m_c_s.dx1;
	s_rot[0].z = at.z - m_c_s.dz;
	s_rot[1].x = at.x + m_c_s.dx2 * dScaleColorBar * dKoefXY;
	s_rot[1].z = at.z - m_c_s.dz;
	scale_rotation();
	glBegin(GL_POLYGON);

	glNormal3d(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3d(s_rot[0].x, m_c_s.dy1 , s_rot[0].z);

	glNormal3d(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3d(s_rot[1].x, m_c_s.dy1, s_rot[1].z);

	glNormal3d(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3d(s_rot[1].x, m_c_s.dy2 * dScaleColorBar, s_rot[1].z);

	glNormal3d(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3d(s_rot[0].x, m_c_s.dy2 * dScaleColorBar, s_rot[0].z);

	glEnd();
}
void Olyalya(){
	glBegin(GL_LINE_STRIP);
	glVertex3d(0, 30, 0);
	glVertex3d(30, 30, 30);
	glVertex3d(60, 30, 0);
	glVertex3d(30, 30, 30);
	glVertex3d(30, 30, 60);
	glVertex3d(0, 30, 30);
	glVertex3d(30, 30, 60);
	glVertex3d(0, 30, 30);
	glVertex3d(30, 30, 65);
	glVertex3d(35, 30, 65);
	glVertex3d(30, 30, 70);
	glVertex3d(20, 30, 70);
	glVertex3d(30, 30, 65);
	glVertex3d(35, 30, 60);
	glEnd();
}
void displaygl()
{
	//cout << "88888\n";
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //clear background screen to black
	//handle_resize(windowgl, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(255.0f /255., 255.0f / 255., 255.0f / 255., 0.0f); //clear background screen to black
	//glClearColor(0, 0, 0, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	from.x = rot.r * cos(rot.teta) * sin(rot.phi) + at.x;
	from.z = rot.r * cos(rot.teta) * cos(rot.phi) + at.z;
	from.y = rot.r * sin(rot.teta);
	ort.x = sin(rot.teta) * sin((rot.phi) + pi); //+ (size_x / 2);
	ort.z = sin(rot.teta) * cos((rot.phi) + pi); //+ (size_y / 2);
	ort.y = cos(rot.teta);
	//from.x = from.x * cos(rot.phi * pi / 180) - from.y * sin(rot.phi * pi / 180);
	//from.y = from.y * cos(rot.phi * pi / 180) + from.x * sin(rot.phi * pi / 180);
	gluLookAt(
		from.x, from.y, from.z,
		at.x, at.y, at.z,
		ort.x, ort.y, ort.z);
	

	{
		double ground_length = gist_d.size_y, ground_width = gist_d.size_x;
		glPushMatrix();
		
		//glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, map_texture);
		glTranslated(0, -1, 0);
		draw_rectagle(ground_length, ground_width);
		
		glDisable(GL_TEXTURE_2D);
		//glEnable(GL_LIGHTING);
		//font.drawText(200, 200, "_(o_o)_: ");
		//Draw_axis();
		DrawLineStrip();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gist_texture);
		DrawTriangleStrip();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, gist_texture);
		Draw_scale_color();
		glDisable(GL_TEXTURE_2D);
		//DrawTriangleStrip();
		

		double rx[4] = { 1, -1, 0, 0 };
		double rz[4] = { 0, 0, 1, -1 };

		glPopMatrix();
	}



	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor3d(0.0, 0.0, 0.0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//GLfloat value = 100;
		GLfloat mat_dif[] = { 1.0, 0, 0.0, 1 };
		//GLfloat mat_spec[] = { 1, 1, 1, 1 };
		//GLfloat mat_emission[] = { 1, 1, 1, 1 };
		//glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_dif);
		//glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec);
		//glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 128);
		//glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
		//glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS, &value);

		glPushMatrix();
		
		glRotatef(180, 1, 0, 0);
		front_seg_scale();
		//glRotatef(10, at.x, 32, at.z);
		//cout << "9999\n";
		double dFontHeight = (m_c_s.dy2 - m_c_s.dy1) * dScaleColorBar / 3.;

		for(int i = 0; i < 5; i++)
		{
			glTranslated(front_seg[i].x, -m_c_s.dy2 * dScaleColorBar - dFontHeight * 1.25, front_seg[i].z);
			glScaled(0.1 * dScaleColorBar, 0.1 * dScaleColorBar, 0.1 * dScaleColorBar);
			glRotatef(-m_c_s.alfa*180/pi, 0, 1, 0);

			if ((int)tex_col.seg[i] == tex_col.seg[i])
			{
				font.drawText(0, 0, std::to_string((int)tex_col.seg[i]));
			}
			else
			{
				std::ostringstream ss;
				ss << std::fixed << std::setprecision(2) << tex_col.seg[i];

				//font.drawText(0, 0, std::to_string(int(tex_col.seg[i])));
				font.drawText(0, 0, ss.str());
			}

			//font.drawText(0, 0, "8=(^_^)=8");
			glRotatef(m_c_s.alfa * 180 / pi, 0, 1, 0);
			glScaled(1.0 / 0.1 / dScaleColorBar, 1.0 / 0.1 / dScaleColorBar, 1.0 / 0.1 / dScaleColorBar);
			glTranslated(-front_seg[i].x, m_c_s.dy2 * dScaleColorBar + dFontHeight + 1, -front_seg[i].z);
			//glRotatef(-30, 0, 1, 0);
		}

		glRotatef(-180, 1, 0, 0);
		glTranslated(0, -1, 0);

		draw_axis();
		glPopMatrix();
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		}
	
	{	
		


		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glColor3d(0.0, 0.0, 0.0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPushMatrix();
		//glRotatef(180, 1, 0, 0);
		//glRotatef(90, 1, 0, 0);
		//glTranslated(size_x, -20, size_y);
		double x_s = int(gist_d.start_x) - 1;
		double y_s = int(gist_d.start_y) - 1;
		double x_e = int(gist_d.end_x) + 1;
		double y_e = int(gist_d.end_y) + 1;
		//------------------------------------------------------------------------// 
		//-------------------------------DRAW_X_AXIS------------------------------//
		//------------------------------------------------------------------------// 
		draw_font();
		//------------------------------------------------------------------------// 
		//------------------------------DRAW_FONT_Z-------------------------------//
		//------------------------------------------------------------------------// 
		/*GLdouble z = get_max_all_h();
		if (gl_data.looking_at == L"NORTH"){			
			for (int i = 5; i < int(z) + 4; i += 5){
				coord_gl a = get_coord_gl((gist_d.start_x), (gist_d.end_y));//gist_d.start_y + (gist_d.end_y - gist_d.start_y)/2)
				glTranslated(a.x - 5.3, i * s_param.zoom, a.y);
				glRotatef(180, 0, 0, 1);
				glRotatef(180, 0, 1, 0);
				glScaled(gl_data.axis_font_size, gl_data.axis_font_size, gl_data.axis_font_size);
				font.drawText(0, 0, to_string(i));
				glScaled(1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size);
				glRotatef(-180, 0, 1, 0);
				glRotatef(-180, 0, 0, 1);
				glTranslated(-a.x + 5.0, -i * s_param.zoom, -a.y);
			}
		}*/
		//------------------------------------------------------------------------// 
		//------------------------------DRAW_LABEL_Z------------------------------//
		//------------------------------------------------------------------------// 
		/*if (gl_data.looking_at == L"NORTH"){
			coord_gl tmp = get_coord_gl(gist_d.start_x, gist_d.end_y);
			glTranslated(tmp.x - gl_data.axis_font_size * 100 - 5, z / 2 * s_param.zoom, tmp.y);
			glRotatef(270, 0, 0, 1);
			glRotatef(180, 0, 1, 0);
			glScaled(gl_data.axis_font_size, gl_data.axis_font_size, gl_data.axis_font_size);
			font.drawText(0, 0, "h,m");
			glScaled(1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size);
			glRotatef(-180, 0, 1, 0);
			glRotatef(-270, 0, 0, 1);
			glTranslated(-tmp.x + gl_data.axis_font_size * 100 + 5, -z / 2 * s_param.zoom, -tmp.y);
		}*/
		//------------------------------------------------------------------------// 
		//------------------------------DRAW_Y_AXIS-------------------------------//
		//------------------------------------------------------------------------// 
		/*if (gl_data.looking_at == L"NORTH"){
			for (double i = y_s, k = 0; i < y_e; i += 0.2, k += 0.5){

				if (i >= gist_d.start_y && i <= gist_d.end_y){
					coord_gl a = get_coord_gl(GLdouble(gist_d.start_x), GLdouble(i));
					glTranslated(a.x - k * gl_data.axis_font_size * 14 - 6.7, (i - gist_d.start_y) + k / 5, a.y - gl_data.axis_font_size * 7);
					glScaled(gl_data.axis_font_size, gl_data.axis_font_size, gl_data.axis_font_size);
					glRotatef(180, 1, 0, 0);
					std::string s = to_string(i);
					for (int j = 0; j < s.size(); j++) { if (s[j] == '.') s.erase(j + 3, s.size() - j + 3); }
					font.drawText(0, 0, s);
					glRotatef(-180, 1, 0, 0);
					glScaled(1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size);
					glTranslated(-a.x + k * gl_data.axis_font_size * 14 + 6.7, -(i - gist_d.start_y) - k / 5, -a.y + gl_data.axis_font_size * 7);
				}
			}
		}*/
		//------------------------------------------------------------------------// 
		//------------------------------DRAW_LABEL_Y------------------------------//
		//------------------------------------------------------------------------// 	
		/*if (gl_data.looking_at == L"NORTH"){
			glTranslated(get_coord_gl(gist_d.start_x, gist_d.start_y).x - gl_data.axis_font_size * 100 - 9.5, 0., get_coord_gl(gist_d.start_x, gist_d.start_y + (gist_d.end_y - gist_d.start_y) / 2).y);
			glScaled(gl_data.axis_font_size, gl_data.axis_font_size, gl_data.axis_font_size);	
			glRotatef(180, 1, 0, 0);
			font.drawText(0, 0, "N");
			glRotatef(-180, 1, 0, 0);
			glScaled(1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size, 1.0 / gl_data.axis_font_size);
			glTranslated(-get_coord_gl(gist_d.start_x, gist_d.start_y).x + gl_data.axis_font_size * 100 + 9.5, 0., -get_coord_gl(gist_d.start_x, gist_d.start_y + (gist_d.end_y - gist_d.start_y) / 2).y);
		}*/
		//------------------------------------------------------------------------// 
		//--------------------DRAW_X-AXIS,_WITH_A_SMALL_Y.------------------------//
		//------------------------------------------------------------------------// 
		/*for (double i = x_s; i <= x_e; i += (gist_d.end_x - gist_d.start_x) / 5){
			coord_gl a = get_coord_gl(GLdouble(i), GLdouble(y_e));
			glTranslated(a.x + 1, -1, a.y - 1);
			glScaled(0.01, 0.01, 0.01);
			glRotatef(180, 1, 0, 0);
			glRotatef(180, 0, 1, 0);
			font.drawText(0, 0, "E" + to_string(i));
			//font.drawText(0, 0, "!=(^_^)=!");
			glRotatef(-180, 0, 1, 0);
			glRotatef(-180, 1, 0, 0);
			glScaled(1.0 / 0.01, 1.0 / 0.01, 1.0 / 0.01);
			glTranslated(-a.x - 1, +1, -a.y + 1);
		}
		
		*/
		
		/*---------------------------------------------------------*/
		/*---------------------------------------------------------*/
		/*glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glColor3f(0.0, 0.0, 0.0);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPushMatrix();
		//glRotatef(180, 1, 0, 0);
		//glRotatef(90, 1, 0, 0);
		//glTranslated(size_x, -20, size_y);
		double x_s = (gist_d.start_x);
		double y_s = (gist_d.start_y);
		double x_e = (gist_d.end_x);
		double y_e = (gist_d.end_y);
		for (double i = x_s; i <= x_e; i+= (gist_d.end_x - gist_d.start_x) / 10)
		{
			for (double j = y_s; j <= y_e; j += (gist_d.end_y - gist_d.start_y) / 10)
			{
				coord_gl a = get_coord_gl(GLdouble(i), GLdouble(j));
				glTranslated(a.x + 1, 0, a.y + 1);
				glScaled(0.01, 0.01, 0.01);
				glRotatef(90, 1, 0, 0);
				font.drawText(0, 0, "E" + to_string(i));
				//font.drawText(0, 0, "!=(^_^)=!");
				glRotatef(-90, 1, 0, 0);
				glScaled(1.0 / 0.01, 1.0 / 0.01, 1.0 / 0.01);
				glTranslated(-a.x - 1, 0, -a.y - 1);
			}
		}
		for (double i = x_s; i <= x_e; i += (gist_d.end_x - gist_d.start_x) / 10)
		{
			for (double j = y_s; j <= y_e; j += (gist_d.end_y - gist_d.start_y) / 10)
			{
				coord_gl a = get_coord_gl(GLdouble(i), GLdouble(j));
				glTranslated(a.x, 0, a.y - 0.5);
				glScaled(0.01, 0.01, 0.01);
				glRotatef(90, 1, 0, 0);
				glRotatef(-90, 0, 0, 1);
				font.drawText(0, 0, "N" + to_string(j));
				//font.drawText(0, 0, "!=(^_^)=!");
				glRotatef(90, 0, 0, 1);
				glRotatef(-90, 1, 0, 0);
				
				glScaled(1.0 / 0.01, 1.0 / 0.01, 1.0 / 0.01);
				glTranslated(-a.x, 0, -a.y + 0.5);
			}
		}*/
		/*---------------------------------------------------------*/
		/*---------------------------------------------------------*/
		/*for (int i = x_s; i <= x_e; i++)
		{
			for (int j = y_s; j <= y_e; j++)
			{
				coord_gl a = get_coord_gl(GLdouble(i), GLdouble(j));
				//glTranslated(a.x, -20, -a.y);
				glScaled(0.1, 0.1, 0.1);
				glRotatef(20, 0, 0, 0.1);
				font.drawText(a.x * 1 / 0.1-60, a.y * 1 / 0.1 - 60, to_string(j));
				glRotatef(-20, 0, 0, 0.1);
				glScaled(1.0 / 0.1, 1.0 / 0.1, 1.0 / 0.1);
				//glTranslated(-a.x, 20, a.y);
			}
		}*/
		//glTranslated
		//glScaled(0.06, 0.06, 0.06);
		//font.drawText(size_x, size_y, "Hi!");
		//font.drawText(size_x, -size_y, "Hi!");
		//font.drawText(size_x/2, size_y/2, "Hi!");
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
	}
	//cout << "10\n";
}

/*glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
glEnable(GL_COLOR_MATERIAL);
glEnable(GL_BLEND);
glBlendFunc(GL_ONE, GL_ZERO);
glBegin(GL_LINES);
glLineWidth(1.4f);
glColor3d(0, 0, 0);
glVertex3d(0, 0, 0);
glVertex3d(0, -20, 0);
glVertex3d(0, 0, 0);
glVertex3d(0, 20, 0);
glEnd();
glDisable(GL_BLEND);
glDisable(GL_COLOR_MATERIAL);*/