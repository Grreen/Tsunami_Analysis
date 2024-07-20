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
#include <ctime>
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

#define EVENT DEFAULT
#define MAGNITUDE 8

#define FONT_SIZE 28
#define SAVE_FONT_SIZE 28
#define WIDTH_MARGIN EVENT == MEXICO_BIG ? 500 : 300
#define HEIGHT_MARGIN EVENT == MEXICO_BIG ? 100 : 20 

//#define X_LBL EVENT == MEXICO_BIG ? L"W" : L"E" 
using namespace std;
struct scale {
	double scale_h;
	nana::color scale_c;
};

template <typename T> 
void output_array_max_vis(const std::wstring& filePath, T **a)
{
	wfstream file;
	file.open(filePath.c_str(), std::fstream::out);

	if (file.fail())
		return;

	for (int i = 0; i < size_y; i++){
		for (int j = 0; j < size_x; j++) {
			if (a[i][j] >-0.001)
				file << fixed << setprecision(10) << a[i][j] << L"\t";
			else file << fixed << setprecision(7) << a[i][j] << L"\t";
		}
		file << "\n";
	}
}

nana::string get_time(){
	nana::string clock_min, clock_sec, clock_hour, clock_time;
	int time = t - (int(t / 60) * 60);
	clock_sec = to_wstring((time));
	time = int(t / 60) - (int(t / 60 / 60) * 60);
	clock_min = to_wstring((time));
	time = int(t / 60 / 60);
	clock_hour = to_wstring((time));
	if (clock_sec.size() == 1) { clock_sec = L"0" + clock_sec; }
	if (clock_min.size() == 1) { clock_min = L"0" + clock_min; }
	if (clock_hour.size() == 1) { clock_hour = L"0" + clock_hour; }
	clock_time = clock_hour + L":" + clock_min + L":" + clock_sec;
	return clock_time;
}

