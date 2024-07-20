#include "displaying.h"
#include "calculation.h"
#include "interface_GL.h"

#include <iomanip>

std::string ConvertToString(double value)
{
	std::ostringstream wstream;
	wstream << std::fixed << std::setprecision(2) << value;
	std::string result = wstream.str();
	result.erase(result.find_last_not_of('0') + 1, std::string::npos);
	result.erase(result.find_last_not_of('.') + 1, std::string::npos);
	return result;
}

GLdouble get_max_all_h(){
	GLdouble max = 0;
	for (int i = 0; i < int(f_c_c.size()); i++){
		if (max_h(i) > max) max = GLdouble(max_h(i));
	}
	return max;
}
std::string erase_str(std::string str,int count){
	for (int j = 0; j < int(str.size()); j++) { if (str[j] == '.') str.erase(j + count + 1, str.size() - j + count + 1); }
	return str;
}
void draw_grid(coord_gl pos_z, coord_gl pos_z1, coord_gl pos_z2, coord_gl pos_z3){

	GLdouble x[4]; x[0] = pos_z.x; x[1] = pos_z1.x; x[2] = pos_z2.x; x[3] = pos_z3.x;
	GLdouble y[4]; y[0] = pos_z.y; y[1] = pos_z1.y; y[2] = pos_z2.y; y[3] = pos_z3.y;
	GLdouble z = get_max_all_h();
	coord_gl a_z[4]; //a_z[0] = get_coord_gl(x[0], y[0]);
	for (int i = 0; i < 4; i++)	a_z[i] = get_coord_gl(x[i], y[i]);
	glLineWidth(1.f);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glBegin(GL_LINES);

	glColor3d(0, 0, 0);

	for (int i = 0; i < 3; i++){
		glVertex3d(a_z[i].x, 0, a_z[i].y);
		glVertex3d(a_z[i].x, z * s_param.zoom + z / 10 * s_param.zoom, a_z[i].y);
	}
	double delta_x = 0, delta_y = 0;
	if (gl_data.looking_at == L"NORTH")	{ delta_x = -0.3; }
	if (gl_data.looking_at == L"SOUTH")	{ delta_x = 0.3; }
	if (gl_data.looking_at == L"EAST")	{ delta_y = -0.3; }
	if (gl_data.looking_at == L"WEST")	{ delta_y = 0.3; }

/*	for (int i = 1; i < int(z) + 5; i++){

		if (i % int(gl_data.step_font.z) == 0){ delta_x *= 1.5; delta_y *= 1.5; }
		glVertex3d(a_z[0].x + delta_x, i * s_param.zoom, a_z[0].y + delta_y);
		glVertex3d(a_z[0].x, i * s_param.zoom, a_z[0].y);
		if (i % int(gl_data.step_font.z) == 0){ delta_x /= 1.5; delta_y /= 1.5; }

		if (i % int(gl_data.step_font.z) == 0){
			glVertex3d(a_z[0].x, i * s_param.zoom, a_z[0].y);
			glVertex3d(a_z[1].x, i * s_param.zoom, a_z[1].y);

			glVertex3d(a_z[1].x, i * s_param.zoom, a_z[1].y);
			glVertex3d(a_z[2].x, i * s_param.zoom, a_z[2].y);
		}
	}*/
	for (double i = gl_data.step_font.z; i < z + z / 10; i += gl_data.step_font.z){
		bool initial = true;//i / gl_data.step_font.z >= 0.9999999 && i / gl_data.step_font.z <= 1.0000001;
		if (initial){ delta_x *= 1.5; delta_y *= 1.5; }
		glVertex3d(a_z[0].x + delta_x, i * s_param.zoom, a_z[0].y + delta_y);
		glVertex3d(a_z[0].x, i * s_param.zoom, a_z[0].y);
		if (initial){ delta_x /= 1.5; delta_y /= 1.5; }

		if (initial){
			glVertex3d(a_z[0].x, i * s_param.zoom, a_z[0].y);
			glVertex3d(a_z[1].x, i * s_param.zoom, a_z[1].y);

			glVertex3d(a_z[1].x, i * s_param.zoom, a_z[1].y);
			glVertex3d(a_z[2].x, i * s_param.zoom, a_z[2].y);
		}
	}
	for (double i = int(gist_d.start_x) - 1; i < int(gist_d.end_x) + 1; i += gl_data.step_axis.x){
		if (i >= gist_d.start_x && i <= gist_d.end_x) {
			coord_gl a, b;
			a = get_coord_gl(GLdouble(i), GLdouble(gist_d.start_y));
			b = get_coord_gl(GLdouble(i), GLdouble(gist_d.end_y));
			glVertex3d(a.x, 0.1, a.y);
			glVertex3d(b.x, 0.1, b.y);
		}
	}

	for (double i = int(gist_d.start_y) - 1; i < int(gist_d.end_y) + 1; i += gl_data.step_axis.y){
		if (i >= gist_d.start_y && i <= gist_d.end_y) {
			coord_gl a, b;
			a = get_coord_gl(GLdouble(gist_d.start_x), GLdouble(i));
			b = get_coord_gl(GLdouble(gist_d.end_x), GLdouble(i));
			glVertex3d(a.x, 0.1, a.y);
			glVertex3d(b.x, 0.1, b.y);
		}
	}
	glEnd();
	for (double i = int(gist_d.start_x) - 1; i < int(gist_d.end_x) + 1; i += gl_data.step_font.x){
		if (i >= gist_d.start_x && i <= gist_d.end_x){
			coord_gl a;// , b;
			double delta = 0;
			a = get_coord_gl(GLdouble(i), GLdouble(pos_z3.y));
			//b = get_coord_gl(GLdouble(i), GLdouble(gist_d.end_y));
			glBegin(GL_POLYGON);

			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y - 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y - 0.6f);
			//////////////////////////////////////////////////////
			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y + 0.6f);

			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y - 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y - 0.6f);

			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x - 0.6f + delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x - 0.6f + delta / 4, 0.01f, a.y - 0.6f);

			glVertex3d(a.x + 0.6f - delta / 4, 0.01f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f - delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y + 0.6f);
			glEnd();
		}
	}
	for (double i = int(gist_d.start_y) - 1; i < int(gist_d.end_y) + 1; i += gl_data.step_font.y){
		if (i >= gist_d.start_y && i <= gist_d.end_y) {
			double delta = 0;
			coord_gl a;
			a = get_coord_gl(GLdouble(pos_z3.x), GLdouble(i));

			glBegin(GL_POLYGON);

			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y - 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y - 0.6f);
			//////////////////////////////////////////////////////
			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y + 0.6f);

			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y - 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y - 0.6f);

			glVertex3d(a.x - 0.6f - delta / 4, 0.01f, a.y + 0.6f);
			glVertex3d(a.x - 0.6f - delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x - 0.6f + delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x - 0.6f + delta / 4, 0.01f, a.y - 0.6f);

			glVertex3d(a.x + 0.6f - delta / 4, 0.01f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f - delta / 4, -1.0f, a.y - 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, -1.0f, a.y + 0.6f);
			glVertex3d(a.x + 0.6f + delta / 4, 0.01f, a.y + 0.6f);

			glEnd();
		}
	}
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_BLEND);
}
void draw_string( point_gl c_text, GLdouble scaled, GLdouble angle[3], std::string str) {
	glTranslated(c_text.x, c_text.y, c_text.z);
	glScaled(scaled, scaled, scaled);

	glRotated(angle[0], 1, 0, 0);
	glRotated(angle[1], 0, 1, 0);
	glRotated(angle[2], 0, 0, 1);

	font.drawText(0, 0, str);

	glRotated(-angle[2], 0, 0, 1);
	glRotated(-angle[1], 0, 1, 0);
	glRotated(-angle[0], 1, 0, 0);

	glScaled(1./scaled, 1./scaled, 1./scaled);
	glTranslated(-c_text.x, -c_text.y, -c_text.z);
}

