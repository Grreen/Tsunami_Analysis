#include "plot.h"

namespace nana {
	color operator * (color x, double y)
	{
		return color{ (unsigned int)(x.r() * y), (unsigned int)(x.g() * y), (unsigned int)(x.b() * y) };
	}

	color operator + (color x, color y)
	{
		return color{ (unsigned int)(x.r() + y.r()), (unsigned int)(x.g() + y.g()), (unsigned int)(x.b() + y.b()) };
	}

	nana::point plot2d::get_point(const real_point pt)
	{
		point ans;
		if (axis_x_log_) ans.x = (int)(window_.x + window_.width * (log(pt.x) / axis_x_log_base_ - region_log_.x) / region_log_.width);
		else ans.x = (int)(window_.x + window_.width * (pt.x - region_.x) / region_.width);
		if (axis_y_log_) ans.y = (int)(window_.y + window_.height - window_.height * (log(pt.y) / axis_y_log_base_ - region_log_.y) / region_log_.height);
		else ans.y = (int)(window_.y + window_.height - window_.height * (pt.y - region_.y) / region_.height);
		return ans;
	}

	real_point plot2d::get_real_point(const nana::point pt)
	{
		real_point ans;
		if (axis_x_log_) ans.x = exp((region_log_.width * ((double)pt.x - window_.x) / window_.width + region_log_.x) * axis_x_log_base_);
		else ans.x = region_.width * ((double)pt.x - window_.x) / window_.width + region_.x;

		if (axis_y_log_) ans.y = exp((region_log_.height * ((double)window_.y + window_.height - pt.y) / window_.height + region_log_.y) * axis_y_log_base_);
		else ans.y = region_.height * ((double)window_.y + window_.height - pt.y) / window_.height + region_.y;
		return ans;
	}

	void plot2d::line(std::vector <real_point> points, nana::color color, int point_radius)
	{
		if (!points.size()) return;
		for (int i = 1; i < (int)points.size(); i++) {
			gr_plot->set_color(color);
			gr_plot->line(get_point(points[i - 1]), get_point(points[i]));
			//gr_plot->round_rectangle (rectangle { cur, nana::size {2,2} }, point_radius, point_radius, nana::color { 0, 0, 255 }, true, nana::color { 0, 0, 255 });
			//	point_radius
		}
	}
	
