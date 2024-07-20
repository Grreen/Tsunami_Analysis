#define _USE_MATH_DEFINES 
#include "plot.h"
//#include "geometr.h"
#include "interface_GL.h"
#include "calculation.h"
#include "displaying.h"
#include "interface_nana.h"
#include <string>
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
extern nana::string p_barchart;
struct coord_double{
	double x, y;
};
coord_double crd1 = { 0, 0 }, crd2{0, 0};

coord_double get_coord_double(int x, int y, int s_window_x, int s_window_y){
	return{ (double(x) / double(s_window_x - 1))*(end_x-start_x) + start_x, 
		( double(s_window_y - y - 1) / double(s_window_y - 1))*(end_y - start_y) + start_y };

}

void update_pic(nana::drawing dr){
	dr.update();
}

void output_gist_data(){
	cout << "START_X: " << gist_d.start_x << "\tSTART_Y: " << gist_d.start_y << endl;
	cout << "END_X: " << gist_d.end_x << "\tEND_Y: " << gist_d.end_y << endl;
	cout << "SIZE_X: " << gist_d.size_x << "\tSIZE_Y: " << gist_d.size_y << endl;
	//cout << "|_" << gist_d.l_l.x << "\t_|" << gist_d.l_r << endl;
}

void fm_select_area(){
	coord_double c1 = { start_x + delta_x / 2., end_y - delta_y / 2. }, c2{ start_x + delta_x / 2., end_y - delta_y / 2. };
	crd1 = { start_x, end_y }; crd2 = { start_x, end_y };
	//set_delta(variant_delta_t);
	//fm_param_set();
	using namespace nana;


	form fm(API::make_center(600, 400));
	fm.caption(L"Select area");
	nana::picture pic(fm);

	nana::drawing dw1(pic);// , dw2(pic);

	//pic.move({ 10, 60 });
	pic.size(nana::size{ (unsigned int)size_y, (unsigned int)size_x });


	//_______________________________________________//

	//-----------------FORM--------------------------//
	form fm1(API::make_center(350, 180), appear::decorate<appear::minimize, appear::maximize>());
	fm1.caption(L"Parametr's");
	//-----------------TextBox--------------------------//
	textbox tb_x1(fm1);
	tb_x1.tip_string(L"X1").multi_lines(false);
	API::eat_tabstop(tb_x1, false);
	tb_x1.append(to_wstring(c1.x), true);
	tb_x1.events().click([&dw1](){ dw1.update(); });
	//------------------TextBox-------------------------//
	textbox tb_y1(fm1);
	tb_y1.tip_string(L"Y1").multi_lines(false);
	API::eat_tabstop(tb_y1, false);
	tb_y1.append(to_wstring(c1.y), true);
	tb_y1.events().click([&dw1](){ dw1.update(); });
	//------------------TextBox-------------------------//
	textbox tb_x2(fm1);
	tb_x2.tip_string(L"X2").multi_lines(false);
	API::eat_tabstop(tb_x2, false);
	tb_x2.append(to_wstring(c2.x), true);
	tb_x2.events().click([&dw1](){ dw1.update(); });
	//------------------TextBox-------------------------//
	textbox tb_y2(fm1);
	tb_y2.tip_string(L"Y2").multi_lines(false);
	API::eat_tabstop(tb_y2, false);
	tb_y2.append(to_wstring(c2.y), true);
	tb_y2.events().click([&dw1](){ dw1.update(); });
	//------------------Pic_draw--------------------------//
	bool status_map = false;
	fm.events().resized([&status_map]()
	{
		status_map = false;
	});
	nana::paint::graphics gr;
	dw1.draw([&dw1, &pic, &tb_x1, &tb_x2, &tb_y1, &tb_y2, &c1, &c2, &gr, &status_map](nana::paint::graphics& graph){
		gr.make(nana::size(size_x, size_y));
		nana::color col = { 0, 0, 0 };
		
		double e_x = max(c1.x, c2.x);
		double e_y = max(c1.y, c2.y);
		double s_x = min(c1.x, c2.x);
		double s_y = min(c1.y, c2.y);
		coord l_l = get_coord(s_x, s_y), t_r = get_coord(e_x, e_y);

		std::wcout << l_l.x << L" _ " << l_l.y << L" | " << t_r.x << L" _ " << t_r.y << std::endl;

		for (int y = l_l.y; y <= t_r.y; y++)
		{
			if (bottom[y][l_l.x] >= 0.0) 
				gr.set_pixel(l_l.x, y, {0, 255, 0});
			else 
				gr.set_pixel(l_l.x, y, { 0, 0, 255 });
			if (bottom[y][t_r.x] >= 0.0) 
				gr.set_pixel(t_r.x, y, {0, 255, 0});
			else
				gr.set_pixel(t_r.x, y, { 0, 0, 255 });
		}
		for (int x = l_l.x; x <= t_r.x; x++)
		{
			if (bottom[l_l.y][x] >= 0.0) 
				gr.set_pixel(x, l_l.y, {0, 255, 0});
			else 
				gr.set_pixel(x, l_l.y, { 0, 0, 255 });
			if (bottom[t_r.y][x] >= 0.0) 
				gr.set_pixel(x, t_r.y, { 0, 255, 0});
			else 
				gr.set_pixel(x, t_r.y, { 0, 0, 255 });
		}
		gr.stretch(graph, rectangle(0, 0, pic.size().width, pic.size().height));

		c1.x = tb_x1.to_double(); c1.y = tb_y1.to_double();
		c2.x = tb_x2.to_double(); c2.y = tb_y2.to_double();
		e_x = max(c1.x, c2.x);
		e_y = max(c1.y, c2.y);
		s_x = min(c1.x, c2.x);
		s_y = min(c1.y, c2.y);
		l_l = get_coord(s_x, s_y); t_r = get_coord(e_x, e_y);
		if (!status_map){
			status_map = true;
			for (int y = 0; y < size_y; y++) {
				for (int x = 0; x < size_x; x++) {
					nana::color c;
					if (bottom[y][x] >= 0.0) {
						c = { 0, 255, 0 };
					}
					else {

						c = { 0, 0, 255 };
					}

					gr.set_pixel(x, y, c);
				}

			}
		}
		for (int y = t_r.y; y <= l_l.y; y++){
			gr.set_pixel(l_l.x, y, col);
			gr.set_pixel(t_r.x, y, col);
		}
		for (int x = l_l.x; x <= t_r.x; x++){
			gr.set_pixel(x, l_l.y, col);
			gr.set_pixel(x, t_r.y, col);
		}

		gr.stretch(graph, rectangle(0, 0, pic.size().width, pic.size().height));
	});
	

	//-----------------------------------------------------//
	place pl(fm);
	pl.div("<pic>");
	pl.field("pic") << pic;
	pl.collocate();
	//----------------------------------------------------//
	pic.events().mouse_down([&fm, &tb_x1, &tb_x2, &tb_y1, &tb_y2, &dw1](const nana::arg_mouse & arg){
		crd2 = crd1;
		crd1 = get_coord_double(arg.pos.x, arg.pos.y, fm.size().width, fm.size().height);
		cout << crd1.x << "  |  " << crd1.y << endl;
		tb_x1.select(true);	tb_x1.del();
		tb_x1.append(to_wstring(crd1.x), 0);
		tb_x2.select(true);  tb_x2.del();
		tb_x2.append(to_wstring(crd2.x), 0);
		tb_y1.select(true); tb_y1.del();
		tb_y1.append(to_wstring(crd1.y), 0);
		tb_y2.select(true);  tb_y2.del();
		tb_y2.append(to_wstring(crd2.y), 0);
		dw1.update();
	});
	//------------------BUTTON-------------------------//
	button select_all(fm1);
	select_all.caption(L"All area");
	select_all.events().click([&fm, &fm1]{
		gist_d.l_l = {0 , size_y - 1};
		gist_d.t_r = { size_x - 1, 0};
		gist_d.start_x = start_x;
		gist_d.start_y = start_y;
		gist_d.end_x = end_x;
		gist_d.end_y = end_y;
		gist_d.size_x = size_x;
		gist_d.size_y = size_y;
		read_array(p_barchart);
		find_comps();
		fm.close();
		fm1.close();
	});
	//------------------BUTTON-------------------------//
	button load_area(fm1, L"Load from file");
	load_area.events().click([&fm, &fm1](){
		filebox fb(true);
		fb.add_filter(L"Load area", L"*.gldat;");
		if (fb()){
			load_position(fb.file(), true);
			output_gist_data();
			read_array(p_barchart);
			find_comps();
			fm.close();
			fm1.close();
		}
	});
	
	button select_area(fm1);
	select_area.caption(L"Select area");
	select_area.events().click([&tb_x1, &tb_x2, &tb_y1, &tb_y2, &fm, &fm1]{
		crd1.x = tb_x1.to_double(); crd1.y = tb_y1.to_double();
		crd2.x = tb_x2.to_double(); crd2.y = tb_y2.to_double();
		gist_d.end_x = max(crd1.x, crd2.x);
		gist_d.end_y = max(crd1.y, crd2.y);
		gist_d.start_x = min(crd1.x, crd2.x);
		gist_d.start_y = min(crd1.y, crd2.y);
		gist_d.l_l = get_coord(gist_d.start_x, gist_d.start_y);
		gist_d.t_r = get_coord(gist_d.end_x, gist_d.end_y);
		gist_d.size_x = gist_d.t_r.x - gist_d.l_l.x + 1;
		gist_d.size_y = (gist_d.l_l.y - gist_d.t_r.y + 1);
		output_gist_data();
		read_array(p_barchart);
		find_comps();
		fm.close();
		fm1.close();
	});
	//--------------------PLACE-----------------------//
	place pl1(fm1);
	pl1.div("<> <weight=90% vert < weight = 60 vert <weight = 10 vert> < <  <gap=10 tb1>  > <weight = 10> <gap=10 tb2> > <> > <weight = 90% vert < weight=90 vert gap=10 btn > > > <>");
	pl1.field("tb1") << tb_x1 << tb_y1;
	pl1.field("tb2") << tb_x2 << tb_y2;
	pl1.field("btn") << select_all << select_area << load_area;
	pl1.collocate();
	fm.show();
	fm1.show();
	nana::exec();
	
}