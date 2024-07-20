#define _USE_MATH_DEFINES 
#include "plot.h"
//#include "geometr.h"
#include "interface_GL.h"
#include "calculation.h"
#include "displaying.h"
#include "interface_nana.h"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <fstream>
#include <nana/gui/widgets/combox.hpp>
#include <nana/threads/pool.hpp>
#include <nana/system/platform.hpp>
#include <nana/fwd.hpp>
#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/listbox.hpp>
using namespace std;


struct gist2d_gl{
	std::wstring looking_at;
	int check;
	double h;
	double step_x, step_y, step_font_x, step_font_y;
	bool save;
	int str_save;
	std::wstring color;
	//double aver, max_h;
};

gist2d_gl gist2d_d = {L"NORTH", 0, 1, 2, 0.1, 1, 0.1, true, 0, L"RED"};

struct gist_info{
	int n;
	nana::string name;
	bool north, south, west, east;
};

nana::listbox::oresolver& operator << (nana::listbox::oresolver& or, const gist_info& pr)
{
	or << pr.n;
	or << pr.name;
	return or;
}
std::wostream & operator << (std::wostream& or, const gist_info& pr)
{
	or << pr.n;
	or << pr.name;
	return or;
}
nana::listbox::iresolver& operator >> (nana::listbox::iresolver& or, gist_info& pr)
{
	or >> pr.n;
	or >> pr.name;
	return or;
}

void print(listbox & l)
{
	for (auto x : l.at(0)){
		printf("pos: %d ", x.pos().item);
		wcout << "'" << x.text(0) << "' ";
		auto y = x.value_ptr<gist_info>();
		if (y == nullptr) printf("nullptr\n");
		else printf("num: %d\n", y->n);
	}
}
vector <vector <double>> vct_gist_d;

void gist2d_file_new(std::wstring path){
	int size = 0, amount = vct_gist_d.size();  char c;
	if (gist2d_d.looking_at == L"NORTH") { c = 'N'; size = gist_d.size_x; }
	if (gist2d_d.looking_at == L"SOUTH") { c = 'S'; size = gist_d.size_x; }
	if (gist2d_d.looking_at == L"WEST") { c = 'W'; size = gist_d.size_y; }
	if (gist2d_d.looking_at == L"EAST") { c = 'E'; size = gist_d.size_y; }
	
	wfstream file;
	file.open(path.c_str(), std::wfstream::out);
	file << amount << " " << size << " " << c << endl;
	for (int i = 0; i < amount; i++){
		for (int j = 0; j < size; j++){
			file << vct_gist_d[i][j]; 
			if(j < size - 1) file << " ";
		}
		file << endl;
	}
	file << gist2d_d.step_font_x << " " << gist2d_d.step_font_y << " " << gist2d_d.step_x << " " << gist2d_d.step_y << endl;
}

void gist2d_file(std::wstring path, bool add){
	
	
	if (!add) vct_gist_d.clear();
	wfstream file;
	file.open(path.c_str(), std::wfstream::in);
	int amount = 0;
	int size = 0;
	wchar_t a = 0;
	file >> amount >> size >> a;
	if (a == L'N') gist2d_d.looking_at = L"NORTH";
	if (a == L'S') gist2d_d.looking_at = L"SOUTH";
	if (a == L'W') gist2d_d.looking_at = L"WEST";
	if (a == L'E') gist2d_d.looking_at = L"EAST";

	for (int i = 0; i < amount; i++)
	{
		vector <double> buf;
		for (int j = 0; j < size; j++){
			double val = 0;
			file >> val;
			buf.push_back(val);
		}
		vct_gist_d.push_back(buf);
	}
	file >> gist2d_d.step_font_x >> gist2d_d.step_font_y >> gist2d_d.step_x >> gist2d_d.step_y;
	if (add) gist2d_file_new(path);
		
}


