	#include "plot.h"

	namespace nana 
	{
		#define MAX_MAG_WAVE 1.5
		#define MIN_MAG_WAVE 0.25

		nana::string ConvertToString(double value)
		{
			std::wostringstream stream;
			stream << std::fixed << std::setprecision(2) << value;
			nana::string result = stream.str();
			result.erase(result.find_last_not_of('0') + 1, std::string::npos);
			result.erase(result.find_last_not_of('.') + 1, std::string::npos);
			return result;
		}

		color operator * (color x, double y)
		{
			return color{ (unsigned int)(x.r() * y), (unsigned int)(x.g() * y), (unsigned int)(x.b() * y) };
		}

		color operator + (color x, color y)
		{
			return color{ (unsigned int)(x.r() + y.r()), (unsigned int)(x.g() + y.g()), (unsigned int)(x.b() + y.b()) };
		}

		nana::point plot2d::get_point(const real_point& pt)
		{
			point ans;
			if (axis_x_log_) ans.x = (int)(window_.x + window_.width * (log(pt.x) / axis_x_log_base_ - region_log_.x) / region_log_.width);
			else ans.x = (int)(window_.x + window_.width * (pt.x - region_.x) / region_.width);
			if (axis_y_log_) ans.y = (int)(window_.y + window_.height - window_.height * (log(pt.y) / axis_y_log_base_ - region_log_.y) / region_log_.height);
			else ans.y = (int)(window_.y + window_.height - window_.height * (pt.y - region_.y) / region_.height);
			return ans;
		}

		real_point plot2d::get_real_point(const nana::point& pt)
		{
			real_point ans;
			if (axis_x_log_) ans.x = exp((region_log_.width * ((double)pt.x - window_.x) / window_.width + region_log_.x) * axis_x_log_base_);
			else ans.x = region_.width * ((double)pt.x - window_.x) / window_.width + region_.x;

			if (axis_y_log_) ans.y = exp((region_log_.height * ((double)window_.y + window_.height - pt.y) / window_.height + region_log_.y) * axis_y_log_base_);
			else ans.y = region_.height * ((double)window_.y + window_.height - pt.y) / window_.height + region_.y;
			return ans;
		}

		bool isInside(const vector<real_point>& polygon, double x, double y)
		{
			int intersections = 0;

			for (int i = 0; i < polygon.size(); i++)
			{

				int j = (i + 1) % polygon.size();

				// проверка пересечения отрезка с лучом 
				if (((polygon[i].y > y) != (polygon[j].y > y)) &&
					(x < (polygon[j].x - polygon[i].x) * (y - polygon[i].y) /
						(polygon[j].y - polygon[i].y) + polygon[i].x))
				{
					intersections++;
				}
			}

			return intersections % 2 == 1;
		}

		void plot2d::fill(const std::vector <real_point>& points, const nana::color& color)
		{
			if (points.size() < 3)
				return;

			double minX = DBL_MAX;
			double maxX = -DBL_MAX;
			double minY = DBL_MAX;
			double maxY = -DBL_MAX;

			for (const real_point& oCoord : points)
			{
				minX = std::min(oCoord.x, minX);
				maxX = std::max(oCoord.x, maxX);
				minY = std::min(oCoord.y, minY);
				maxY = std::max(oCoord.y, maxY);
			}

			nana::point point_1 = get_point({ (double)minX, (double)maxY });
			nana::point point_2 = get_point({ (double)maxX, (double)minY });

			for (int x = point_1.x; x <= point_2.x; ++x)
				for (int y = point_1.y; y <= point_2.y; ++y)
					gr_plot->line({ x, y }, { x, y }, color);
		}

		//inline bool plot2d::check_bounds(const point & pt, const rectangle & window) { return (pt.x >= 0 && pt.y >= 0 && pt.x <= window.width && pt.y <= window.height); }
		/*
		void plot2d::smooth_line(point a, point b, color color, paint::pixel_buffer pbuffer)
		{
			int x1 = a.x, x2 = b.x, y1 = a.y, y2 = b.y;
			if (x1 > x2) {
				std::swap(x1, x2);
				std::swap(y1, y2);
			}
			int dx = x2 - x1;
			int dy = y2 - y1;
			int c = -x1 * y2 + x2 * y1;
			double l = 1.0 / sqrt(dx * dx + dy * dy);
			int i = x1, j = y1;
			bool fx = i != x2, fy = j != y2;
			while (fx | fy) {
				if (vect(dx, dy, i + 1 - x1, j + 1 - y1) < 0) {
					j++;
				}
				else if (vect(dx, dy, i + 1 - x1, j - y1) < 0) {
					i++;
				}
				else {
					j--;
				}
				double d = (dy * (i + 0.5) - dx * (j + 0.5) + c) * l;
				if (d < 2.0) {
					color.alpha(1.0 - d * 0.5);
					//gr_plot->set_pixel(i, j, color);
					// (pbuffer[j] + i)->value = color.argb().value;
				}
				if (i == x2) fx = false;
				if (j == y2) fy = false;
			}
			color.alpha(1.0);
			(pbuffer[y1] + x1)->value = color.argb().value;
			(pbuffer[y1] + x1)->value = color.argb().value;
			//gr_plot->set_pixel(x1, y1, color);
			//gr_plot->set_pixel(x2, y2, color);
		}*/

		void plot2d::smooth_line(const point& a, const point& b, const color& color)
		{
			int x1 = a.x, x2 = b.x, y1 = a.y, y2 = b.y;
			if (x1 > x2) {
				std::swap(x1, x2);
				std::swap(y1, y2);
			}

			int dx = x2 - x1;
			int dy = y2 - y1;
			int c = -x1 * y2 + x2 * y1;
			double l = 1.0 / sqrt(dx * dx + dy * dy);
			int i = x1, j = y1;
			bool fx = i != x2, fy = j != y2;

			nana::color oCurrentColor(color);

			while (fx | fy) 
			{
				if (vect(dx, dy, i + 1 - x1, j + 1 - y1) < 0) {
					j++;
				}
				else if (vect(dx, dy, i + 1 - x1, j - y1) < 0) {
					i++;
				}
				else {
					j--;
				}
				double d = (dy * (i + 0.5) - dx * (j + 0.5) + c) * l;
				if (d < 2.0) 
				{
					oCurrentColor.alpha(1.0 - d * 0.5);
					gr_plot->set_pixel(i, j, color);
				}
				if (i == x2) fx = false;
				if (j == y2) fy = false;
			}
			oCurrentColor.alpha(1.0);
			gr_plot->set_pixel(x1, y1, color);
			gr_plot->set_pixel(x2, y2, color);
		}

		void plot2d::axis_x_label(const nana::string& start_label, const nana::string& end_label)
		{
			axix_x_label_start = start_label;
			axix_x_label_end   = end_label;
		}

		void plot2d::axis_y_label(const nana::string& start_label, const nana::string& end_label)
		{
			axix_y_label_start = start_label;
			axix_y_label_end = end_label;
		}

		void plot2d::plot_function(func1d & func, double a, double b, nana::color color, bool good_plot)
		{
			if (good_plot) {
				func1d g = [](double x) -> double { return x; };
				h_check = 20;
				plot_recursive(g, func, a, b, color, 20);
			}
			else {
				// plot point by point connnect with line
				a = std::max(a, region_.x);
				b = std::min(b, region_.x + region_.width);
				point ga = get_point({ a, region_.y });
				point gb = get_point({ b, region_.y });
				real_point prev = { a, region_.y };
				prev.y = func(prev.x);
				for (int i = ga.x + 1; i <= gb.x; i++) {
					real_point cur(get_real_point({ i, ga.y }));
					cur.y = func(cur.x);
					//gr_plot->line(get_point(prev), get_point(cur), color);
					smooth_line(get_point(prev), get_point(cur), color);

					prev = cur;
				}
			}
		}

		int xt[5] = { -1, 0, 1, 0, 0 };
		int yt[5] = { 0, -1, 0, 1, 0 };
		double al[5] = { 0.2,0.2,0.2,0.2,1.0 };

		void plot2d::plot_recursive(func1d &f, func1d &g, double lt, double rt, color color, int h)
		{
			point pl = get_point(real_point{ f(lt), g(lt) });
			point pr = get_point(real_point{ f(rt), g(rt) });
			//color.alpha(0.0);
			//color.from_rgba (255,0,0);
			//color = { 255, 0, 0, 1.0 };
			double mt = (lt + rt) / 2.;
			point pm = get_point(real_point{ f(mt), g(mt) });
			//if (pl.x <= (int)window_.width + window_.x && pl.x >= window_.x && pl.y <= (int)window_.height + window_.y && pl.y >= window_.y) 
			//gr_plot->set_pixel(pl.x, pl.y, color);
			//if (pr.x <= (int)window_.width + window_.x && pr.x >= window_.x && pr.y <= (int)window_.height + window_.y && pr.y >= window_.y) 
			//gr_plot->set_pixel(pr.x, pr.y, color);


			if (h < h_check) {
				if ((pl.x == pm.x && pl.y == pm.y) || (pr.x == pm.x && pr.y == pm.y)) {
					int lm = abs(pm.x - pl.x) + abs(pm.y - pl.y), mr = abs(pm.x - pr.x) + abs(pm.y - pr.y);
					if (lm > 2 && mr < 2) {
						//draw_arrowhead(pm, pr, color * 0.5);
						gr_plot->round_rectangle({ pr.x - 1, pr.y - 2, 5, 5 }, 1, 1, color, false, color);
						gr_plot->round_rectangle({ pl.x - 1, pl.y - 2, 5, 5 }, 1, 1, color, false, color);
					}
					if (lm < 2 && mr > 2) {
						//draw_arrowhead(pl, pm, color * 0.5);
						gr_plot->round_rectangle({ pl.x - 1, pl.y - 2, 5, 5 }, 1, 1, color, false, color);
						gr_plot->round_rectangle({ pr.x - 1, pr.y - 2, 5, 5 }, 1, 1, color, false, color);
					
					}
					return;
				}
			}
			for (int i = 0; i < 5; i++) {
				gr_plot->blend(rectangle{ pm.x + xt[i], pm.y + yt[i], 1, 1 }, color, al[i]);
				//gr_plot->blend(rectangle{ pl.x + xt[i], pl.y + yt[i], 1, 1 }, color, 0.2);
			}
			if (h == 0) {
				int lm = abs(pm.x - pl.x) + abs(pm.y - pl.y), mr = abs(pm.x - pr.x) + abs(pm.y - pr.y);
				if (lm > 2 && mr < 2) {
					//draw_arrowhead(pm, pr, color * 0.5);
					gr_plot->round_rectangle({ pr.x - 1, pr.y - 2, 5, 5 }, 1, 1, color, false, color);
					gr_plot->round_rectangle({ pl.x - 1, pl.y - 2, 5, 5 }, 1, 1, color, false, color);
				}
				if (lm < 2 && mr > 2) {
					//draw_arrowhead(pl, pm, color * 0.5);
					gr_plot->round_rectangle({ pl.x - 1, pl.y - 2, 5, 5 }, 1, 1, color, false, color);
					gr_plot->round_rectangle({ pr.x - 1, pr.y - 2, 5, 5 }, 1, 1, color, false, color);
				}
				return;
			}
			plot_recursive(f, g, lt, mt, color, h - 1);
			plot_recursive(f, g, mt, rt, color, h - 1);
		}

		void plot2d::plot_parametric(func1d & f, func1d & g, double lt, double rt, color color)
		{
			h_check = 10;
			plot_recursive(f, g, lt, rt, color, 16);
		}

		void plot2d::plot_2d_function(func2d & f, colormap & c, real_rectangle region)
		{
			clock_t be = clock();
			auto sz = gr_plot->size();
			point a = get_point(real_point{ region.x, region.y });
			point b = get_point(real_point{ region.x + region.width, region.y + region.height });
			paint::pixel_buffer pbuffer;
			rectangle r;
			r.x = min (a.x, b.x);
			r.y = min (a.y, b.y);
			r.width = abs (b.x - a.x);
			r.height = abs (b.y - a.y);
			pbuffer.attach(gr_plot->handle(), r);
		
			for (int x = 0; x <= r.width; x++) {
				for (int y = 0; y <= r.height; y++) {
					auto pt = get_real_point({x, y});
					//gr_plot->set_pixel(x, y, c.get_color(f(pt.x, pt.y)));
					color col = c.get_color(f(pt.x, pt.y));
					pixel_color_t* cur_color = pbuffer[y] + x;
					cur_color->value = col.argb().value;
				}
			}
			pbuffer.close();
			cerr << clock() - be << endl;
		}
	
		void plot2d::plot_2d_function(func2d & f, colormap & c)
		{
			plot_2d_function(f, c, region_);
		}
		real_point plot2d::get_step(){
			auto sz = gr_plot->size();
			rectangle r;
			real_rectangle region = region_;
			point a = get_point(real_point{ region.x, region.y });
			point b = get_point(real_point{ region.x + region.width, region.y + region.height });
		
			r.x = min(a.x, b.x);
			r.y = min(a.y, b.y);
			r.width = abs(b.x - a.x);
			r.height = abs(b.y - a.y);
			auto pt = get_real_point({ 0 + a.x, 0 + b.y });
			auto pt1 = get_real_point({ 1 + a.x, 1 + b.y });
			//pt.x = pt1.x - pt.x;
			//pt.y = pt1.y - pt.y;
			return {pt1.x - pt.x, pt1.y - pt.y };
		}

		void plot2d::plot_2d_color_function(color_func2d & f, real_rectangle region)
		{
			clock_t be = clock();
			auto sz = gr_plot->size();
			point a = get_point(real_point{ region.x, region.y });
			point b = get_point(real_point{ region.x + region.width, region.y + region.height });
			paint::pixel_buffer pbuffer;
			rectangle r;
			r.x = min(a.x, b.x);
			r.y = min(a.y, b.y);
			r.width = abs(b.x - a.x);
			r.height = abs(b.y - a.y);
			pbuffer.attach(gr_plot->handle(), r);

			for (int x = 0; x < r.width; x++) {
				for (int y = 0; y < r.height; y++) {
					auto pt = get_real_point({ x + a.x, y + b.y });
					color col = f(pt.x, pt.y);
					pixel_color_t* cur_color = pbuffer[y] + x;
					cur_color->value = col.argb().value;
				}
			}
			pbuffer.close();
			//cerr << clock() - be << endl;
		}

		void plot2d::plot_2d_color_function(color_func2d & f)
		{
			plot_2d_color_function(f, region_);
		}

		/*void plot2d::plot_2d_function(func2i & f, colormap & c, rectangle region)
		{
			clock_t be = clock();
			auto sz = gr_plot->size();
			point a = get_point(real_point{ region.x, region.y });
			point b = get_point(real_point{ region.x + region.width, region.y + region.height });
			paint::pixel_buffer pbuffer;
			rectangle r;
			r.x = min(a.x, b.x);
			r.y = min(a.y, b.y);
			r.width = abs(b.x - a.x);
			r.height = abs(b.y - a.y);
			pbuffer.attach(gr_plot->handle(), r);

			for (int x = region.x; x <= region.width; x++) {
				for (int y = region.y; y <= region.height; y++) {
					auto pt = get_real_point({ x, y });
					//gr_plot->set_pixel(x, y, c.get_color(f(pt.x, pt.y)));
					color col = c.get_color(f(pt.x, pt.y));
					pixel_color_t* cur_color = pbuffer[y] + x;
					cur_color->value = col.argb().value;
				}
			}
			pbuffer.close();
			cerr << clock() - be << endl;
		}*/


		void plot2d::draw_angle_field(func2d & f, func2d & g, colormap & c, real_rectangle region)
		{
		
			auto sz = gr_plot->size();
			point a = get_point(real_point{ region.x, region.y });
			point b = get_point(real_point{ region.x + region.width, region.y + region.height });
			for (int x = min(a.x, b.x); x <= max(b.x, a.x); x++) {
				for (int y = min(a.y, b.y); y <= max(a.y, b.y); y++) {
					auto pt = get_real_point({ x, y });
					double fp = f(pt.x, pt.y), gp = g(pt.x, pt.y);
					double al = atan2(gp, fp) + acos (-1.0);
					gr_plot->set_pixel(x, y, c.get_color(al));
				}
			}
		}

		/*void plot2d::fast_drawing()
		{
	
		}*/

		void plot2d::draw_field(func2d & f, func2d & g, colormap & c, real_rectangle region)
		{
			if (region.is_degenerate()) region = region_;
			auto sz = gr_plot->size();
			point a = get_point(real_point{ region.x, region.y });
			point b = get_point(real_point{ region.x + region.width, region.y + region.height });
			for (int x = min(a.x, b.x); x <= max(b.x, a.x); x++) {
				for (int y = min(a.y, b.y); y <= max(a.y, b.y); y++) {
					//if (x % 10 || y % 10) continue;
					if (rand() % 50) continue;
					auto pt = get_real_point({ x, y });
					double fp = f(pt.x, pt.y) + pt.x, gp = g(pt.x, pt.y) + pt.y;
					auto vp = get_point({ fp, gp });
					/*vp.x = 10.0 * vp.x / sqrt(sqr((double)vp.x) + sqr((double)vp.y));
					vp.y = 10.0 * vp.y / sqrt(sqr((double)vp.x) + sqr((double)vp.y));*/
					vp.x -= x;
					vp.y -= y;
					double l = sqrt(sqr((double)vp.x) + sqr((double)vp.y));
					vp.x = 10.0 * vp.x / l + x;
					vp.y = 10.0 * vp.y / l + y;

					//double al = atan2(gp, fp) + acos(-1.0);
					gr_plot->line({ x, y }, { vp.x, vp.y }, color{0,0,255});
					draw_arrowhead({ x, y }, { vp.x, vp.y }, color{ 0,127,0 }, 5, 2, 4);
				}
			}
		
		}

		void plot2d::draw_realization(func2d & f, func2d & g, colormap & c, real_point pt, double dt, int nt)
		{
			point vp = get_point(pt) - point{ window_.x, window_.y };
			int sign = 1;
			if (dt < 0) sign = -1;
			if (!flag_pbuffer) {
				point a = get_point(real_point{ region_.x, region_.y });
				point b = get_point(real_point{ region_.x + region_.width, region_.y + region_.height });

				rectangle r;
				r.x = min(a.x, b.x);
				r.y = min(a.y, b.y);
				r.width = abs(b.x - a.x);
				r.height = abs(b.y - a.y);
				pbuffer.attach(gr_plot->handle(), r);
			}
			for (int i = 0; i < nt; i++) {
				double fp = f(pt.x, pt.y) * dt, gp = g(pt.x, pt.y) * dt;
				auto vc = get_point({ pt.x + fp, pt.y + gp }) - point{window_.x, window_.y};
				/*if (flag_was) {
					if (vp.x <= window_.width && vp.x >= 0 && vp.y <= window_.height && vp.y >= 0) {
						if (was[vp.x + vp.y * (window_.width + 1)] && was[vp.x + vp.y * (window_.width + 1)] != nt) break;
						was[vp.x + vp.y * (window_.width + 1)] = nt;
					}
				}*/
				pbuffer.line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color(sign * (double)i / nt), 0.0);
				// gr_plot->line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color(sign * (double)i / nt)); // , 5, 2, 4);
				vp = vc;
				pt.x += fp, pt.y += gp;
			}
			if (!flag_pbuffer) {
				pbuffer.close();
			}
		}

		void plot2d::draw_integrate_field(func2d & f, func2d & g, colormap & c, real_rectangle region, bool flag_was)
		{
			if (region.is_degenerate()) region = region_;
			int q;
			int * was = &q;
			if (flag_was) {
				was = new int[(window_.height + 1) * (window_.width + 1)];
				memset(was, 0, (window_.height + 1) * (window_.width + 1));
			}
		
			clock_t be = clock();
			auto sz = gr_plot->size();
			point a = get_point(real_point{ region.x, region.y });
			point b = get_point(real_point{ region.x + region.width, region.y + region.height });
		
			rectangle r;
			r.x = min(a.x, b.x);
			r.y = min(a.y, b.y);
			r.width = abs(b.x - a.x);
			r.height = abs(b.y - a.y);
			pbuffer.attach(gr_plot->handle(), r);
			flag_pbuffer = true;
		
			double dt = 1.0 / (1 << 10);
			int nt = 1;
			const int NT = 500;
			for (int x = min(a.x, b.x); x <= max(a.x, b.x); x++) {
				for (int y = min(a.y,b.y); y <= max(a.y,b.y); y++) {
					if (rand () % 500) continue;
					//if (pt != point{ x, y }) continue;
					auto pt = point{x, y};
					draw_realization(f, g, c, get_real_point(pt), dt, NT);
					draw_realization(f, g, c, get_real_point(pt), -dt, NT);
					/*point vp = { x, y };
					for (int i = 0; i < NT; i++) {
						double fp = f(pt.x, pt.y) * dt, gp = g(pt.x, pt.y) * dt;
						auto vc = get_point({ pt.x + fp, pt.y + gp });
						if (flag_was) {
							if (vp.x <= window_.width && vp.x >= 0 && vp.y <= window_.height && vp.y >= 0) {
								if (was[vp.x + vp.y * (window_.width + 1)] && was[vp.x + vp.y * (window_.width + 1)] != nt) break;
								was[vp.x + vp.y * (window_.width + 1)] = nt;
							}
						}
						pbuffer.line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color((double)i / NT), 0.0); 
						//gr_plot->line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color((double)i / NT)); // , 5, 2, 4);
						vp = vc;
						pt.x += fp, pt.y += gp;
					}*/
					/*pt = get_real_point({ x, y });
					vp = { x, y };
					for (int i = 0; i < NT; i++) {
						double fp = -f(pt.x, pt.y) * dt, gp = -g(pt.x, pt.y) * dt;
						auto vc = get_point({ pt.x + fp, pt.y + gp });
						if (flag_was) {
							if (vp.x <= window_.width && vp.x >= 0 && vp.y <= window_.height && vp.y >= 0) {
								if (was[vp.x + vp.y * (window_.width + 1)] && was[vp.x + vp.y * (window_.width + 1)] != nt) break;
								was[vp.x + vp.y * (window_.width + 1)] = nt;
							}
						}
						//gr_plot->line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color((double)-i / NT)); // , 5, 2, 4);
						pbuffer.line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color((double)-i / NT), 0.0);
						vp = vc;
						pt.x += fp, pt.y += gp;
					}
					nt++;*/
				}
			}
			pbuffer.close();
			flag_pbuffer = false;

			if (flag_was) {
				delete[]was;
			}
			cerr << clock() - be << endl;
		}


		nana::string to_string(double x, int count)
		{
			std::wstringstream ss;
			//ss.width (count);
			ss << std::fixed << std::setprecision(count) << x;
			return ss.str();
			/*wchar_t dst[1000];
			memset (dst, 0, sizeof (dst));
			_swprintf (dst, L"%6.3lf", x);
			return nana::string (dst);*/
		}

		void plot2d::clear()
		{
			gr_plot->rectangle(true, { 255,255,255 });
		}

		void plot2d::draw(nana::paint::graphics & gr)
		{
			gr_plot->paste(gr, 0, 0);
		}

		void plot2d::fill_color(const point& a, const color& fill_color = { 255, 255, 255 }, const color& border_color = { 0,0,0 })
		{
			auto sz = gr_plot->size();
			paint::pixel_buffer pbuffer;
			rectangle r;
			r.x = 0;
			r.y = 0;
			r.width = sz.width;
			r.height = sz.height;
			gr_plot->flush();
			pbuffer.attach(gr_plot->handle(), r);

			int xt[4] = { -1, 0, 1, 0 };
			int yt[4] = { 0, -1, 0, 1 };
			queue <point> q;
			q.push(a);
			set <point> was;
			was.insert(a);
			int k = 0, p = 0;
			//printf("%d %d %d\n", a.x, a.y, fill_color.argb().value);
			while (q.size()) {
				point cur = q.front();
				q.pop();
				//gr_plot->set_pixel(cur.x, cur.y, fill_color);

				pixel_color_t* cur_color = pbuffer[cur.y] + cur.x;
				cur_color->value = fill_color.argb().value;
				for (int i = 0; i < 4; i++) {
					point next = { cur.x + xt[i], cur.y + yt[i] };
					if (next.x >= (int)sz.width || next.y >= (int)sz.height || next.x < 0 || next.y < 0) continue;
					pixel_color_t* line = pbuffer[next.y] + next.x;
					//size_t lineBytes = pbuffer.bytes_per_line();
					//const void * ptr = gr_plot->pixmap(), *ptr2 = gr_plot->handle();
					//color next_color (line->element.red, line->element.green, line->element.blue, line->element.alpha_channel);
					if ((line->value & ((1 << 24) - 1)) != (border_color.argb().value & ((1 << 24) - 1)) && was.count(next) == 0) {
						q.push(next);
						was.insert(next);
					}
				}
			}
			pbuffer.close();
		}

		void plot2d::draw_arrowhead(const point& a, const point& b, const color& color, double h, double w, double corner)
		{
			double l = sqrt(sqr(a.x - b.x) + sqr(a.y - b.y));
			h /= l; w /= l; corner /= l;
			point c = { (int)(b.x - (b.x - a.x) * h), (int)(b.y - (b.y - a.y) * h) };
			point d = { (int)(c.x + (b.y - a.y) * w), (int)(c.y - (b.x - a.x) * w) };
			point f = { (int)(c.x - (b.y - a.y) * w), (int)(c.y + (b.x - a.x) * w) };
			point g = { (int)(b.x - (b.x - a.x) * corner), (int)(b.y - (b.y - a.y) * corner) };
			gr_plot->line(b, d, color);
			gr_plot->line(b, f, color);
			gr_plot->line(d, g, color);
			gr_plot->line(g, f, color);
			//fill_color({ (b.x + g.x + f.x) / 3, (b.y + g.y + f.y) / 3 }, color, color);
			//fill_color({ (b.x + g.x + d.x) / 3, (b.y + g.y + d.y) / 3 }, color, color);
		}

		bool IsZero(float fValue)
		{
			return fValue < 0.01;
		}

		// Вычисляет длину вектора
		double length(const real_point& p) {
			return std::sqrt(p.x * p.x + p.y * p.y);
		}

		// Нормализует вектор (приводит его длину к 1)
		real_point normalize(const real_point& p) {
			double len = length(p);
			return { p.x / len, p.y / len };
		}

		// Создает перпендикулярный вектор (поворот на 90 градусов против часовой стрелки)
		real_point perpendicular(const real_point& p) {
			return { -p.y, p.x };
		}

		// Вычисляет координаты наконечника стрелы
		std::pair<real_point, real_point> computeArrowhead(const real_point& start, const real_point& end, double backOffset, double sideOffset) 
		{
			// Вектор направления стрелы
			real_point direction = { end.x - start.x, end.y - start.y };
			real_point normalizedDir = normalize(direction);

			// Отступаем назад от конечной точки
			real_point backPoint = { end.x - normalizedDir.x * backOffset, end.y - normalizedDir.y * backOffset };

			// Вычисляем перпендикулярный вектор и нормализуем его
			real_point perp = perpendicular(normalizedDir);

			// Вычисляем точки наконечника, отступая в обе стороны по перпендикуляру
			real_point arrowLeft = { backPoint.x + perp.x * sideOffset, backPoint.y + perp.y * sideOffset };
			real_point arrowRight = { backPoint.x - perp.x * sideOffset, backPoint.y - perp.y * sideOffset };

			return { arrowLeft, arrowRight };
		}

		void plot2d::draw_direction_wave(float dScale)
		{
			unsigned int unWidth  = u[0].size() / (100 * dScale);
			unsigned int unHeight = u.size() / (100 * dScale);

			if (unWidth < 10) unWidth = 10;
			if (unHeight < 5) unHeight = 5;

			for (unsigned int unY = unHeight; unY <= u.size() - unHeight; unY += unHeight)
			{
				for (unsigned int unX = unWidth; unX <= u[unY].size() - unWidth; unX += unWidth)
				{
					if (IsZero(eta[unY][unX]))
						continue;

					float dMag = sqrt(u[unY][unX] * u[unY][unX] + v[unY][unX] * v[unY][unX]) * 10 * eta[unY][unX];

					if (dMag > MAX_MAG_WAVE)
						dMag = MAX_MAG_WAVE;
					else if (dMag <= MIN_MAG_WAVE)
						dMag = MIN_MAG_WAVE;

					const float dAngle = atan2(v[unY][unX], u[unY][unX]);

					const unsigned int unPointX = window_.x + static_cast<unsigned int>(((float)window_.width / (float)size_x * unX) + 0.5);
					const unsigned int unPointY = window_.y + static_cast<unsigned int>(((float)window_.height / (float)size_y * unY) + 0.5);

					const coord oPoint1{ unPointX - unWidth * dMag * dScale * cos(dAngle), unPointY + unWidth * dMag * dScale * sin(dAngle)};
					const coord oPoint2{ unPointX + unWidth * dMag * dScale * cos(dAngle), unPointY - unWidth * dMag * dScale * sin(dAngle)};
					
					gr_plot->line({ oPoint1.x, oPoint1.y }, { oPoint2.x, oPoint2.y }, { 0, 0, 0 });

					const float dX = oPoint2.x - oPoint1.x;
					const float dY = oPoint2.y - oPoint1.y;

					const float dLength = sqrt(dX * dX + dY * dY);

					if (dLength < 3)
						continue;

					std::pair<real_point, real_point> oArrow = computeArrowhead({ (float)oPoint1.x, (float)oPoint1.y }, { (float)oPoint2.x, (float)oPoint2.y }, max(dLength / 3., 2.), max(dLength / 10., 2.));

					gr_plot->line({ oPoint2.x, oPoint2.y }, { (int)oArrow.first.x, (int)oArrow.first.y }, { 0, 0, 0 });
					gr_plot->line({ oPoint2.x, oPoint2.y }, { (int)oArrow.second.x, (int)oArrow.second.y }, { 0, 0, 0 });
				}
			}
		}

		void plot2d::draw_axis(bool axis, bool arrow)
		{
		}

		void plot2d::colorbar(const colormap& cm, const nana::string& wsUnitMeasure, double dTick)
		{
			unsigned int unFontSize = gr_plot->typeface().size();
			double dLineHeight = (gr_plot->typeface().size() > 30) ? (double)(gr_plot->typeface().size()) / 15. : 2.;

			//double step = -((number_one[5].scale_h - number_one[0].scale_h) * (double)(1) / (-b + a) + number_one[0].scale_h) + (number_one[0].scale_h);
			double a = cm.min(), b = cm.max();
			int ay = window_.y + unFontSize * 0.75, by = window_.y + window_.height;
			int w = unFontSize, cx = (int)window_a.width - w - unFontSize * 3;

			nana::color c;

			std::vector<double> ticks = cm.get_ticks(dTick);

			if (ticks.empty())
				ticks = cm.get_ticks();

			const unsigned step = std::abs(by - ay) / (ticks.size());

			{
				const color BeginColor = cm.get_color(cm.min());
				const color EndColor   = cm.get_color(cm.max());

				gr_plot->gradual_rectangle(rectangle{ cx, (int)(by - step), (unsigned)w, (unsigned)step }, BeginColor, BeginColor, true);
				gr_plot->gradual_rectangle(rectangle{ cx, ay, (unsigned)w, (unsigned)step }, EndColor, EndColor, true);
			}

			if (0 == dTick)
			{
				const std::vector<color> colors = cm.get_colors();
				for (int i = 0; i < ticks.size() - 1; ++i)
					gr_plot->gradual_rectangle(rectangle{ cx, by - (int)(i * step + step * 1.5), (unsigned)w, (unsigned)step }, colors[i + 1], colors[i], true);
			}
			else
			{
				int ny = by - step / 2;
				for (unsigned int i = 0; i < ticks.size() - 1; ++i)
				{
					int nTempY = ny;
					for (double value = ticks[i]; value < ticks[i + 1]; value += (ticks[i + 1] - ticks[i]) / step)
					{
						c = cm.get_color(value);
						gr_plot->line({ cx, nTempY }, { cx + w - 1, nTempY }, c);
						--nTempY;
					}
					ny -= step;
				}
			}

			nana::paint::font oOldFont{ gr_plot->typeface() };
			nana::paint::font oNewFont(oOldFont.name().c_str(), oOldFont.size() * 0.75);
			gr_plot->typeface(oNewFont);

			unFontSize = gr_plot->typeface().size();
			dLineHeight = (gr_plot->typeface().size() > 30) ? (double)(gr_plot->typeface().size()) / 15. : 2.;

			const unsigned int unKoef = (L"km" == wsUnitMeasure) ? 1000 : 1;

			int nY = by - step / 2;
			for (const double dValue : ticks)
			{
				//gr_plot->line({ cx, nY }, { cx + w - 1 , nY }, {0, 0, 0});
				gr_plot->rectangle({ cx, nY, (unsigned)(w - 1 + dLineHeight), (unsigned)dLineHeight }, true, {0, 0, 0});
				gr_plot->string({ (int)cx + w + std::max(5, (int)(unFontSize / 10)), nY - (int)(unFontSize / 1.25) }, ConvertToString(dValue / unKoef));
				nY -= step;
			}

			gr_plot->string({ (int)(cx), (int)(ay - unFontSize * 1.25 - 2) }, wsUnitMeasure);

			gr_plot->rectangle({(int)(cx - dLineHeight / 2), ay, (unsigned)(w + dLineHeight), (unsigned)dLineHeight}, true, { 0, 0, 0 });
			gr_plot->rectangle({ (int)(cx + w), ay, (unsigned)dLineHeight, (unsigned)(by - ay + dLineHeight) }, true, { 0, 0, 0 });
			gr_plot->rectangle({ (int)(cx - dLineHeight + 1), ay, (unsigned)dLineHeight, (unsigned)(by - ay + dLineHeight) }, true, { 0, 0, 0 });
			gr_plot->rectangle({ (int)(cx - dLineHeight / 2), (int)(by), (unsigned)(w + dLineHeight), (unsigned)dLineHeight }, true, { 0, 0, 0 });

			gr_plot->typeface(oOldFont);
			//gr_plot->rectangle(rectangle{ cx, ay, (unsigned int)w, (unsigned int)by - ay }, false, { 0, 0, 0 });
		}

		void plot2d::draw_grid(bool grid, bool ticks, double step_x, double step_y, double step_font_x, double step_font_y, double height)
		{
			const double dLineHeight = (gr_plot->typeface().size() > 30) ? (double)(gr_plot->typeface().size()) / 15. : 2.;

			const nana::point oLT = get_point({ region_.x , region_.y });
			const nana::point oRB = get_point({ region_.x + region_.width, region_.y + region_.height });

			const nana::size oHorizontal((unsigned)(oRB.x - oLT.x + dLineHeight), (unsigned)dLineHeight);
			const nana::size oVertical((unsigned)dLineHeight, (unsigned)(oRB.y - oLT.y));

			gr_plot->rectangle(nana::rectangle(get_point({ region_.x , region_.y }) - nana::point(0, (int)(dLineHeight / 2)), oHorizontal), true, { 0, 0, 0 }); // Bottom line
			gr_plot->rectangle(nana::rectangle(get_point({ region_.x , region_.y + region_.height }) - nana::point(0, (int)(dLineHeight / 2)), oHorizontal), true, { 0, 0, 0 }); // Top line

			gr_plot->rectangle(nana::rectangle(get_point({ region_.x , region_.y }), oVertical), true, { 0, 0, 0 }); // Left line
			gr_plot->rectangle(nana::rectangle(get_point({ region_.x + region_.width , region_.y }), oVertical), true, { 0, 0, 0 }); // Right line

			int k = (int)trunc(log(region_.width) / log(axis_x_base_));
			double dx = exp(log(axis_x_base_) * k), dx_font = dx;
			int max_k = window_.width / (gr_plot->typeface().size() * (k + 5));
			// *dr = 
			if (step_x > 0.001) dx = step_x;
			else{
				while (region_.width < max_k * dx / 10) {
					dx /= 10, k--;
				}
				if (region_.width < max_k * dx / 5) {
					dx /= 5, k--;
				}
				if (region_.width < max_k * dx / 2) {
					dx /= 2, k--;
				}
			}
			//printf("dx: %lf k: %d\n", dx, abs(k));
			//double dx2 = round(log(region_.width / 2.0) / log(axis_x_base_));
			nana::color oGridColor{ 235, 235, 235 }, oGridColor2{ 25, 25, 25 };

			for (double x = round(region_.x / dx + 0.5) * dx; x < region_.width + region_.x; x += dx) 
			{
				if (abs(x) < dx / 100) x = 0; 
				
				if (grid)
				{
					gr_plot->line(get_point({ x, region_.y }), get_point({ x, region_.y + region_.height }), oGridColor);
					//std::swap(oGridColor, oGridColor2);
				}

				nana::size oRectSize{ (unsigned int)(gr_plot->typeface().size() / 10), (unsigned int)(gr_plot->typeface().size() / 3) };
				nana::point oRecCenter{ get_point({ x, region_.y}) - nana::point{(int)(oRectSize.width / 2),  (int)(oRectSize.height / 2) } };

				gr_plot->rectangle(nana::rectangle{ oRecCenter , oRectSize }, true, { 0, 0, 0 });
			}
			if (step_font_x > 0.01) dx_font = step_font_x;
			else dx_font = dx;

			double dXStart = round(region_.x / dx_font + 0.5) * dx_font;
			double dXEnd = region_.width + region_.x;

			nana::point oXLabelPointStart = get_point({ region_.x, region_.y }) - nana::point{ 0, -(int)(gr_plot->typeface().size() / 10) };
			nana::point oXLabelPointEnd   = get_point({ region_.x + region_.width, region_.y }) - nana::point{ (int)(gr_plot->typeface().size() * 1.15), -(int)(gr_plot->typeface().size() / 10) };

			for (double x = dXStart; x < dXEnd; x += dx_font)
			{
				nana::string val = ConvertToString(std::abs(x - ((dXEnd > 180) ? 0 : 0)));

				/*
				if (x < 0)
					val += axix_x_label_start;
				else
					val += axix_x_label_end;
				*/
				int nXPointEnd = get_point({ x, region_.y }).x + (int)(gr_plot->typeface().size() * (val.length() == 1 ? 1 : (1, (val.length() - 1) / 2.)) * 0.65);

				if (nXPointEnd >= oXLabelPointEnd.x || nXPointEnd <= (oXLabelPointStart.x + (gr_plot->typeface().size() * (val.length() - 1) / 2.)))
					break;

				gr_plot->string(get_point({ x, region_.y }) - nana::point{ (int)(gr_plot->typeface().size() * (val.length() == 1 ? 0.3 :(1, (val.length() - 1) / 2.))), -(int)(gr_plot->typeface().size() / 10) }, val);

				nana::size oRectSize{ (unsigned int)(gr_plot->typeface().size() / 5), (unsigned int)(gr_plot->typeface().size() / 1.5) };
				nana::point oRecCenter{ get_point({ x, region_.y}) - nana::point{(int)(oRectSize.width / 2),  (int)(oRectSize.height / 2) } };

				gr_plot->rectangle(nana::rectangle{ oRecCenter , oRectSize }, true, { 0, 0, 0 });
			}

			if (region_.width + region_.x > 0)
			{
				gr_plot->string(oXLabelPointEnd, axix_x_label_end);
				
				if (round(region_.x / dx + 0.5) * dx < 0)
					gr_plot->string(oXLabelPointStart, axix_x_label_start);
			}
			else if (round(region_.x / dx + 0.5) * dx < 0)
				gr_plot->string(oXLabelPointEnd, axix_x_label_start);

			if (axis_y_log_) 
			{
				double dy = exp((floor(log(region_.y) / axis_y_log_base_) + 1.0) * axis_y_log_base_);
				for (double y = region_.y; dy < region_.height + region_.y; dy *= axis_y_base_) {
					if (grid) gr_plot->line(get_point({ region_.x, dy }), get_point({ region_.x + region_.width, dy }), { 235, 235, 235 });
					gr_plot->line(get_point({ region_.x, dy }) + nana::point{ -5, 0 }, get_point({ region_.x, dy }) + nana::point{ 5, 0 }, { 0, 0, 0 });
					gr_plot->string(get_point({ region_.x, dy }) + nana::point{ -40, -7 }, to_string(dy, 4));
				}
			}
			else {
				int k = (int)trunc(log(region_.height) / log(axis_y_base_));
				int max_k = window_.height / (gr_plot->typeface().size() * (k + 2));

				double dy = exp(log(axis_y_base_) * k), dy_font = dy;

				if (step_y > 0.001) dy = step_y;
				else {
					while (region_.width < max_k * dy / 10) {
						dy /= 10, k--;
					}
					if (region_.width < max_k * dy / 5) {
						dy /= 5, k--;
					}
					if (region_.width < max_k * dy / 2) {
						dy /= 2, k--;
					}
				}


				if (step_font_y > 0.01) dy_font = step_font_y;
				else dy_font = dy;

				double dYStart = round(region_.y / dy_font + 0.5) * dy_font;
				double dYEnd = (-1 == height) ? region_.height + region_.y + dy_font / axis_y_base_ - 1 : height;

				nana::color oGridColor{ 235, 235, 235 }, oGridColor2{ 25, 25, 25 };

				for (double y = round(region_.y / dy + 0.5) * dy; y < region_.height + region_.y; y += dy)
				{
					if (abs(y) < dy / 100) y = 0;
					if (grid)
					{
						gr_plot->line(get_point({ region_.x, y }), get_point({ region_.x + region_.width, y }), oGridColor);
						//std::swap(oGridColor, oGridColor2);
					}

					nana::size oRectSize{ (unsigned int)(gr_plot->typeface().size() / 3), (unsigned int)(gr_plot->typeface().size() / 10) };
					nana::point oRecCenter{ get_point({ region_.x, y}) - nana::point{(int)((oRectSize.width - dLineHeight) / 2),  (int)(oRectSize.height / 2) } };

					gr_plot->rectangle(nana::rectangle{ oRecCenter , oRectSize }, true, { 0, 0, 0 });
				}

				for (double y = dYStart; y < dYEnd; y += dy_font)
				{
					nana::string val = ConvertToString(std::abs(y));

					/*
					if (y > 0)
						val += axix_y_label_start;
					else
						val += axix_y_label_end;
					*/

					int nYPointStart = get_point({ region_.x, y }).y - (int)(gr_plot->typeface().size() / 1.25);

					if (y >= nYPointStart)
						break;

					gr_plot->string(get_point({ region_.x, y }) - nana::point{ (int)(gr_plot->typeface().size() * 1.5 + (gr_plot->typeface().size() * (val.length() - 1) * 0.6 )), (int)(gr_plot->typeface().size() / 1.25) }, val);
				
					nana::size oRectSize{ (unsigned int)(gr_plot->typeface().size() / 1.5), (unsigned int)(gr_plot->typeface().size() / 5) };
					nana::point oRecCenter{ get_point({ region_.x, y}) - nana::point{(int)(oRectSize.width / 2),  (int)(oRectSize.height / 2) } };

					gr_plot->rectangle(nana::rectangle{ oRecCenter , oRectSize }, true, { 0, 0, 0 });
				}

				if (round(region_.y / dy + 0.5) * dy < 0)
				{
					gr_plot->string(get_point({ region_.x, region_.y }) - nana::point{ (int)(gr_plot->typeface().size() * 1.15), (int)(gr_plot->typeface().size() * 1.25) }, axix_y_label_end);

					if (region_.height + region_.y > 0)
						gr_plot->string(get_point({ region_.x, region_.height + region_.y }) - nana::point{ (int)(gr_plot->typeface().size() * 1.15), 0 }, axix_y_label_start);
				}
				else if (region_.height + region_.y > 0)
					gr_plot->string(get_point({ region_.x, region_.height + region_.y }) - nana::point{ (int)(gr_plot->typeface().size() * 1.15), 0 }, axix_y_label_end);

			}
		}

		void plot2d::draw_time(const nana::string& time)
		{
			const double dLineHeight = (gr_plot->typeface().size() > 30) ? (double)(gr_plot->typeface().size()) / 15. : 2.;

			const unsigned uWidth  = gr_plot->typeface().size() * 5.4;
			const unsigned uHeight = gr_plot->typeface().size() + 3 * dLineHeight;

			gr_plot->rectangle({ window_.x, window_.y, uWidth, uHeight }, true, {255, 255, 255});
			gr_plot->rectangle({ (int)(window_.x + uWidth - dLineHeight + 1), window_.y, (unsigned)dLineHeight, uHeight }, true, {0, 0, 0});
			gr_plot->rectangle({ window_.x, (int)(window_.y + uHeight - dLineHeight + 1), uWidth, (unsigned)dLineHeight }, true, { 0, 0, 0 });
			gr_plot->string(nana::point{ (int)(window_.x + gr_plot->typeface().size() / 4.), (int)(window_.y - gr_plot->typeface().size() / 6.) }, time);
		}

		void plot2d::draw_mareograms(const std::vector<real_point>& arCoords, bool drawIndex)
		{
			const unsigned int width = gr_plot->typeface().size() / 4;
			nana::point tempPoint;
			nana::point centerPoint;

			nana::paint::font oOldFont(gr_plot->typeface().name().c_str(), gr_plot->typeface().size(), gr_plot->typeface().bold(), gr_plot->typeface().italic());
			nana::paint::font oNewFont(gr_plot->typeface().name().c_str(), gr_plot->typeface().size() / 2.);

			for (unsigned int i = 0; i < arCoords.size(); ++i)
			{
				tempPoint = get_point(arCoords[i]);
				// Необходимо ддобавить масштабируемость
				// Чтобы масштаб координат сопоставить с масштабом самой картинки акватории
				gr_plot->round_rectangle({ (int)(tempPoint.x - width / 2), (int)(tempPoint.y - width / 2), width, width }, (int)(width / 2), (int)(width / 2), { 255, 0, 0 }, true, { 255, 0, 0 });
				gr_plot->round_rectangle({ (int)(tempPoint.x - width / 4), (int)(tempPoint.y - width / 4), (unsigned int)(width / 2), (unsigned int)(width / 2) }, (int)(width / 2), (int)(width / 2), { 100, 0, 200 }, true, { 100, 0, 200 });

				if (drawIndex)
				{
					nana::string index{ ConvertToString(i + 1) };
					gr_plot->typeface(oNewFont);
					gr_plot->string({ (int)(tempPoint.x - (oNewFont.size() + ((index.length() > 1) ? 1 : 0)) / 2.9), (int)(tempPoint.y - oNewFont.size() * 1.5) }, index);
					gr_plot->typeface(oOldFont);
				}
			}
		}
}