void draw_font_axis(coord_gl z, double x, double y, double step[2], double angle[3], double d_x[2], double d_y[2], double d_z[2]){
	double x_s = int(gist_d.start_x) - 1;
	double y_s = int(gist_d.start_y) - 1;
	double x_e = int(gist_d.end_x) + 1;
	double y_e = int(gist_d.end_y) + 1;
	GLdouble max_h = get_max_all_h();
		for (double i = x_s; i <= x_e; i += step[0]){
			if (i >= gist_d.start_x && i <= gist_d.end_x){
				double dy = d_y[0], dz = d_z[0];
				if (gl_data.looking_at == L"EAST"){ dz += (i - x_s) * gl_data.ratio;	dy -= (i - x_s) * gl_data.ratio; }
				if (gl_data.looking_at == L"WEST"){ dz += (x_e - i) * gl_data.ratio;	dy += (x_e - i) * gl_data.ratio; }
				coord_gl a = get_coord_gl(GLdouble(i), GLdouble(y));
				std::string s = ConvertToString(i);
				draw_string({ a.x + d_x[0], dz, a.y + dy}, gl_data.axis_font_size, angle, s + "°");
			}
		}
		for (double i = y_s; i <= y_e; i += step[1]){
			if (i >= gist_d.start_y && i <= gist_d.end_y){
				double dx = d_x[1], dz = d_z[1];
				if (gl_data.looking_at == L"NORTH"){ dz += (i - y_s) * gl_data.ratio;	dx -= (i - y_s) * gl_data.ratio; }
				if (gl_data.looking_at == L"SOUTH"){ dz += (y_e - i) * gl_data.ratio;	dx += (y_e - i) * gl_data.ratio; }
				coord_gl a = get_coord_gl(GLdouble(x), GLdouble(i));
				std::string s = ConvertToString(i);
				draw_string({ a.x + dx, dz, a.y + d_y[1] }, gl_data.axis_font_size, angle, s + "°");
			}
		}
		double dx, dy;
		if(abs(d_x[0]) < abs(d_x[1])) dx = d_x[0];
		else dx = d_x[1];
		if(abs(d_y[0]) < abs(d_y[1])) dy = d_y[0];
		else dy = d_y[1];
		if (abs(dx) > abs(dy)) dy = 0;
		else dx = 0;
		for (GLdouble i = gl_data.step_font.z; i < max_h + max_h / 10; i += gl_data.step_font.z){
			double dx2 = 0, dy2 = 0;
			std::string text = ConvertToString(i);
			double delta = text.size();
			if (gl_data.looking_at == L"SOUTH") { dx2 = delta * 1.8 + delta * 0.5; }
			if (gl_data.looking_at == L"NORTH") { dx2 = -delta * 1.8 - delta * 0.5; }
			if (gl_data.looking_at == L"EAST") { dy2 = delta * 1.8 + delta * 0.5; }
			if (gl_data.looking_at == L"WEST") { dy2 = -delta * 1.8 - delta * 0.5; }
			draw_string({ z.x + dx + dx2, i * s_param.zoom + 20 * gl_data.axis_font_size, z.y + dy + dy2 }, gl_data.axis_font_size, angle, text);
			}
}
//void draw_font_y(){}
//void draw_font_z(){}



