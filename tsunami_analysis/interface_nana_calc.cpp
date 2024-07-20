#define _USE_MATH_DEFINES 
#include "plot.h"
//#include "geometr.h"
#include "calculation.h"
#include "interface_nana.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <nana/gui/widgets/combox.hpp>
#include <nana/threads/pool.hpp>
#include <nana/system/platform.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/button.hpp>

#define PATH_FRONTS "files\\Fronts\\"//WORK MART\\2017MINI\\Fronts\\"//"WORK DECEMBER\\2017MINI2\\Fronts\\"//"WORK SEPTEMBER\\2017MINI\\Fronts\\"
#define PATH_MAX "files\\MaxHeights\\MAX.mtx" //WORK MART\\2017MINI\\MaxHeights\\MAX_2017TEST.mtx"//"WORK DECEMBER\\2017MINI2\\MaxHeights\\MAX_2017MINI.mtx"//"WORK SEPTEMBER\\2017MINI\\MaxHeights\\MAX_2017MINI.mtx"//"WORK SEPTEMBER\\2017 BIG\\MaxHeights\\MAX_2017BIG.mtx"
#define EVENT PACIFIC_CUT//DEFAULT//MEXICO_BIG
constexpr auto MAGNITUDE = 7.5;

#define FONT_SIZE EVENT == MEXICO_BIG ? 72 : 28 
#define WIDTH_MARGIN EVENT == MEXICO_BIG ? 500 : 300
#define HEIGHT_MARGIN EVENT == MEXICO_BIG ? 50 : 20 
//#define X_LBL EVENT == MEXICO_BIG ? L"W" : L"E" 
using namespace std;
struct scale {
	double scale_h;
	nana::color scale_c;
};
scale *number_one = new scale[8];
scale *bath_earth = new scale[5];
int *z = new int[5];

template <typename T> void output_array_max_vis(const std::string& name, T **a){
	fstream file;
	file.open(name, std::fstream::out);

	for (int i = 0; i < size_y; i++){
		for (int j = 0; j < size_x; j++) {
			if (a[i][j] >-0.001)
				file << fixed << setprecision(10) << a[i][j] << "\t";
			else file << fixed << setprecision(7) << a[i][j] << "\t";
		}
		file << "\n";
	}
}

void color_ind(int l2)
{
	z[0] = 0;


	for (int i = 0; i < 5; i++)
	{
		const int l1 = 0;
		z[i] = l1 + int(((number_one[i + 1].scale_h - number_one[0].scale_h) / (number_one[5].scale_h - number_one[0].scale_h))*(l2 - l1));
		//cout << z[i] << endl;
	}

}
nana::string get_time(){
	int time = t - (int(t / 60) * 60);
	nana::string clock_sec = to_wstring((time));
	time = int(t / 60) - (int(t / 60 / 60) * 60);
	nana::string clock_min = to_wstring((time));
	time = int(t / 60 / 60);
	nana::string clock_hour = to_wstring((time));
	if (clock_sec.size() == 1) { clock_sec = L"0" + clock_sec; }
	if (clock_min.size() == 1) { clock_min = L"0" + clock_min; }
	if (clock_hour.size() == 1) { clock_hour = L"0" + clock_hour; }
	nana::string clock_time = clock_hour + L":" + clock_min + L":" + clock_sec;
	return clock_time;
}

