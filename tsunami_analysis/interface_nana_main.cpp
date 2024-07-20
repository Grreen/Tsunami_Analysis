//#define _CRT_SECURE_NO_WARNINGS 
#define _USE_MATH_DEFINES 
#include "plot.h"
//#include "geometr.h"
#include "calculation.h"
#include "interface_nana.h"
#include <cstdlib>
#include <nana/gui/widgets/combox.hpp>
#include <nana/threads/pool.hpp>
#include <nana/system/platform.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/filebox.hpp>


using namespace std;
nana::string p_dat;
nana::string p_barchart;
nana::string p_brick;// double pi = 1;

void mn_enabled_check(nana::menubar mn, bool enbl){
	
}


void fm_main()
{
	
	using namespace nana;
	nana::threads::pool pool_bar_chart;
	form fm_main_menu(API::make_center(450, 300), appear::decorate<>());
	fm_main_menu.caption(L"Tsunami analysis: Main menu");
	//scale_c_h();
	//color_ind(size_y);
	//button btn_run_culc(fm_main_menu);
	//btn_run_culc.caption(L"Run calculation");
	//API::eat_tabstop(btn_run_culc, false);
	/*btn_run_culc.events().click([]{
		read_dat(p_dat);
		fm_calc();
	});*/
	/*button btn_bar_chart(fm_main_menu);
	btn_bar_chart.caption(L"Bar chart");
	API::eat_tabstop(btn_bar_chart, false);
	btn_bar_chart.events().click(pool_push(pool_bar_chart, []{
		bar_chart_gl();
	}));*/
	//---------------LABEL's---------------//
	label lbl_time(fm_main_menu);
	lbl_time.caption(L"Time:");

	label lbl_izobata(fm_main_menu);
	lbl_izobata.caption(L"Izobata:");

	label lbl_output_time(fm_main_menu);
	lbl_output_time.caption(L"Output t:");

	//---------------TEXTBOX---------------//
	textbox tb_izobata(fm_main_menu);
	tb_izobata.multi_lines(false);
	API::eat_tabstop(tb_izobata, false);
	tb_izobata.append(to_wstring(izobata) , false);
	tb_izobata.events().text_changed([&tb_izobata]
	{
		if (tb_izobata.edited()){
			if (tb_izobata.to_double() < -0.01)
			izobata = tb_izobata.to_double();
		}
	});
	//---------------TEXTBOX---------------//
	textbox tb_time_moments(fm_main_menu);
	tb_time_moments.multi_lines(false);
	API::eat_tabstop(tb_time_moments, false);
	
	tb_time_moments.append(to_wstring(time_moments), false);
	tb_time_moments.events().text_changed([&tb_time_moments]
	{
		if (tb_time_moments.edited()){
			if (tb_time_moments.to_int() > 0)
			time_moments = tb_time_moments.to_int();
		}
	});

	//---------------TEXTBOX---------------//
	textbox tb_output_time(fm_main_menu);
	tb_output_time.multi_lines(false);
	API::eat_tabstop(tb_output_time, false);
	tb_output_time.append(to_wstring(output_time), false);
	tb_output_time.events().text_changed([&tb_output_time]
	{
		if (tb_output_time.edited()){
			output_time = tb_output_time.to_int();
		}
	});
	//---------------MENU_BAR---------------//
	menubar mn(fm_main_menu);


	mn.push_back(L"File");
	mn.push_back(L"Show");
	mn.push_back(L"Ñalculation");
	//---------------MENU_BAR_0_0---------------//
	mn.at(0).append(L"Open DAT file", [&fm_main_menu, &mn](nana::menu::item_proxy& ip) {
		
		filebox fb(nullptr, true);
		fb.add_filter(L"Map dat file", L"*.dat;");
		if (fb())
		{
			const nana::string p_dat = fb.file();
			read_dat(p_dat);
			for (int i = 1; i < 3; i++){
				for (int j = 0; j < int(mn.at(i).size()); j++)
				{
					mn.at(i).enabled(j, true);
				}
			}
			mn.at(1).enabled(1, false);
			mn.at(1).enabled(2, false);
			fm_calc();
			
		}
	});
	//---------------MENU_BAR_0_1---------------//
	mn.at(0).append(L"Read MaxVis", [&fm_main_menu, &mn](nana::menu::item_proxy& ip) {
		filebox fb(nullptr, true);
		fb.add_filter(L"Map barchart mtx file", L"*.mtx;");
		if (fb())
		{
			p_barchart = fb.file();
			mn.at(1).enabled(1, true);
			mn.at(1).enabled(2, true);
			fm_select_area();
			
			
			//fm_set_area();
		}
	});
	//---------------MENU_BAR_0_2---------------//
	mn.at(0).append(L"Convertation to dat file", [&fm_main_menu](nana::menu::item_proxy& ip) {
		filebox fb(nullptr, true);
		fb.add_filter(L"Map asc file", L"*.asc;");
		if (fb())
		{
			const nana::string p_asc = fb.file();
			convert_to_dat(p_asc);
		}
	});
	//---------------MENU_BAR_1_0---------------//
	mn.at(1).append(L"Mareograph's", [&fm_main_menu](nana::menu::item_proxy& ip)	{});
	//---------------MENU_BAR_1_1---------------//
	mn.at(1).append(L"Gistogram 2D", [&fm_main_menu](nana::menu::item_proxy& ip)	{			
			//wcout << p_barchart << L"\n"; std::system("pause");
			fm_gist();
	});
	//---------------MENU_BAR_1_2---------------//
	mn.at(1).append(L"Gistogram 3D", [&fm_main_menu](nana::menu::item_proxy& ip)	{
		fm_param_gl();
	});
	//---------------MENU_BAR_1_3---------------//
	mn.at(1).append(L"Brick table", [&fm_main_menu](nana::menu::item_proxy& ip)	{
		filebox fb(nullptr, true);
		fb.add_filter(L"Map brick file", L"*.txt;");
		if (fb())
		{
			const nana::string p_brick = fb.file();
			read_brick_file(p_brick);
		}
	});

	mn.at(1).append(L"eta", [&fm_main_menu](nana::menu::item_proxy& ip)	{fm_calc(); });
	//mn.at(2).append(L"Run calculation", [&fm_main_menu](nana::menu::item_proxy& ip)	{});
	mn.at(2).append(L"Pause", [&fm_main_menu](nana::menu::item_proxy& ip)	{ std::system("pause"); });
	mn.at(2).append(L"Stop calculation", [&fm_main_menu](nana::menu::item_proxy& ip)	{});
	/*for (int i = 0; i < int(mn.at(1).size()); i++){
		mn.at()
	}*/
	place pl_menu(fm_main_menu);
	pl_menu.div("<><weight=80% vert <> <weight=80%  <weight=120 vert <<weight=50 <arrange=20 gap=10 vert lb>> <weight=60 <arrange=20 gap=10 vert tb>>> <vert arrange=20> > > <> ><>");
	pl_menu.field("lb") << lbl_izobata << lbl_time << lbl_output_time;
	pl_menu.field("tb") << tb_izobata << tb_time_moments << tb_output_time;
	for (int i = 1; i < 3; i++){
		for (int j = 0; j < int(mn.at(i).size()); j++)
		{
			mn.at(i).enabled(j, false);
		}
	}
	//pl_menu.field("btn") << btn_run_culc;// << btn_bar_chart;
	//mn_enabled_check(mn, false);
	pl_menu.collocate();
	fm_main_menu.show();
	nana::exec();
}
/*nana::color get_color(double t, bool a){
int h = int(t);
//cout << h << endl;

if (a)
{
if (t <= number_one[1].scale_h){
return{ 255, 255, 255 };

}
if (t <= number_one[2].scale_h && t > number_one[1].scale_h){
return{
unsigned int(number_one[1].scale_c.r + (double(t - number_one[1].scale_h) / double(number_one[1].scale_h - number_one[0].scale_h))*(number_one[2].scale_c.r - number_one[1].scale_c.r)),
unsigned int(number_one[1].scale_c.g + (double(t - number_one[1].scale_h) / double(number_one[1].scale_h - number_one[0].scale_h))*(number_one[2].scale_c.g - number_one[1].scale_c.g)),
unsigned int(number_one[1].scale_c.b + (double(t - number_one[1].scale_h) / double(number_one[1].scale_h - number_one[0].scale_h))*(number_one[2].scale_c.b - number_one[1].scale_c.b))
};
}
if (t <= number_one[3].scale_h && t > number_one[2].scale_h){

return{
unsigned int(number_one[2].scale_c.r + (double(t - number_one[2].scale_h) / double(number_one[3].scale_h - number_one[2].scale_h))*(number_one[3].scale_c.r - number_one[2].scale_c.r)),
unsigned int(number_one[2].scale_c.g + (double(t - number_one[2].scale_h) / double(number_one[3].scale_h - number_one[2].scale_h))*(number_one[3].scale_c.g - number_one[2].scale_c.g)),
unsigned int(number_one[2].scale_c.b + (double(t - number_one[2].scale_h) / double(number_one[3].scale_h - number_one[2].scale_h))*(number_one[3].scale_c.b - number_one[2].scale_c.b))
};
}
if (t <= number_one[4].scale_h && t > number_one[3].scale_h){

return{
unsigned int(number_one[3].scale_c.r + (double(t - number_one[3].scale_h) / double(number_one[4].scale_h - number_one[3].scale_h))*(number_one[4].scale_c.r - number_one[3].scale_c.r)),
unsigned int(number_one[3].scale_c.g + (double(t - number_one[3].scale_h) / double(number_one[4].scale_h - number_one[3].scale_h))*(number_one[4].scale_c.g - number_one[3].scale_c.g)),
unsigned int(number_one[3].scale_c.b + (double(t - number_one[3].scale_h) / double(number_one[4].scale_h - number_one[3].scale_h))*(number_one[4].scale_c.b - number_one[3].scale_c.b))
};
}
if ((t <= number_one[5].scale_h && t > number_one[4].scale_h)){

return{
unsigned int(number_one[4].scale_c.r + (double(t - number_one[4].scale_h) / double(number_one[5].scale_h - number_one[4].scale_h))*(number_one[5].scale_c.r - number_one[4].scale_c.r)),
unsigned int(number_one[4].scale_c.g + (double(t - number_one[4].scale_h) / double(number_one[5].scale_h - number_one[4].scale_h))*(number_one[5].scale_c.g - number_one[4].scale_c.g)),
unsigned int(number_one[4].scale_c.b + (double(t - number_one[4].scale_h) / double(number_one[5].scale_h - number_one[4].scale_h))*(number_one[5].scale_c.b - number_one[4].scale_c.b))
};
}


}
if (!a){
if (h <= z[0]){
return{ 255, 255, 255 };

}
if (h <= z[1] && h > z[0]){


return{
unsigned int(number_one[1].scale_c.r + (double(h - z[0]) / double(z[1] - z[0])) * (number_one[2].scale_c.r - number_one[1].scale_c.r)),
unsigned int(number_one[1].scale_c.g + (double(h - z[0]) / double(z[1] - z[0])) * (number_one[2].scale_c.g - number_one[1].scale_c.g)),
unsigned int(number_one[1].scale_c.b + (double(h - z[0]) / double(z[1] - z[0])) * (number_one[2].scale_c.b - number_one[1].scale_c.b)) };

}
if (h <= z[2] && h > z[1]){


return{
unsigned int(number_one[2].scale_c.r + (double(h - z[1]) / double(z[2] - z[1])) * (number_one[3].scale_c.r - number_one[2].scale_c.r)),
unsigned int(number_one[2].scale_c.g + (double(h - z[1]) / double(z[2] - z[1])) * (number_one[3].scale_c.g - number_one[2].scale_c.g)),
unsigned int(number_one[2].scale_c.b + (double(h - z[1]) / double(z[2] - z[1])) * (number_one[3].scale_c.b - number_one[2].scale_c.b)) };

}
if (h <= z[3] && h > z[2]){


return{
unsigned int(number_one[3].scale_c.r + (double(h - z[2]) / double(z[3] - z[2])) * (number_one[4].scale_c.r - number_one[3].scale_c.r)),
unsigned int(number_one[3].scale_c.g + (double(h - z[2]) / double(z[3] - z[2])) * (number_one[4].scale_c.g - number_one[3].scale_c.g)),
unsigned int(number_one[3].scale_c.b + (double(h - z[2]) / double(z[3] - z[2])) * (number_one[4].scale_c.b - number_one[3].scale_c.b)) };

}
if (h <= z[4] && h > z[3]){

return{
unsigned int(number_one[4].scale_c.r + (double(h - z[3]) / double(z[4] - z[3])) * (number_one[5].scale_c.r - number_one[4].scale_c.r)),
unsigned int(number_one[4].scale_c.g + (double(h - z[3]) / double(z[4] - z[3])) * (number_one[5].scale_c.g - number_one[4].scale_c.g)),
unsigned int(number_one[4].scale_c.b + (double(h - z[3]) / double(z[4] - z[3])) * (number_one[5].scale_c.b - number_one[4].scale_c.b)) };
}
}
return{ 0, 0, 0 };
}*/
/*if (bottom[y][x] >= 0. && bottom[y][x] < 100) c = { 0, 255, 0 };
if (bottom[y][x] >= 100 && bottom[y][x] < 150) c = { 0, 235, 0 };
if (bottom[y][x] >= 150 && bottom[y][x] < 200) c = { 0, 220, 0 };
if (bottom[y][x] >= 200 && bottom[y][x] < 250) c = { 0, 200, 0 };
if (bottom[y][x] >= 250 && bottom[y][x] < 300) c = { 0, 180, 0 };
if (bottom[y][x] >= 300 && bottom[y][x] < 400) c = { 0, 160, 0 };
if (bottom[y][x] >= 400 && bottom[y][x] < 500) c = { 0, 140, 0 };
if (bottom[y][x] >= 500 && bottom[y][x] < 750) c = { 0, 120, 0 };
if (bottom[y][x] >= 750 && bottom[y][x] < 1000) c = { 0, 100, 0 };
if (bottom[y][x] >= 1000 && bottom[y][x] < 1250) c = { 0, 80, 0 };
if (bottom[y][x] >= 1250 && bottom[y][x] < 1500) c = { 0, 60, 0 };
if (bottom[y][x] >= 1500 && bottom[y][x] < 1750) c = { 0, 40, 0 };
if (bottom[y][x] >= 1750 && bottom[y][x] < 2200) c = { 0, 20, 0 };
if (bottom[y][x] >= 2200) c = { 0, 0, 0 };
*/
/*paint::pixel_buffer pbuffer;
rectangle r;
r.x = 0;
r.y = 0;
r.width = pic.size().width;
r.height = pic.size().height;
pbuffer.attach(gr.handle(), r);
for (int y = 0; y <size_y; y++) {
for (int x = 0; x < size_x; x++) {
nana::color c;
if (bottom[y][x] >= 0.0 ){
c = cmap2.get_color(bottom[y][x]);
if (bottom[y][x] < 0) c = { 38, 226, 255 };
}
else {

if (out_s_type == L"eta") c = cmap.get_color(eta[y][x]);
if (out_s_type == L"MaxVis") c = cmap.get_color(maxVis[y][x]);
if (out_s_type == L"MinVis")  c = {255, 255, 255};
}
pixel_color_t* cur_color = pbuffer[y] + x;
cur_color->value = c.argb().value;
//gr.set_pixel(x, y, c);
}
}
pbuffer.close();
gr.stretch(graph, rectangle(10, 30, pic.size().width - 100, pic.size().height - 50));*/
/*int h = 0;
auto sz = graph.size();

int a = 50, b = sz.height - 50;
double step = -((number_one[5].scale_h - number_one[0].scale_h) * (double)(1) / (-b + a) + number_one[0].scale_h) + (number_one[0].scale_h);
for (int y = b; y >= a; y--) {
double val = (number_one[5].scale_h - number_one[0].scale_h) * (double)(y - b) / (-b + a) + number_one[0].scale_h;
int sx = pic.size().width - 30;
for (int j = 0; j < 6; j++)
{
//nana ::
if (val == number_one[j].scale_h) {
nana::string a = to_wstring(val);
a = to_wstring_short(a, 1);
graph.string({ sx - 5, y  }, a);
//if(j==0) graph.string({ sx - 5, y - 20  }, to_wstring(int(val)));
//if (j == 5) graph.string({ sx - 5, y + 10 }, to_wstring(int(val)));
}
else if (val >number_one[j].scale_h && val - step < number_one[j].scale_h)	{
nana::string a = to_wstring(val);
a = to_wstring_short(a, 1);
graph.string({ sx - 5, y }, a);
}
}
nana::color c = cmap.get_color(val);
//printf("%lf %lf %lf\n", c.r(), c.g(), c.b());
for (int x = pic.size().width - 50; x < pic.size().width - 20; x++) {
graph.set_pixel(x, y, c);
}
}*/
/*nana::string to_wstring_short(nana::string a, int len){
bool flag = false;
int i;
for (i = 0; a[i]!=L'.'; i++){
//if (a[i] == '.') i++;
}
for (int j = i + len + 1; j < int(a.size()); j++)
a[j] = L'\0';
return a;
}*/
/*void fm_select_area(){
scale_c_h();
nana::form fm{ API::make_center(800, 400) };
nana::colormap cmap({ { number_one[0].scale_h, number_one[0].scale_c }, { number_one[1].scale_h, number_one[1].scale_c }, { number_one[2].scale_h, number_one[2].scale_c }, { number_one[3].scale_h, number_one[3].scale_c }, { number_one[4].scale_h, number_one[4].scale_c }, { number_one[5].scale_h, number_one[5].scale_c }, { number_one[6].scale_h, number_one[6].scale_c }, { number_one[7].scale_h, number_one[7].scale_c } });
nana::colormap cmap2({ { bath_earth[0].scale_h, bath_earth[0].scale_c }, { bath_earth[1].scale_h, bath_earth[1].scale_c }, { bath_earth[2].scale_h, bath_earth[2].scale_c }, { bath_earth[3].scale_h, bath_earth[3].scale_c }, { bath_earth[4].scale_h, bath_earth[4].scale_c } });
nana::picture pic(fm);
nana::drawing dw1(pic);
}*/
/*const wchar_t * n_dat;
const wchar_t * n_brick;
const wchar_t * n_barchart;*/
/*struct my_color{
int r; int g; int b;
};*/