void draw_font(){
	coord_gl z;
	
	double  x, y, dz[2] = {0, 0}, dx[2] = { 0, 0 }, dy[2] = { 0, 0 }, step[2] = { 1, 1 }; double angle[3];
	double delta = gl_data.axis_font_size * 90, delta2 = gl_data.axis_font_size * 250;
	if (gl_data.looking_at == L"NORTH"){ 
		z = get_coord_gl(gist_d.start_x, gist_d.end_y);
		x = gist_d.start_x; y = gist_d.start_y; 
		
		dx[0] = -delta; dx[1] = -delta2;
		dy[0] = 1;
		dz[0] = -2; dz[1] = 0;
		/**********************LABEL_Z*****************************/
		angle[0] = 180; angle[1] = 0; angle[2] = 270;
		double h = get_max_all_h();
		draw_string({ z.x +dx[1], h / 2 * s_param.zoom, z.y }, gl_data.axis_font_size, angle, "h,m");
		/*********************************************************/
		angle[0] = 180; angle[1] = 0; angle[2] = 0; 
		step[0] = gl_data.step_font.x; step[1] = gl_data.step_font.y;//gist_d.start_y + (gist_d.end_y - gist_d.start_y) / 2)
		draw_string({ get_coord_gl(x, y).x + dx[1] / 1.5, dz[0], get_coord_gl(x, y).y + 2 * dy[0] }, gl_data.axis_font_size, angle, "E"); //"E");
		draw_string({ get_coord_gl(x, y).x + dx[1]*1.5, 0.5, get_coord_gl(x, y + (gist_d.end_y - y) / 2).y }, gl_data.axis_font_size, angle, "N");
	}
	if (gl_data.looking_at == L"SOUTH"){
		z = get_coord_gl(gist_d.end_x, gist_d.start_y);
		x = gist_d.end_x; y = gist_d.end_y; 
		
		dx[0] = delta; dx[1] = delta2;
		dy[0] = -1;
		dz[0] = -2; dz[1] = 0.;
		step[0] = gl_data.step_font.x; step[1] = gl_data.step_font.y;
		/**********************LABEL_Z*****************************/
		angle[0] = 180; angle[1] = 180; angle[2] = 270;
		double h = get_max_all_h();
		draw_string({ z.x + dx[1], h / 2 * s_param.zoom, z.y }, gl_data.axis_font_size, angle, "h,m");
		//--------------------------------------------------------//
		angle[0] = 180; angle[1] = 180; angle[2] = 0;
		
		draw_string({ get_coord_gl(x, y).x + dx[1] / 1.5, dz[0], get_coord_gl(x, y).y + 2 * dy[0] }, gl_data.axis_font_size, angle, "E"); //"E");
		draw_string({ get_coord_gl(x, y).x + dx[1]*1.5, 0.5, get_coord_gl(x, y - (y - gist_d.start_y) / 2).y }, gl_data.axis_font_size, angle, "N");
	}
	if (gl_data.looking_at == L"WEST"){ 
		z = get_coord_gl(gist_d.start_x, gist_d.start_y);
		x = gist_d.end_x;  y = gist_d.start_y; 
		
		dx[1] = 1; 
		dy[0] = delta2; dy[1] = delta;		
		dz[0] = 0.; dz[1] = -2;
		step[0] = gl_data.step_font.x; step[1] = gl_data.step_font.y;
		
		/**********************LABEL_Z*****************************/
		angle[0] = 180; angle[1] = 270; angle[2] = 270;
		double h = get_max_all_h();
		draw_string({ z.x, h / 2 * s_param.zoom, z.y + dy[1] * 1.5 + 5 }, gl_data.axis_font_size, angle, "h,m");
		/*********************************************************/
		angle[0] = 180; angle[1] = 270; angle[2] = 0;

		draw_string({ get_coord_gl(x - (x - gist_d.start_x) / 2, y).x + dx[1], dz[0], get_coord_gl(x, y).y + dy[0] * 1.5 }, gl_data.axis_font_size, angle, "E"); //"E");
		draw_string({ get_coord_gl(x, y).x + 2*dx[1], dz[1], get_coord_gl(x, y).y + dy[0]/1.5 }, gl_data.axis_font_size, angle, "N");
	}
	if (gl_data.looking_at == L"EAST"){ 
		z = get_coord_gl(gist_d.end_x, gist_d.end_y);
		x = gist_d.start_x; y = gist_d.end_y;	
		
		dx[1] = -1; 
		dy[0] = -delta2; dy[1] = -delta;
		dz[0] = 0.; dz[1] = -2;
		step[0] = gl_data.step_font.x; step[1] = gl_data.step_font.y;

		/**********************LABEL_Z*****************************/
		angle[0] = 180; angle[1] = 90; angle[2] = 270;
		double h = get_max_all_h();
		draw_string({ z.x, h / 2 * s_param.zoom, z.y + dy[1] * 1.5 - 5 }, gl_data.axis_font_size, angle, "h,m");
		/*********************************************************/
		angle[0] = 180; angle[1] = 90; angle[2] = 0;
		draw_string({ get_coord_gl(x + (gist_d.end_x - x) / 2, y).x + dx[1], dz[0], get_coord_gl(x, y).y + dy[0] * 1.5 }, gl_data.axis_font_size, angle, "E"); //"E");
		draw_string({ get_coord_gl(x, y).x + 2 * dx[1], dz[1], get_coord_gl(x, y).y + dy[0] / 1.5 }, gl_data.axis_font_size, angle, "N");
	}
	draw_font_axis(z, x, y, step, angle, dx, dy, dz);

}

