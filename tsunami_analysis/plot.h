#ifndef PLOT_H
#define PLOT_H

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <vector>
#include <set>
#include <map>
#include <cassert>
#include "geometry.h"
#include <iostream>
#include "calculation.h"

using namespace nana;

#define WIDTH_CHAR 12

#define EVENT_PLOT DEFAULT
#define LEFT_MARGIN EVENT_PLOT == MEXICO_BIG ? 80 : 0
//#define WIDTH_MARGIN EVENT == MEXICO_BIG ? 100 : 0


typedef function <color(double, double)> color_func2d;

namespace nana {
	
	color operator * (color x, double y);

	color operator + (color x, color y);

	template <typename T> using func_colormap = function <color(T val)>;

	class colormap {
	private:
		map <double, color> map_color;
		bool interpolation;
		func_colormap<double> func_color{ ([this](double val) -> color { // linear interpolation
			auto it = map_color.lower_bound(val);
			if (it == map_color.end()) {
				return prev(it)->second;
			}
			else if (it == map_color.begin()) {
				return it->second;
			}
			else {
				double al = ((val - prev(it)->first) / (it->first - prev(it)->first));
				return (prev(it)->second) * (1.0 - al) + (it->second) * al;
			}
		}) };
	public:
		colormap(map <double, color> map_, bool interpolation_ = true) : map_color(map_), interpolation(interpolation_)
		{
		};
		colormap(func_colormap<double> func_color_) : func_color(func_color_) {};
		void set_colormap(const map <double, color>& colormap)
		{
			map_color = colormap;
		}
		size_t get_size() const
		{
			return map_color.size();
		}
		std::vector<color> get_colors() const
		{
			std::vector<color> arColors;

			for (const std::pair<double, color> oValue : map_color)
				arColors.push_back(oValue.second);

			return arColors;
		}
		std::vector<double> get_ticks(double dStep = -1) const
		{
			std::vector<double> ticks;

			if (dStep <= 0)
			{
				for (const std::pair<double, color>& value : map_color)
					ticks.push_back(value.first);
			}
			else
			{
				for (double value = min(); value <= max(); value += dStep)
					ticks.push_back(value);
			}

			return ticks;
		}
		color get_color(double val) const { return func_color(val); }
		double min() const { return map_color.begin()->first; }
		double max() const { return map_color.rbegin()->first; }
	};
	/*
	template <class T1, class T2>
	class rectangle_map {
		T1 & r1;
		T2 & r2;
	public:
		rectangle_map(T1 & region_1, T2 region_2) : r1(region_1), r2(region_2) {}
		T2::point transform_1_to_2(const T1::point & pt)
		{
			T2::point ans;
			ans.x = (int)(r2.x + r2.width * (pt.x - r1.x) / r1.width);
			ans.y = (int)(r2.y + r2.height - r2.height * (pt.y - r1.y) / r1.height);
			return ans;
		}
		T1::point transform_2_to_1(const T2::point & pt)
		{
			T1::point ans;
			ans.x = r1.width * ((double)pt.x - r2.x) / r2.width + r1.x;
			ans.y = r1.height * ((double)r2.y + r2.height - pt.y) / r2.height + r1.y;
			return ans;
		}
	};*/

	class plot2d
	{
	private:
		paint::graphics * gr_plot{nullptr};
		rectangle window_, window_a;
		real_rectangle region_, region_log_;
		bool axis_x_ { true }, axis_y_ {true};
		bool axis_x_log_ { false }, axis_y_log_ { false };
		double axis_x_base_ = 10.0, axis_y_base_ = 10.0, axis_x_log_base_ = log (axis_x_base_), axis_y_log_base_ = log (axis_y_base_);
		nana::string axix_x_label_start, axix_x_label_end;
		nana::string axix_y_label_start, axix_y_label_end;
		bool colorbar_{ false };
		paint::pixel_buffer pbuffer;
		bool flag_pbuffer = false;
		// for recursive_plot max depth without check on the same point
		int h_check;
	public:
		plot2d() {}
		plot2d (paint::graphics * gr) : gr_plot (gr) {}
		void size (const nana::size& pt)
		{
			gr_plot->make (pt);
		}

		void set_graphics (paint::graphics * gr)
		{
			//if (gr_plot != nullptr) delete gr_plot;
			gr_plot = gr;
		
		}

