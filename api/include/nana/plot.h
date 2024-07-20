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
#include <cassert>
#include "geometry.h"
#include <iostream>

using namespace nana;

#define WIDTH_CHAR 7

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
			assert(map_.size());
		};
		colormap(func_colormap<double> func_color_) : func_color(func_color_) {};
		color get_color(double val) { return func_color(val); }
	};


	class plot2d
	{
	private:
		paint::graphics * gr_plot{nullptr};
		rectangle window_;
		real_rectangle region_, region_log_;
		bool axis_x_ { true }, axis_y_ {true};
		bool axis_x_log_ { false }, axis_y_log_ { false };
		double axis_x_base_ = 10.0, axis_y_base_ = 10.0, axis_x_log_base_ = log (axis_x_base_), axis_y_log_base_ = log (axis_y_base_);
		nana::string axis_x_label_, axis_y_label_, label_;
		// for recursive_plot max depth without check on the same point
		int h_check;
	public:
		plot2d() {}
		plot2d (paint::graphics * gr) : gr_plot (gr) {}
		void size (const nana::size pt)
		{
			gr_plot->make (pt);
		}

		void set_graphics (paint::graphics * gr)
		{
			//if (gr_plot != nullptr) delete gr_plot;
			gr_plot = gr;
		}
		void window (rectangle window) { 
			if (gr_plot->empty ()) {
				gr_plot->make ({ window_.width + window_.x, window_.height + window_.y });
				gr_plot->rectangle ({ 0,0, window_.width + window_.x, window_.height + window_.y }, true, { 255, 255, 255 });
			} else gr_plot->rectangle (window, true, { 255, 255, 255 });
			window_ = {window.x + 80, window.y, window.width - 120, window.height - 20};
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

		void draw_axis ();
		void draw_grid();
		void smooth_line(point a, point b, color color);

		void axis_x_label (nana::string label) { axis_x_label_ = label; }
		void axis_y_label (nana::string label) { axis_y_label_ = label; }
		void label (nana::string label) { label_ = label; }

		void line (std::vector <real_point>, nana::color color = {0, 0, 0}, int point_radius = 0);
		void plot_function(func1d & func, double a, double b, nana::color color = { 0,0,255 }, bool good_plot = true);
		void plot_recursive(func1d & f, func1d & g, double lt, double rt, color color, int h);
		void plot_parametric(func1d & f, func1d & g, double lt, double rt, color color);
		void plot_2d_function(func2d & f, colormap & c, real_rectangle region);
		void plot_2d_function(func2d & f, colormap & c);
		void draw_angle_field(func2d & f, func2d & g, colormap & c, real_rectangle region);

		void draw_field(func2d & f, func2d & g, colormap & c, real_rectangle region);

		void draw_integrate_field(func2d & f, func2d & g, colormap & c, real_rectangle region, bool flag_was = false);

		nana::point get_point (const real_point pt);
		real_point get_real_point (const nana::point pt);

		void clear ();
		void draw (nana::paint::graphics & gr);
		void fill_color(point a, color fill_color, color border_color);
		void draw_arrowhead(point a, point b, color color, double h, double w, double corner);
		//void draw_arrowhead(point a, point b, color color);

		void save_as_file()
		{
			static int num = 0;
			//gr_plot->save_as_file((to_string(num) + ".bmp").c_str());
		}
	};
};

#endif