void draw_axis(){

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glLineWidth(1.4f);

	//--------------------------------------------------------------------//
	//---------------------------DRAW_AXIS_Z------------------------------//
	//--------------------------------------------------------------------//
	if (gl_data.looking_at == L"NORTH"){
		draw_grid({ gist_d.start_x, gist_d.end_y }, { gist_d.end_x, gist_d.end_y },
		{ gist_d.end_x, gist_d.start_y }, { gist_d.start_x, gist_d.start_y });
	}
	if (gl_data.looking_at == L"SOUTH"){
		draw_grid({ gist_d.end_x, gist_d.start_y }, { gist_d.start_x, gist_d.start_y },
		{ gist_d.start_x, gist_d.end_y }, { gist_d.end_x, gist_d.end_y });
	}
	if (gl_data.looking_at == L"WEST"){
		draw_grid({ gist_d.start_x, gist_d.start_y }, { gist_d.start_x, gist_d.end_y },
		{ gist_d.end_x, gist_d.end_y }, { gist_d.end_x, gist_d.start_y });
	}
	if (gl_data.looking_at == L"EAST"){
		draw_grid({ gist_d.end_x, gist_d.end_y }, { gist_d.end_x, gist_d.start_y },
		{ gist_d.start_x, gist_d.start_y }, { gist_d.start_x, gist_d.end_y });
	}
	glEnd();
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	
}