double max_h(int s){
	if (!v_c_c.empty()){
		double m = CComp[v_c_c[s][0].y][v_c_c[s][0].x];
		for (int i = 1; i < v_c_c[s].size(); i++){
			if (m < CComp[v_c_c[s][i].y][v_c_c[s][i].x]) m = CComp[v_c_c[s][i].y][v_c_c[s][i].x];
		}
		return m;
	}
	return - 1;
}


void h_array(){
	if (!vct_gist_d.empty()) {
		vct_gist_d.clear();
	}
	unsigned int l;
	if (gist2d_d.looking_at == L"NORTH" || gist2d_d.looking_at == L"SOUTH") l = gist_d.size_x; 
	else l = gist_d.size_y;
	double *arr = new double[l];
	
	if (gist2d_d.looking_at == L"NORTH")
	{
		memset(arr, 0, gist_d.size_x * sizeof(double));
		int* buf = new int[v_c_c[gist2d_d.check].size()];
		memset(buf, 0, v_c_c[gist2d_d.check].size()*sizeof(int));
		for (int i = 0; i < v_c_c[gist2d_d.check].size(); i++){
			coord a = v_c_c[gist2d_d.check][i];
			if ( (buf[a.x] < a.y) ) {
				buf[a.x] = a.y;
				arr[a.x] = CComp[a.y][a.x];
			}
		}
	}
	else if (gist2d_d.looking_at == L"SOUTH")
	{
		memset(arr, 0, gist_d.size_x * sizeof(double));
		int* buf = new int[v_c_c[gist2d_d.check].size()];
		memset(buf, 0, v_c_c[gist2d_d.check].size() * sizeof(int));
		for (int i = 0; i < v_c_c[gist2d_d.check].size(); i++){
			coord a = v_c_c[gist2d_d.check][i];
			if ((buf[a.x] > a.y)) {
				buf[a.x] = a.y;
				arr[a.x] = CComp[a.y][a.x];
			}
		}
	}
	else if (gist2d_d.looking_at == L"EAST")
	{
		memset(arr, 0, gist_d.size_y * sizeof(double));
		int* buf = new int[v_c_c[gist2d_d.check].size()];
		memset(buf, 0, v_c_c[gist2d_d.check].size() * sizeof(int));
		for (int i = 0; i < v_c_c[gist2d_d.check].size(); i++){
			coord a = v_c_c[gist2d_d.check][i];
			if ((buf[a.y] > a.x)) {
				buf[a.y] = a.x;
				arr[gist_d.size_y - a.y] = CComp[a.y][a.x];
			}
		}

	}
	else if (gist2d_d.looking_at == L"WEST")
	{
		memset(arr, 0, gist_d.size_y * sizeof(double));
		int* buf = new int[v_c_c[gist2d_d.check].size()];
		memset(buf, 0, v_c_c[gist2d_d.check].size() * sizeof(int));
		for (int i = 0; i < v_c_c[gist2d_d.check].size(); i++){
			coord a = v_c_c[gist2d_d.check][i];
			if ((buf[a.y] < a.x)) {
				buf[a.y] = a.x;
				arr[gist_d.size_y - a.y] = CComp[a.y][a.x];
			}
		}
	}
	vector <double> buf;
	for (int i = 0; i < l; i++) 
	{
		if (i % 2 == 0 && false){
			buf.push_back(0);
		}
		else buf.push_back(arr[i]);
	}
	printf("\n");
	vct_gist_d.push_back(buf);
}
coord get_coord_gist_2d(double x, double y){
	return{ (int)((x - gist_d.start_x) / (gist_d.end_x - gist_d.start_x)*(gist_d.size_x)),
		(int)((y - gist_d.start_y) / (gist_d.end_y - gist_d.start_y)*(gist_d.size_y)) };
}