void scale_c_h(){
	if (MAGNITUDE > 7.99 && MAGNITUDE < 8.01) {
		number_one[0].scale_c = { 38, 0, 255 };
		number_one[1].scale_c = { 222, 255, 248 };
		number_one[2].scale_c = { 222, 255, 248 };
		number_one[3].scale_c = { 128, 0, 128 };
		number_one[4].scale_c = { 255, 0, 0 };
		//number_one[4].scale_c = { 255, 214, 64 };
		number_one[5].scale_c = { 255, 128, 0 };
		number_one[6].scale_c = { 255, 255, 0 };
		number_one[7].scale_c = { 0, 255, 0 };

		number_one[0].scale_h = -3;
		number_one[1].scale_h = -0.1;
		number_one[2].scale_h = 0;//0;
		number_one[3].scale_h = 1;//0.5;//2;
		number_one[4].scale_h = 3;//2;//5;
		number_one[5].scale_h = 5;
		number_one[6].scale_h = 8;//3.5;//10;
		number_one[7].scale_h = 11;//6.8;//15;
	}
	else if (MAGNITUDE > 8.19 && MAGNITUDE < 8.21) {
		number_one[0].scale_c = { 38, 0, 255 };
		number_one[1].scale_c = { 222, 255, 248 };
		number_one[2].scale_c = { 222, 255, 248 };
		number_one[3].scale_c = { 128, 0, 128 };
		number_one[4].scale_c = { 255, 0, 0 };
		number_one[5].scale_c = { 255, 128, 0 };
		number_one[6].scale_c = { 255, 255, 0 };
		number_one[7].scale_c = { 0, 255, 0 };

		number_one[0].scale_h = -5;
		number_one[1].scale_h = -0.1;
		number_one[2].scale_h = 0;
		number_one[3].scale_h = 1;
		number_one[4].scale_h = 3;
		number_one[5].scale_h = 5;
		number_one[6].scale_h = 8;
		number_one[7].scale_h = 13;
	}
	else if (MAGNITUDE > 7.49 && MAGNITUDE < 7.51) {
		number_one[0].scale_c = { 38, 0, 255 };
		number_one[1].scale_c = { 222, 255, 248 };
		number_one[2].scale_c = { 222, 255, 248 };
		number_one[3].scale_c = { 128, 0, 128 };
		number_one[4].scale_c = { 255, 0, 0 };
		number_one[5].scale_c = { 255, 128, 0 };
		number_one[6].scale_c = { 255, 255, 0 };
		number_one[7].scale_c = { 0, 255, 0 };

		number_one[0].scale_h = -2;
		number_one[1].scale_h = -0.1;
		number_one[2].scale_h = 0;
		number_one[3].scale_h = 0.5;
		number_one[4].scale_h = 1;
		number_one[5].scale_h = 1.5;
		number_one[6].scale_h = 3;
		number_one[7].scale_h = 4;
	}
	else if (MAGNITUDE > 6.79 && MAGNITUDE < 6.81) {
		number_one[0].scale_c = { 38, 0, 255 };
		number_one[1].scale_c = { 222, 255, 248 };
		number_one[2].scale_c = { 222, 255, 248 };
		number_one[3].scale_c = { 128, 0, 128 };
		number_one[4].scale_c = { 255, 0, 0 };
		number_one[5].scale_c = { 255, 128, 0 };
		number_one[6].scale_c = { 255, 255, 0 };
		number_one[7].scale_c = { 0, 255, 0 };

		number_one[0].scale_h = -1;
		number_one[1].scale_h = -0.1;
		number_one[2].scale_h = 0;
		number_one[3].scale_h = 0.1;
		number_one[4].scale_h = 0.2;
		number_one[5].scale_h = 0.4;
		number_one[6].scale_h = 0.6;
		number_one[7].scale_h = 1;
	}
	else if (MAGNITUDE > 7.19 && MAGNITUDE < 7.21) {
		number_one[0].scale_c = { 38, 0, 255 };
		number_one[1].scale_c = { 222, 255, 248 };
		number_one[2].scale_c = { 222, 255, 248 };
		number_one[3].scale_c = { 128, 0, 128 };
		number_one[4].scale_c = { 255, 0, 0 };
		number_one[5].scale_c = { 255, 128, 0 };
		number_one[6].scale_c = { 255, 255, 0 };
		number_one[7].scale_c = { 0, 255, 0 };

		number_one[0].scale_h = -2;
		number_one[1].scale_h = -0.1;
		number_one[2].scale_h = 0;
		number_one[3].scale_h = 0.2;
		number_one[4].scale_h = 0.4;
		number_one[5].scale_h = 0.8;
		number_one[6].scale_h = 1.4;
		number_one[7].scale_h = 2;
	}
	bath_earth[0].scale_h = 0;
	bath_earth[1].scale_h = 200;
	bath_earth[2].scale_h = 800;
	bath_earth[3].scale_h = 1500;
	bath_earth[4].scale_h = 6000;
	//bath_earth[5].scale_h = 8000;

	bath_earth[0].scale_c = { 0, 91, 65 };
	bath_earth[1].scale_c = { 201, 180, 102 };
	bath_earth[2].scale_c = { 160, 55, 0 };
	bath_earth[3].scale_c = { 121, 83, 83 };
	bath_earth[4].scale_c = { 214, 214, 214 };
	//bath_earth[5].scale_c = { 0, 0, 0 };
}
void fm_calc()
{
	/*paint::pixel_buffer pbuffer;
	rectangle r;
	//r.x = min(a.x, b.x);*/

	nana::threads::pool pool_calc;
	set_delta(variant_delta_t);
	scale_c_h();
	color_ind(size_y);
	using namespace nana;

	nana::form fm{ API::make_center(800, 400) };
	combox out_type(fm);
	out_type.push_back(L"eta");
	out_type.push_back(L"MaxVis");
	out_type.push_back(L"MinVis");
	nana::string out_s_type = L"eta";
	out_type.events().selected([&out_s_type](const arg_combox &ar_cbx){ out_s_type = ar_cbx.widget.caption(); });
	nana::colormap cmap({ { number_one[0].scale_h, number_one[0].scale_c }, { number_one[1].scale_h, number_one[1].scale_c }, { number_one[2].scale_h, number_one[2].scale_c }, { number_one[3].scale_h, number_one[3].scale_c }, { number_one[4].scale_h, number_one[4].scale_c }, { number_one[5].scale_h, number_one[5].scale_c }, { number_one[6].scale_h, number_one[6].scale_c }, { number_one[7].scale_h, number_one[7].scale_c } });

	nana::colormap cmap2({ { bath_earth[0].scale_h, bath_earth[0].scale_c }, { bath_earth[1].scale_h, bath_earth[1].scale_c }, { bath_earth[2].scale_h, bath_earth[2].scale_c }, { bath_earth[3].scale_h, bath_earth[3].scale_c }, { bath_earth[4].scale_h, bath_earth[4].scale_c } });
	nana::picture pic(fm);
	nana::drawing dw1(pic);

	pic.move({ 10, 60 });
	pic.size(nana::size{ (unsigned int)size_y + 200, (unsigned int)size_x });

	dw1.draw([&dw1, &pic, &out_s_type, &cmap, &cmap2](nana::paint::graphics& graph){
		
		nana::paint::graphics gr;
		plot2d pl(&gr);
		pl.colorbar(true);
		pl.region(real_rectangle{ 0.0, 0.0, (double)size_x, (double)size_y });
		pl.region(real_rectangle{ { start_x + delta_x / 2., start_y + delta_y / 2. },
			{ end_x - delta_x / 2., end_y - delta_y / 2. } });
		pl.window({ 0, 0, graph.size().width, graph.size().height });
		pl.axis_x(true);
		pl.axis_y(true);

		pl.axis_x_label(L"E");
		pl.axis_y_label(L"N");

		color_func2d f = [&out_s_type, &cmap, &cmap2](double x, double y)->color{
			nana::color c;
			coord p = get_coord(x, y);
			if (bottom[p.y][p.x] >= 0.0) {
				c = cmap2.get_color(bottom[p.y][p.x]);
				if (bottom[p.y][p.x] < 0) c = { 38, 226, 255 };
			}
			else {
				nana::color mar1(255, 0, 0);
				nana::color mar2(0, 0, 0);

				switch(out_s_type[0])
				{
					case L'e':
						c = cmap.get_color(eta[p.y][p.x]);
						break;
					case L'M':
					{
						if(out_s_type[1] == L'a')
						{
							if (maxVis[p.y][p.x] < 0) c = { 38, 226, 255 };
							else c = cmap.get_color(maxVis[p.y][p.x]);
						}
						else
						{
							c = cmap.get_color(minVis[p.y][p.x]); //c = { 255, 255, 255 };
						}
						break;
					}

				}

				//if (out_s_type == L"eta") c = cmap.get_color(eta[p.y][p.x]);
				//if (out_s_type == L"MaxVis") { 
				//	if (maxVis[p.y][p.x] < 0) c = { 38, 226, 255 };
				//	else c = cmap.get_color(maxVis[p.y][p.x]); 
				//}
				//if (out_s_type == L"MinVis") c = cmap.get_color(minVis[p.y][p.x]); //c = { 255, 255, 255 };
			}
			nana::color mar1(255, 0, 0);
			nana::color mar2(0, 0, 255);
			nana::color mar3(0, 0, 0);
			if (out_s_type == L"MaxVis") mar3 = { 255, 255, 255 };
			double maxDist = double(size_x + size_y) / 20000.;
			double dist = getDistantionOfCenterMareographs(x, y, maxDist);
			if (dist >= maxDist - (maxDist / 3.) && dist <= maxDist) c = mar3;
			else if (dist <= maxDist / 3.) c = mar2;
			else if (dist <= maxDist) c = mar1;
			return c;
		};
				
		pl.plot_2d_color_function(f);
		pl.draw_grid(false);
		pl.draw_axis(true, false);
		//pl.colorbar(cmap, { { number_one[0].scale_h }, { number_one[2].scale_h }, { number_one[3].scale_h }, { number_one[4].scale_h }, { number_one[5].scale_h }, { number_one[6].scale_h }, { number_one[7].scale_h } });
		//pl.colorbar(cmap, { { -5 }, { -4 }, { -3 }, { -2 }, { -1 }, { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 }, { 10 }, { 11 } });
		vector <double> ticks;
		//for (int i = int(number_one[0].scale_h); i <= int(number_one[7].scale_h); i++)
		for (double i = double(number_one[0].scale_h); i <= int(number_one[7].scale_h); i += 0.5){
			ticks.push_back(i);
		}
		//pl.colorbar(cmap, { {-6}, { -5 }, { -4 }, { -3 }, { -2 }, { -1 }, { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 } });
		pl.colorbar(cmap, ticks);
		nana::paint::font ft_1(L"Times New Roman", 15u);
		
		graph = gr;
		/*gr.make(size(size_x + 300, size_y + 20));




		int lal = 1000000 + t;
		std::string lal2 = to_string(lal);
		lal2[0] = '0';
		std::string name = "c:\\tmp\\save_small\\" + lal2 + ".bmp";
		gr.save_as_file(name.c_str());

		*/

		graph.typeface(ft_1);
		nana::string time = get_time();
		graph.string({ 120, 5 }, time, { 0, 0, 0 });
		


		nana::paint::graphics gr1;
		//nana::paint::font ft_2(L"Times New Roman", 28u, true);
		nana::paint::font ft_2(L"Times New Roman", FONT_SIZE, true);
		gr1.typeface(ft_2);
		//gr1.make(size(size_x + 300, size_y + 20));
		int w = WIDTH_MARGIN; int h = HEIGHT_MARGIN;
		gr1.make(nana::size(size_x + w, size_y + h));
		gr1.rectangle(true, { 255, 255, 255 });
		/*switch(EVENT){
		case MEXICO_BIG: 
			
			break;
		default: 
			
			break;
		}*/
		plot2d save(&gr1);
		save.colorbar(true);
		//save.axis_x_label(EVENT == MEXICO_BIG ? L"W" : L"E");
		save.axis_x_label(L"E");
		save.axis_y_label(L"N");
		save.region(real_rectangle{ 0.0, 0.0, (double)size_x, (double)size_y });
		save.region(real_rectangle{ { start_x + delta_x / 2., start_y + delta_y / 2. }, 
			{ end_x - delta_x / 2., end_y - delta_y / 2. } });
		save.window({ 0, 0, unsigned int(size_x + 300), unsigned int(size_y + 20) });
		//save.window({ 0, 0, unsigned int(size_x + 500), unsigned int(size_y + 100) });
		save.axis_x(true); //save.axis_x(true);
		save.axis_y(true); //save.axis_y(true);
		//save.axis_x_label(L"E");
		//save.axis_y_label(L"N");
		save.plot_2d_color_function(f);
		//save.draw_grid(false, true, FONT_SIZE, 0.5, 0.5, 2, 1);
		save.draw_grid(false, true, FONT_SIZE);
		save.draw_axis(true, false);
		//save.colorbar(cmap, { { number_one[0].scale_h }, { number_one[2].scale_h }, { number_one[3].scale_h }, { number_one[4].scale_h }, { number_one[5].scale_h }, { number_one[6].scale_h }, { number_one[7].scale_h } });
		save.colorbar(cmap, ticks);
		//nana::paint::font ft_3(L"Times New Roman", 32u, true);
		gr1.typeface(ft_2);
		//gr1.string({ 120, 5 }, time, { 0, 0, 0 });
		int timeInt = 1000000 + t;
		std::string timeString = to_string(timeInt);
		timeString[0] = '0'; 
		
		nana::point recCenter(EVENT == MEXICO_BIG ? 1140 : EVENT == PACIFIC_CUT ? 70 : 120, EVENT == MEXICO_BIG ? 7 : 5);
		nana::size rect(EVENT == MEXICO_BIG ? 366 : 180, EVENT == MEXICO_BIG ? 110 : 50);
		gr1.rectangle(nana::rectangle{ recCenter, rect }, true, { 255, 255, 255 });
		//gr1.rectangle(rec, nana::color(255, 255, 255));
		std::string namePath = PATH_FRONTS;
		std::string name;
		if (t < time_moments) name = namePath + timeString + ".bmp";
		else {
			name = namePath + "MAX.bmp";
		}
		nana::point textCenter(EVENT == MEXICO_BIG ? 1148 : EVENT == PACIFIC_CUT ? 90 : 140, EVENT == MEXICO_BIG ? 7 : 8);
		gr1.string(textCenter, time, { 0, 0, 0 });
		gr1.save_as_file(name.c_str());
	});

	nana::button bu(fm);
	//bu.move({10, 10, 100, 20});
	bu.caption("Do this!");
	//bu.umake_event<nana::events::click>(nana::threads::pool_push(pool_calc, *this, &example::_m_start));
	//bu.events().click(pool_push(pool_calc, run_calc));
	bu.events().click(pool_push(pool_calc, [&dw1, &out_s_type, &pool_calc]{
		for (; t < time_moments + 1; t++) {
			if (time_moments == 0) { 
				t = t - 1;
				continue;
			}
			clock_t be = clock();
			run_calc();
			clock_t en = clock();
			
			if (!(t % 100)) cout << t << endl; // << ' ' << en - be << endl;
			if (t % output_time == 0) { dw1.update();}
			if (t % 25 == 0) { 
				checking_mareographs(); 				
			}
			if (t % time_moments == 0 && t != 0) saveMareographs();
		}
		out_s_type = L"MaxVis";
		t--;
		dw1.update();
		output_array_max_vis(PATH_MAX, maxVis);
	}));

	place pl(fm);
	pl.div("< vert <weight=25 <bu> > <weight = 5> < weight=20 <cmbx><><><><><> > <pic>>");
	pl.field("bu") << bu;
	pl.field("cmbx") << out_type;
	pl.field("pic") << pic;
	pl.collocate();
	fm.show();
	nana::exec();
}