		void window (const rectangle& window) 
		{
			window_ = window;
			if (gr_plot->empty()) 
			{
				gr_plot->make({ window_.width + window_.x, window_.height + window_.y });
				gr_plot->rectangle({ 0,0, window_.width + window_.x, window_.height + window_.y }, true, { 255, 255, 255 });
			} //else gr_plot->rectangle (window, true, { 255, 255, 255 });
			window_a = window;

			window_.x += gr_plot->typeface().size() * (axix_x_label_start.length() + 3);
			window_.y += gr_plot->typeface().size();
			window_.height -= gr_plot->typeface().size() * 2.5;

			if (colorbar_)
				window_.width -= gr_plot->typeface().size() * (7.5 + axix_x_label_start.length());
			else
				window_.width -= gr_plot->typeface().size() * 5;

			return;
			if (colorbar_) window_ = { window.x + 120 + (EVENT_PLOT == MEXICO_BIG ? 20 : PACIFIC_CUT ? -60 : 0), window.y, window.width - 220, window.height - 65 };
			else window_ = { window.x + 85, window.y, window.width - 150, window.height - 40 };
		}
		rectangle window () { return window_; }

		void region (real_rectangle region) { 
			region_ = region; 
			axis_x_log (axis_x_log_, axis_x_base_);
			axis_y_log (axis_y_log_, axis_y_base_);
		}
		real_rectangle region () { return region_; }

		bool axis_x () { return axis_x_; }
		void axis_x (bool draw) { axis_x_ = draw; }

		bool axis_y () { return axis_y_; }
		void axis_y (bool draw) { axis_y_ = draw; }
		void colorbar(bool draw) { colorbar_ = draw; }
		//real_point get_step(real_rectangle region) {}

		void axis_x_log (bool logarithmic, double base = 2.0) { 
			if (logarithmic) {
				axis_x_base_ = base;
				axis_x_log_base_ = log (base);
				region_log_.x = log (region_.x) / axis_x_log_base_;
				region_log_.width = log (region_.x + region_.width) / axis_x_log_base_ - region_log_.x;
			}
			axis_x_log_ = logarithmic;
		}
		void axis_y_log (bool logarithmic, double base = 2.0)
		{
			if (logarithmic) {
				axis_y_base_ = base;
				axis_y_log_base_ = log (base);
				region_log_.y = log (region_.y) / axis_y_log_base_;
				region_log_.height = log (region_.y + region_.height) / axis_y_log_base_ - region_log_.y;
			}
			axis_y_log_ = logarithmic;
		}

		void draw_direction_wave(float dScale);

		void draw_axis(bool axis = true, bool arrow = true);
		void draw_grid(bool grid = true, bool ticks = true, double step_x = 0, double step_y = 0, double step_font_x = 0, double step_font_y = 0, double height = -1);
		void draw_time(const nana::string& time);
		void draw_mareograms(const std::vector<real_point>& arCoords, bool drawIndex = false);

		void smooth_line(const point& a, const point& b, const color& color);

		void axis_x_label(const nana::string& start_label, const nana::string& end_label);
		void axis_y_label(const nana::string& start_label, const nana::string& end_label);

		void fill (const std::vector <real_point>&, const nana::color& color = {0, 0, 0});
		void integral_line(const std::vector <real_point>&, const nana::color& color = { 0, 0, 0 }, const nana::color& color_area = { 255, 255, 255 }, int point_radius = 0);

		void plot_function(func1d & func, double a, double b, nana::color color = { 0,0,255 }, bool good_plot = true);
		void plot_recursive(func1d & f, func1d & g, double lt, double rt, color color, int h);
		void plot_parametric(func1d & f, func1d & g, double lt, double rt, color color);
		void plot_2d_function(func2d & f, colormap & c, real_rectangle region);
		void plot_2d_function(func2d & f, colormap & c);
		void plot_2d_color_function(color_func2d & f, real_rectangle region);
		void plot_2d_color_function(color_func2d & f);

		void draw_angle_field(func2d & f, func2d & g, colormap & c, real_rectangle region);

		void draw_field(func2d & f, func2d & g, colormap & c, real_rectangle region);

		void draw_realization(func2d & f, func2d & g, colormap & c, real_point pt, double dt, int nt);

		//void draw_integrate_field(func2d & f, func2d & g, colormap & c, real_rectangle region = {}, bool flag_was = false);
		void plot2d::draw_integrate_field(func2d & f, func2d & g, colormap & c, real_rectangle region = {}, bool flag_was = true);

		nana::point get_point (const real_point& pt);
		real_point get_real_point (const nana::point& pt);

		void clear ();
		void draw (nana::paint::graphics & gr);
		void fill_color(const point& a, const color& fill_color, const color& border_color);
		void draw_arrowhead(const point& a, const point& b, const color& color, double h = 15, double w = 5, double corner = 12);
		void colorbar(const colormap& cm, const nana::string& wsUnitMeasure, double dTick = 0);
		real_point get_step();
		//void draw_arrowhead(point a, point b, color color);

		void save_as_file()
		{
			static int num = 0;
			//gr_plot->save_as_file((to_string(num) + ".bmp").c_str());
		}
	};
};

#endif