void fm_param_nana(){
	gist2d_d.h = max_h(gist2d_d.check) + max_h(gist2d_d.check) / 10;
	std::string path_pic;
	//----------------------------------------------------------------------//
	//---------------------------------FORM---------------------------------//
	//----------------------------------------------------------------------//
	form fm;
	fm.move(0, 0);
	fm.size({ 420, 280 });
	fm.caption(L"DATA");
	
	//------------------------------------------------------------------------//
	//---------------------------------BUTTON---------------------------------//
	//------------------------------------------------------------------------//
	button btn_save(fm, L"Save all"), btn_load(fm, L"Load from file"), btn_create(fm, L"Create file"), btn_save_pic(fm, L"Save picture");

	btn_save_pic.events().click([ &path_pic](){
		if (!gist2d_d.save){
			gist2d_d.str_save++;
			gist2d_d.save = true;
		}
	});

	btn_load.events().click([](){
		filebox fb(0, true);
		fb.add_filter(L"Load", L"*.gist;");
		if (fb()) gist2d_file(fb.file(), false);			
	});

	btn_save.events().click([](){
		filebox fb(0, false);
		fb.add_filter(L"Save all", L"*.gist;");
		if (fb()) gist2d_file(fb.file(), true);
	});
	btn_create.events().click([](){
		filebox fb(0, false);
		fb.add_filter(L"Create new", L"*.gist;");
		if (fb()) gist2d_file_new(fb.file());
	});
	//************************************************************************//
	//*********************************COMBOX*********************************//
	//************************************************************************//

	combox cm_looking_at(fm), cm_color(fm);
	cm_looking_at.caption(L"NORTH");
	cm_looking_at.push_back(L"NORTH");
	cm_looking_at.push_back(L"SOUTH");
	cm_looking_at.push_back(L"EAST");
	cm_looking_at.push_back(L"WEST");
	cm_looking_at.events().selected([&cm_looking_at](const arg_combox &ar_cbx){ gist2d_d.looking_at = ar_cbx.widget.caption(); });

	cm_color.caption(L"RED");
	cm_color.push_back(L"RED");
	cm_color.push_back(L"BLUE");
	cm_color.push_back(L"GREEN");
	cm_color.push_back(L"YELLOW");
	cm_color.events().selected([&cm_color](const arg_combox &ar_cbx){ gist2d_d.color = ar_cbx.widget.caption();	});
	//************************************************************************//
	//**********************************LABEL*********************************//
	//************************************************************************//

	label lbl_step_x(fm);
	lbl_step_x.caption(L"     Step axis X:");

	label lbl_step_y(fm);
	lbl_step_y.caption(L"     Step axis Y:");

	label lbl_step_font_x(fm);
	lbl_step_font_x.caption(L"     Step font X:");

	label lbl_step_font_y(fm);
	lbl_step_font_y.caption(L"     Step font Y:");

	label lbl_look(fm);
	lbl_look.caption(L"     Looking at:");

	label lbl_color(fm);
	lbl_color.caption(L"          Color:");
	
	label lbl_height(fm, L"    Axis height");
	
	label lbl_pust1(fm, L" "), lbl_pust2(fm, L" "), lbl_pust3(fm, L" "), lbl_pust4(fm, L" ");
	//************************************************************************//
	//********************************TEXT_BOX********************************//
	//************************************************************************//

	textbox tb_step_x(fm), tb_step_y(fm), tb_height(fm), tb_step_font_x(fm), tb_step_font_y(fm), ratio_axis(fm);
	tb_step_x.append(to_wstring(gist2d_d.step_x), true); tb_step_y.append(to_wstring(gist2d_d.step_y), true);// step_z.append(L"5", true);
	tb_step_font_x.append(to_wstring(gist2d_d.step_font_x), true); 
	tb_step_font_y.append(to_wstring(gist2d_d.step_font_y), true);
	ratio_axis.append(L"1.0", true);

	ratio_axis.events().text_changed([&ratio_axis](){
		if (ratio_axis.to_double() > -0.01){
			gl_data.ratio = ratio_axis.to_double();
		}
	});

	tb_step_font_x.events().text_changed([&tb_step_font_x](){
		if (tb_step_font_x.to_double() > 0.01){
			gist2d_d.step_font_x = tb_step_font_x.to_double();
		}
	});

	tb_step_font_y.events().text_changed([&tb_step_font_y](){
		if (tb_step_font_y.to_double() > 0.01){
			gist2d_d.step_font_y = tb_step_font_y.to_double();
		}
	});

	tb_step_x.events().text_changed([&tb_step_x](){
		if (tb_step_x.to_double()>0.001){
			gist2d_d.step_x = tb_step_x.to_double();
		}
		
	});

	tb_step_y.events().text_changed([&tb_step_y](){
		if (tb_step_y.to_double()>0.01){
			gist2d_d.step_y = tb_step_y.to_double();
		}
	});
	tb_height.append(to_wstring(gist2d_d.h), true);
	tb_height.events().text_changed([&tb_height]() {
		if (!tb_height.empty()){
			gist2d_d.h = tb_height.to_double();
			if (gist2d_d.h <= 0)
				gist2d_d.h = 1.;
		}
	});
	tb_height.events().dbl_click([&tb_height](){
		gist2d_d.h = max_h(gist2d_d.check) + max_h(gist2d_d.check) / 10;
		tb_height.select(true);	tb_height.del();
		tb_height.append(to_wstring(gist2d_d.h), true);
	});
	
	API::eat_tabstop(tb_step_x, false);
	API::eat_tabstop(tb_step_y, false);
	API::eat_tabstop(tb_step_font_x, false);
	API::eat_tabstop(tb_step_font_y, false);
	API::eat_tabstop(tb_height, false);
	API::eat_tabstop(ratio_axis, false);
	//-----------------------------------------------------------------------//
	//---------------------------------PLACE---------------------------------//
	//-----------------------------------------------------------------------//
	place pl(fm);
	pl.div(
		"<weight = 10>"
		"<weight = 90% vert "
		"<weight = 10 vert>"
		"< vert"
		"<weight=0 vert <"
		"<vert gap=10 lbl_s1>"
		" < weight=35 vert gap=10 step1>"
		" <vert gap=10 lbl_s2>"
		"< weight=35 vert gap=10 step2>"
		"<vert gap=10 lbl_s3>"
		"< weight=35 vert gap=10 step3>  > >"
		"<weight=10 vert>"
		"< weight= 105  vert  <"
		"<vert gap=10 lbl>  "
		"<vert gap=10 eve> "
		"<vert gap=10 lbl2>"
		"<vert gap=10 eve2> > >  >"
		"< weight=60 vert < <weight=10 vert> < vert gap=10 btn1> < weight=15> <vert gap=10 btn2> > > <weight=10 vert>"
		">"
		"<weight = 10>");
	//pl.div("<weight = 5> < weight=90% vert < <weight=10 vert <ara> > <> > > <weight = 5>");
	pl.field("eve") << tb_step_x << tb_step_font_x << cm_looking_at << tb_height;// << cm1 << cm2;
	pl.field("eve2") << tb_step_y << tb_step_font_y << cm_color << lbl_pust1;//lbl_pust3;// << cm1 << cm2;

	pl.field("lbl") << lbl_step_x << lbl_step_font_x << lbl_look << lbl_height;
	pl.field("lbl2") << lbl_step_y << lbl_step_font_y << lbl_color << lbl_pust2;//lbl_pust2;

	pl.field("btn1") << btn_load << btn_create;
	pl.field("btn2") << btn_save << btn_save_pic;

	pl.collocate();
	fm.show();
	nana::exec();
}




