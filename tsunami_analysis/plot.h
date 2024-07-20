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

#define EVENT_PLOT PACIFIC_CUT
#define LEFT_MARGIN EVENT_PLOT == MEXICO_BIG ? 100 : 0
//#define WIDTH_MARGIN EVENT == MEXICO_BIG ? 100 : 0


typedef function <color(double, double)> color_func2d;

namespace nana {
	
	color operator * (const color &x, double y);

	color operator + (const color& x, const color& y);

	template <typename T> using func_colormap = function <color(T val)>;

	class colormap {
	private:
		map <double, color> map_color;
		//bool interpolation;
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
		colormap(map <double, color> map_, bool interpolation_ = true) : map_color(map_)//, interpolation(interpolation_)
		{
			assert(map_.size());
		};
		colormap(func_colormap<double> func_color_) : func_color(std::move(func_color_)) {};
		color get_color(double val) const
		{ return func_color(val); }
		double min() const
		{ return map_color.begin()->first; }
		double max() const
		{ return map_color.rbegin()->first; }
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
		nana::string axis_x_label_, axis_y_label_, label_;
		bool colorbar_{ false };
		paint::pixel_buffer pbuffer;
		bool flag_pbuffer = false;
		// for recursive_plot max depth without check on the same point
		int h_check = 0;
	public:
		plot2d() = default;
		plot2d (paint::graphics * gr) : gr_plot (gr) {}
		void size (const nana::size& pt) const
		{
			gr_plot->make (pt);
		}

		void set_graphics (paint::graphics * gr)
		{
			//if (gr_plot != nullptr) delete gr_plot;
			gr_plot = gr;
		
		}

		void window (const rectangle& window) {
			window_ = window;
			if (gr_plot->empty ()) {
				gr_plot->make({ window_.width + window_.x, window_.height + window_.y });
				gr_plot->rectangle ({ 0,0, window_.width + window_.x, window_.height + window_.y }, true, { 255, 255, 255 });
			} //else gr_plot->rectangle (window, true, { 255, 255, 255 });
			window_a = window;
			if (colorbar_) window_ = { window.x + 120 + (EVENT_PLOT == MEXICO_BIG ? 20 : PACIFIC_CUT ? -60 : 0), window.y, window.width - 220, window.height - 65 };
			else window_ = {window.x + 85, window.y, window.width - 150, window.height - 40};
		}
		rectangle window () const
		{ return window_; }

		void region (const real_rectangle& region) { 
			region_ = region; 
			axis_x_log (axis_x_log_, axis_x_base_);
			axis_y_log (axis_y_log_, axis_y_base_);
		}
		real_rectangle region () const
		{ return region_; }

		bool axis_x () const
		{ return axis_x_; }
		void axis_x (bool draw) { axis_x_ = draw; }

		bool axis_y () const
		{ return axis_y_; }
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

		void draw_axis(bool axis = true, bool arrow = true) const;
		void draw_grid(bool grid = true, bool ticks = true, unsigned int font_size = 20,
			double step_x = 0, double step_y = 0, double step_font_x = 0, double step_font_y = 0);

		
		void smooth_line(point a, point b, color color);

		void axis_x_label (nana::string label) { axis_x_label_ = std::move( label); }
		void axis_y_label (nana::string label) { axis_y_label_ = std::move(label);}
		void label (nana::string label) { label_ = std::move(label);}

		void line (const std::vector<real_point>&, const nana::color& color = {0, 0, 0}, int point_radius = 0) const;
		void integral_line(std::vector <real_point>, nana::color color = { 0, 0, 0 }, nana::color color_area = { 255, 255, 255 }, int point_radius = 0);

		void plot_function(func1d & func, double a, double b, const nana::color& color = {0, 0, 255}, bool good_plot = true);
		void plot_recursive(func1d & f, func1d & g, double lt, double rt, const color& color, int h);
		void plot_parametric(func1d & f, func1d & g, double lt, double rt, const color& color);
		void plot_2d_function(func2d & f, colormap & c, const real_rectangle& region) const;
		void plot_2d_function(func2d & f, colormap & c);
		void plot_2d_color_function(color_func2d & f, const real_rectangle& region) const;
		void plot_2d_color_function(color_func2d & f) const;

		void draw_angle_field(const func2d & f, const func2d & g, const colormap & c, const real_rectangle& region) const;

		void draw_field(func2d & f, func2d & g, colormap & c, real_rectangle region);

		void draw_realization(func2d & f, func2d & g, colormap & c, real_point pt, double dt, int nt);

		//void draw_integrate_field(func2d & f, func2d & g, colormap & c, real_rectangle region = {}, bool flag_was = false);
		void plot2d::draw_integrate_field(func2d & f, func2d & g, colormap & c, real_rectangle region = {}, bool flag_was = true);

		nana::point get_point (const real_point& pt) const;
		real_point get_real_point (const nana::point& pt) const;

		void clear ();
		void draw (nana::paint::graphics & gr);
		void fill_color(const point& a, const color& fill_color, const color& border_color) const;
		void draw_arrowhead(const point& a, const point& b, const color& color, double h = 15, double w = 5, double corner = 12) const;
		void colorbar(const colormap& cm, const vector<double>& x) const;
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