void fm_calc()
{
	nana::threads::pool pool_calc;
	set_delta(variant_delta_t);
	using namespace nana;

	nana::form fm{ API::make_center(800, 400) };
	combox out_type(fm);
	out_type.push_back(L"eta");
	out_type.push_back(L"MaxVis");
	out_type.push_back(L"MinVis");
	nana::string out_s_type = L"eta";
	out_type.events().selected([&out_s_type](const arg_combox& ar_cbx) { out_s_type = ar_cbx.widget.caption(); });

	nana::colormap heightColorMap(settings.height.mMap);
	nana::colormap frontColorMap(settings.front.mMap);
	nana::colormap maxHeightColorMap(settings.maxHeight.mMap);

	nana::picture pic(fm);
	nana::drawing dw1(pic);

	pMainPicture = &dw1;

	pic.move({ 10, 60 });
	pic.size(nana::size{ (unsigned int)size_y + 200, (unsigned int)size_x });

	dw1.draw([&dw1, &pic, &out_s_type, &heightColorMap, &frontColorMap, &maxHeightColorMap](nana::paint::graphics& graph)
		{
			nana::paint::font oFont(settings.wsFontName.c_str(), settings.GetFontSize(min(pic.size().height, pic.size().width)), true);
			nana::string sTime = get_time();

			nana::paint::graphics gr;

			gr.typeface(oFont);
			gr.make(nana::size(graph.width(), graph.height()));
			gr.rectangle(true, { 255, 255, 255 });

			// Создаем plot для отрисовки в окне
			plot2d pl(&gr);
			pl.colorbar(true);
			pl.region(real_rectangle{ { start_x + delta_x / 2., start_y + delta_y / 2. }, { end_x - delta_x / 2., end_y - delta_y / 2. } });

			pl.window({ 0, 0, (unsigned int)graph.width(), (unsigned int)graph.height() });
			pl.axis_x(true);
			pl.axis_y(true);

			pl.axis_x_label(L"W", L"E");
			pl.axis_y_label(L"N", L"S");

			// Лямбда-выражение, которое задает то, как мы будем рисовать нашу акваторию
			color_func2d f = [&out_s_type, &heightColorMap, &frontColorMap, &maxHeightColorMap](double x, double y)->color
			{
				nana::color c;
				coord p = get_coord(x, y);

				if (bottom[p.y][p.x] >= 0.0)
				{
					c = heightColorMap.get_color(bottom[p.y][p.x]);
				}
				else
				{
					switch (out_s_type[0])
					{
					case L'e':
						if (0 != t)
							c = frontColorMap.get_color(eta[p.y][p.x]);
						else
							c = heightColorMap.get_color(bottom[p.y][p.x]);
						break;
					case L'M':
					{
						if (out_s_type[1] == L'a')
							c = maxHeightColorMap.get_color(maxVis[p.y][p.x]);
						else
							c = maxHeightColorMap.get_color(minVis[p.y][p.x]);
						break;
					}
					}
				}

				return c;
			};

			pl.plot_2d_color_function(f);

			if (t != 0 && t <= time_moments)
			{
				pl.colorbar(frontColorMap, settings.front.wsUnitMeasure, settings.front.GetTick());
				pl.draw_time(sTime);

				if (settings.bDrawDirectionWave)
					pl.draw_direction_wave(0.25);
			}
			else if (t == 0)
			{
				pl.colorbar(heightColorMap, L"km", settings.height.GetTick());
				if (!oBricks.Empty())
				{
					srand(time(0));
					vector<real_point> arPoints;
					for (const brick_data& oBrick : oBricks.brick_calc)
					{
						for (const brick_point& oPoint : oBrick.points)
							arPoints.push_back({ oPoint.x, oPoint.y });

						arPoints.push_back({ oBrick.points.front().x, oBrick.points.front().y });

						pl.fill(arPoints, oBrick.oColor);
						arPoints.clear();
					}
				}
			}
			else
				pl.colorbar(maxHeightColorMap, settings.maxHeight.wsUnitMeasure, settings.maxHeight.GetTick());

			const std::vector<real_point> arCoordsMareograms = GetCoordsMareographs();

			pl.draw_mareograms(arCoordsMareograms, true);
			pl.draw_grid(settings.bDrawGrid, true);
			pl.draw_axis(true, false);

			graph = gr;

			const double dScale = settings.uMaxSize / max((end_x - start_x), (end_y - start_y));
			nana::size oImageSize{ (unsigned)((end_x - start_x) * dScale), (unsigned)((end_y - start_y) * dScale) };

			nana::paint::graphics gr1;
			nana::paint::font fontSave(settings.wsFontName.c_str(), settings.GetFontSize(oImageSize.height), true);
			gr1.typeface(fontSave);
			gr1.make(oImageSize);
			gr1.rectangle(true, { 255, 255, 255 });

			// Создаем новый plot для сохранения в файл
			plot2d save(&gr1);
			save.colorbar(true);

			save.axis_x_label(L"W", L"E");
			save.axis_y_label(L"N", L"S");

			save.region(real_rectangle{ 0.0, 0.0, (double)oImageSize.width, (double)oImageSize.height });
			save.region(real_rectangle{ { start_x + delta_x / 2., start_y + delta_y / 2. },
			                            { end_x - delta_x / 2.,   end_y - delta_y / 2. } });
			save.window({ 0, 0, (unsigned int)oImageSize.width, (unsigned int)oImageSize.height });
			save.axis_x(true);
			save.axis_y(true);
			save.plot_2d_color_function(f);
			save.draw_mareograms(arCoordsMareograms, true);

			if (t != 0 && t <= time_moments)
			{
				save.colorbar(frontColorMap, settings.front.wsUnitMeasure, settings.front.GetTick());
				save.draw_time(sTime);

				if (settings.bDrawDirectionWave)
					save.draw_direction_wave(1);
			}
			else if (t == 0)
			{
				save.colorbar(heightColorMap, L"km", settings.height.GetTick());
				if (!oBricks.Empty())
				{
					srand(time(0));
					vector<real_point> arPoints;
					for (const brick_data& oBrick : oBricks.brick_calc)
					{
						for (const brick_point& oPoint : oBrick.points)
							arPoints.push_back({ oPoint.x, oPoint.y });

						arPoints.push_back({ oBrick.points.front().x, oBrick.points.front().y });

						save.fill(arPoints, oBrick.oColor);
						arPoints.clear();
					}
				}
			}
			else
				save.colorbar(maxHeightColorMap, settings.maxHeight.wsUnitMeasure, settings.maxHeight.GetTick());

			save.draw_grid(settings.bDrawGrid, settings.bUseSteps, settings.dStepX, settings.dStepY, settings.dStepFontX, settings.dStepFontY);
			save.draw_axis(true, false);

			std::string namePath = std::string(settings.wsPathToSave.begin(), settings.wsPathToSave.end()) + "fronts/";
			std::string name;

			if (t <= time_moments)
			{
				int timeInt = 1000000 + t;
				std::string timeString = to_string(timeInt);
				timeString[0] = '0';
				name = namePath + timeString + ".bmp";
			}
			else
			{
				if (!settings.bSaveMaxHeight)
					return;

				name = namePath + "MAX.bmp";
			}
			gr1.save_as_file(name.c_str());
		});

	nana::button bu(fm);

	bu.caption("Start calculation!");
	bu.events().click(pool_push(pool_calc, [&dw1, &out_s_type, &pool_calc]
		{
			for (; t <= time_moments; t++) {
				clock_t be = clock();
				run_calc();
				clock_t en = clock();
				if (!(t % 100)) cout << t << endl;
				if (t % output_time == 0) { dw1.update(); }
				if (settings.bSaveMaregrams && t % 25 == 0) 
				{
					checking_mareographs();
				}
				if (settings.bSaveMaregrams && t % time_moments == 0 && t != 0)
					saveMareographs();
			}
			out_s_type = L"MaxVis";
			++t;
			dw1.update();

			if (settings.bSaveMaxHeight)
				output_array_max_vis(settings.wsPathToSave + L"maxHeight/MaxHeight.mtx", maxVis);

			out_s_type = L"eta";
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