double adapt_double(double start, double step, bool x){
	double b = 0;
	if (x){
		b = int(start);
		while (b < start) b += step;
	}
	else{
		//b = int(start) + 1;
		b = 0;
		while (b < start - step) b += step;
	}
		return b;
}
double max_h_x(int a){
	double max = 0;
	for (int i = 0; i < vct_gist_d.size(); i++){
		if (vct_gist_d[i][a] > max) max = vct_gist_d[i][a];
	}
	return max;
}

bool approximatelyEqual(double a, double b, double epsilon) {
	return std::abs(a - b) <= epsilon;
}

void fm_gist()
{
	gist2d_d.check = s_param.NC;
	/****************************************************************/
	/*******************************FORM*****************************/
	/****************************************************************/
	nana::form fm{ API::make_center(800, 400) };
	
	/***************************************************************/
	/****************************COMBOX*****************************/
	/***************************************************************/
	listbox lb_gist(fm);
	lb_gist.append_header(L"  �", 55);
	lb_gist.append_header(L"Name", 110);	
	lb_gist.checkable(true); 
	auto cat = lb_gist.at(0);
	gist_info *info_2d = new gist_info[f_c_c.size()];
	gist2d_d.looking_at = L"NORTH";
	for (int i = 0; i < int(f_c_c.size()); i++)
	{
		
		info_2d[i].n = i;
		info_2d[i].name = L"NoName";
		cat.append(info_2d[i]);
	}
	lb_gist.at(0).at(0).check(true);
	lb_gist.events().checked([&lb_gist](const arg_listbox& a){
		
		auto cr = lb_gist.at(0);
		if (cr.at(gist2d_d.check).checked()) cr.at(gist2d_d.check).check(false);
		for (int i = 0; i < cr.size(); i++){
			if (cr.at(i).checked() == true) gist2d_d.check = i;
		}
	});
	
	
	/***************************************************************/
	/****************************PICTURE****************************/
	/***************************************************************/
	nana::picture pic(fm);

	nana::drawing dw1(pic);
	nana::paint::graphics gr;

	dw1.draw([&dw1, &pic](nana::paint::graphics& graph)
	{
		//if (vct_gist_d[])
		nana::string x_lbl_start, x_lbl_end;
		double s_x, s_y, e_x, e_y;
		s_y = 0;
		e_y = gist2d_d.h;
		if (gist2d_d.looking_at == L"NORTH" || gist2d_d.looking_at == L"SOUTH"){
			x_lbl_start = L"W";
			x_lbl_end   = L"E";
			if (gist_d.start_x > start_x - 0.0001 && gist_d.start_x < start_x + 0.0001) { s_x = gist_d.start_x + 2 * delta_x; }
			else s_x = gist_d.start_x + delta_x; 
			if (gist_d.end_x > end_x - 0.0001 && gist_d.end_x < end_x + 0.0001) { e_x = gist_d.end_x - 2 * delta_x; }
			else e_x = gist_d.end_x;
		}
		else {
			x_lbl_start = L"N";
			x_lbl_end   = L"S";
			if (gist_d.start_y > start_y - 0.0001 && gist_d.start_y < start_y + 0.0001) { s_x = gist_d.start_y + 2 * delta_y; }
			else s_x = gist_d.start_y + delta_y;
			if (gist_d.end_y > end_y - 0.0001 && gist_d.end_y < end_y + 0.0001) { e_x = gist_d.end_y - 2 * delta_y; }
			else e_x = gist_d.end_y;
		}
		nana::paint::graphics gr;
		nana::paint::font fontSave(settings.wsFontName.c_str(), settings.GetFontSize(graph.size().height), true);
		gr.typeface(fontSave);

		plot2d pl(&gr);
		pl.colorbar(false);

		pl.region(real_rectangle{ { s_x, s_y },	{ e_x, e_y} });
		pl.window({ 0, 0, graph.size().width, graph.size().height });

		pl.axis_x(true);
		pl.axis_y(true);
		pl.axis_x_label(x_lbl_start, x_lbl_end);
		auto step = pl.get_step();

		printf("vct_gist_d::size : %d\n", vct_gist_d.size());

		if (vct_gist_d.size() <= 1) h_array();

		double st_x = adapt_double(s_x, gist2d_d.step_x, true), st_y = adapt_double(e_y, gist2d_d.step_y, false);//adapt_double(s_y);

		nana::color* g_col = new color[5];
		g_col[0] = { 220, 0, 0 };//�������
		g_col[1] = { 0, 220, 0 };//�������
		g_col[2] = { 0, 0, 220 };//�����
		g_col[3] = { 220, 220, 0 };
		g_col[4] = { 0, 220, 220 };
		if (gist2d_d.color == L"RED") g_col[0] = { 220, 0, 0 };
		if (gist2d_d.color == L"BLUE") g_col[0] = g_col[2];
		if (gist2d_d.color == L"GREEN") g_col[0] = g_col[1];
		if (gist2d_d.color == L"YELLOW") g_col[0] = g_col[3];
		pl.draw_grid(true, true, gist2d_d.step_x, gist2d_d.step_y, gist2d_d.step_font_x, gist2d_d.step_font_y, gist2d_d.h);
		
		color_func2d f = [&g_col, &step, &st_x, &st_y, &e_y](double x, double y)->color{
			
				color c = { 255, 255, 255 };
				int a = 0; double val = 100;
				if (gist2d_d.looking_at == L"NORTH" || gist2d_d.looking_at == L"SOUTH")  a = get_coord_gist_2d(x, y).x;
				else  a = get_coord_gist_2d(y, x).y;

				for (int i = 0; i < vct_gist_d.size(); i++)
				{
					if (y <= vct_gist_d[i][a] && vct_gist_d[i][a] < val)
					{
						val = vct_gist_d[i][a];
						c = g_col[i];
					}
					
				}

				bool fl = false;
				if (c.argb().element.red == 255 && c.argb().element.green == 255 && c.argb().element.blue == 255) fl = true;
				if (fl && x - step.x <= st_x && x + step.x >= st_x && gist2d_d.step_x != 0){
					if (y + step.y <= max_h_x(a) || y + step.y <= 0.0001) st_x += gist2d_d.step_x;
					c = { 155, 155, 155 };
				}
				if (fl && y + step.y < st_y && y - step.y > st_y && gist2d_d.step_y != 0){
					st_y -= gist2d_d.step_y;

					c = { 155, 155, 155 };
				}
				if (y + step.y <= max_h_x(a) || y + step.y <= 0.0001) st_y = adapt_double(e_y, gist2d_d.step_y, false);

				return c;
			
		};


		pl.plot_2d_color_function(f);
		pl.draw_axis(true, false);
		graph = gr;

		if (gist2d_d.save)
		{
			std::string name = "./gistograms/2D/" + to_string(0) + to_string(gist2d_d.str_save) + ".bmp";
			graph.save_as_file(name.c_str());
		}

		if (gist2d_d.save){
			nana::paint::graphics gr1;
			plot2d save(&gr1);
			save.colorbar(false);
			save.region(real_rectangle{ { s_x, s_y }, { e_x, e_y } });
			save.window({ 0, 0, 1000, 600});
			save.axis_x(true);
			save.axis_y(true);
			save.axis_x_label(x_lbl_start, x_lbl_end);
			save.draw_grid(true, true, gist2d_d.step_x, gist2d_d.step_y, gist2d_d.step_font_x, gist2d_d.step_font_y, gist2d_d.h);
			st_x = adapt_double(s_x, gist2d_d.step_x, true), st_y = adapt_double(e_y, gist2d_d.step_y, false);
			save.plot_2d_color_function(f);
			save.draw_axis(true, false);

		}
		

	});
	fm.events().click([&dw1](){
		dw1.update();
	});
	pic.events().click([&dw1](){dw1.update(); });
	place pl(fm);
	pl.div("<>  < weight = 95% vert <> < weight =90% <pic> <weight=10> <weight = 20% lb> > <> > <>");
	pl.field("lb") << lb_gist;
	pl.field("pic") << pic;
	
	pl.collocate();
	fm.show();
	fm_param_nana();
}