	void plot2d::smooth_line(point a, point b, color color)
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
				gr_plot->set_pixel(i, j, color);
			}
			if (i == x2) fx = false;
			if (j == y2) fy = false;
		}
		color.alpha(1.0);
		gr_plot->set_pixel(x1, y1, color);
		gr_plot->set_pixel(x2, y2, color);
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

	void plot2d::draw_field(func2d & f, func2d & g, colormap & c, real_rectangle region)
	{
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

	void plot2d::draw_integrate_field(func2d & f, func2d & g, colormap & c, real_rectangle region, bool flag_was)
	{
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
		double dt = 1.0 / (1 << 7);
		int nt = 1;
		for (int x = min(a.x, b.x); x <= max(a.x, b.x); x++) {
			for (int y = min(a.y,b.y); y <= max(a.y,b.y); y++) {
				if (rand() % 100) continue;
				auto pt = get_real_point({ x, y });
				point vp = { x, y };
				for (int i = 0; i < 300; i++) {
					double fp = f(pt.x, pt.y) * dt, gp = g(pt.x, pt.y) * dt;
					auto vc = get_point({ pt.x + fp, pt.y + gp });
					if (flag_was) {
						if (vp.x <= window_.width && vp.x >= 0 && vp.y <= window_.height && vp.y >= 0) {
							if (was[vp.x + vp.y * (window_.width + 1)] && was[vp.x + vp.y * (window_.width + 1)] != nt) break;
							was[vp.x + vp.y * (window_.width + 1)] = nt;
						}
					}
					gr_plot->line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color(i)); // , 5, 2, 4);
					vp = vc;
					pt.x += fp, pt.y += gp;
				}
				pt = get_real_point({ x, y });
				vp = { x, y };
				for (int i = 0; i < 300; i++) {
					double fp = -f(pt.x, pt.y) * dt, gp = -g(pt.x, pt.y) * dt;
					auto vc = get_point({ pt.x + fp, pt.y + gp });
					if (flag_was) {
						if (vp.x <= window_.width && vp.x >= 0 && vp.y <= window_.height && vp.y >= 0) {
							if (was[vp.x + vp.y * (window_.width + 1)] && was[vp.x + vp.y * (window_.width + 1)] != nt) break;
							was[vp.x + vp.y * (window_.width + 1)] = nt;
						}
					}
					gr_plot->line({ vp.x, vp.y }, { vc.x, vc.y }, c.get_color(-i)); // , 5, 2, 4);
					vp = vc;
					pt.x += fp, pt.y += gp;
				}
				nt++;
			}
		}
		if (flag_was) {
			delete[]was;
		}
		cerr << clock() - be << endl;
	}


	nana::string to_string(double x, int count)
	{
		std::wstringstream ss;
		//ss.width (count);
		ss << std::setprecision(count) << x;
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

	void plot2d::fill_color(point a, color fill_color = { 255, 255, 255 }, color border_color = { 0,0,0 })
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

	void plot2d::draw_arrowhead(point a, point b, color color, double h = 15, double w = 5, double corner = 12)
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

	void plot2d::draw_axis()
	{
		color color = { 0, 0, 0 };
		//gr_plot->set_color (color);
		if (axis_x_) {
			point a = get_point({ region_.x, region_.y }), b = get_point({ region_.x + region_.width, region_.y });
			gr_plot->line(a, b, color);
			draw_arrowhead(a, b, color);
		}
		if (axis_y_) {
			point a = get_point({ region_.x, region_.y }), b = get_point({ region_.x, region_.y + region_.height });
			gr_plot->line(a, b, color);
			draw_arrowhead(a, b, color);
		}

		nana::point pty = { window_.x + 10, window_.y };
		gr_plot->rectangle(nana::rectangle{ pty,{ (unsigned int)WIDTH_CHAR * (int)axis_y_label_.size(), 20 } }, true, { 255, 255, 255 });
		gr_plot->string(pty, axis_y_label_, { 0, 0, 255 });

		nana::point ptx = { (int)(window_.x + window_.width - 10 - WIDTH_CHAR * (int)axis_x_label_.size()), (int)(window_.y + window_.height - 40) };
		gr_plot->rectangle(nana::rectangle{ ptx,{ (unsigned int)WIDTH_CHAR * (int)axis_x_label_.size(), 20 } }, true, { 255, 255, 255 });
		gr_plot->string(ptx, axis_x_label_, { 0, 0, 255 });

		//gr_plot->string ({ (int)(window_.x + window_.width - 30), window_.y }, label_, { 0, 0, 255 });
	}

	void plot2d::draw_grid()
	{
		nana::paint::font ft(L"Consolas", 11u);
		//wprintf(L"name font : %s\n", gr_plot->typeface().name().c_str());
		gr_plot->typeface(ft);
		if (axis_x_log_) {
			double dx = 1.0;
			int k, en, be;
			do {
				dx *= axis_x_base_;
				be = (int)(floor(region_log_.x / log(dx)) + 1.0);
				en = (int)(floor((region_log_.x + region_log_.width) / log(dx)) + 1.0);
				k = en - be;
			} while (k > (int)window_.width / 70);
			for (double x = exp(be * log(dx)); x < region_.width + region_.x; x *= dx) {
				nana::string val = to_string(x, 2);
				gr_plot->line(get_point({ x, region_.y }), get_point({ x, region_.y + region_.height }), { 235, 235, 235 });
				gr_plot->line(get_point({ x, region_.y }) + nana::point{ 0, -5 }, get_point({ x, region_.y }) + nana::point{ 0, 5 }, { 0, 0, 0 });
				gr_plot->string(get_point({ x, region_.y }) + nana::point{ -WIDTH_CHAR * (int)val.size() / 2 + 1, 5 }, val);
			}
		}
		else {
			//double dx = /*std::max(1.0, */region_.width / (window_.width / 100);
			int k = (int)trunc(log(region_.width) / log(axis_x_base_));
			double dx = exp(log(axis_x_base_) * k);
			int max_k = window_.width / (WIDTH_CHAR * (k + 7));
			while (region_.width < max_k * dx / 10) {
				dx /= 10, k--;
			}
			if (region_.width < max_k * dx / 5) {
				dx /= 5, k--;
			}
			if (region_.width < max_k * dx / 2) {
				dx /= 2, k--;
			}
			//printf("dx: %lf k: %d\n", dx, abs(k));
			//double dx2 = round(log(region_.width / 2.0) / log(axis_x_base_));
			for (double x = round(region_.x / dx) * dx; x < region_.width + region_.x + dx / 10.0; x += dx) {
				if (abs(x) < dx / 100) x = 0;
				nana::string val = to_string(x, 0);
				gr_plot->line(get_point({ x, region_.y }), get_point({ x, region_.y + region_.height }), { 235, 235, 235 });
				gr_plot->line(get_point({ x, region_.y }) + nana::point{ 0, -5 }, get_point({ x, region_.y }) + nana::point{ 0, 5 }, { 0,0,0 });
				gr_plot->string(get_point({ x, region_.y }) + nana::point{ -WIDTH_CHAR * (int)val.size() / 2 + 1, 5 }, val);
			}
		}

		if (axis_y_log_) {
			double dy = exp((floor(log(region_.y) / axis_y_log_base_) + 1.0) * axis_y_log_base_);
			for (double y = region_.y; dy < region_.height + region_.y; dy *= axis_y_base_) {
				gr_plot->line(get_point({ region_.x, dy }), get_point({ region_.x + region_.width, dy }), { 235, 235, 235 });
				gr_plot->line(get_point({ region_.x, dy }) + nana::point{ -5, 0 }, get_point({ region_.x, dy }) + nana::point{ 5, 0 }, { 0, 0, 0 });
				gr_plot->string(get_point({ region_.x, dy }) + nana::point{ -40, -7 }, to_string(dy, 4));
			}
		}
		else {
			//double dy = 5 * ((region_.height / (window_.height / 40)) / 5);
			int k = (int)trunc(log(region_.height) / log(axis_y_base_));
			double dy = exp(log(axis_y_base_) * k);
			int max_k = window_.height / 30;
			//printf("dy : %lf k : %d\n", dy, k);
			while (region_.height < max_k * dy / 10) {
				dy /= 10, k--;
			}
			if (region_.height < max_k * dy / 5) {
				dy /= 5, k--;
			}
			if (region_.height < max_k * dy / 2) {
				dy /= 2, k--;
			}
			//printf("dy : %lf k : %d\n", dy, k);
			for (double y = round(region_.y / dy) * dy; y < region_.height + region_.y + dy / axis_y_base_; y += dy) {
				if (abs(y) < dy / 100) y = 0;
				nana::string val = to_string(y, k);
				gr_plot->line(get_point({ region_.x, y }), get_point({ region_.x + region_.width, y }), { 235, 235, 235 });
				gr_plot->line(get_point({ region_.x, y }) + nana::point{ -5, 0 }, get_point({ region_.x, y }) + nana::point{ 5, 0 }, { 0, 0, 0 });
				gr_plot->string(get_point({ region_.x, y }) + nana::point{ -WIDTH_CHAR * ((int)val.size() + 3), -(WIDTH_CHAR + 1) }, val);
				//gr_plot->string(get_point({ region_.x, y }) + nana::point{ -40, -7 }, std::to_wstring((int)y));
			}
		}
	}
}