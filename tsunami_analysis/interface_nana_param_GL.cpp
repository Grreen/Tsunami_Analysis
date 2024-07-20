#define _USE_MATH_DEFINES 
#include "plot.h"
#include "locale"
//#include "CImg.h"
#include "interface_GL.h"
#include "calculation.h"
#include "displaying.h"
#include "declaration.h"
#include "interface_nana.h"
#include <codecvt>
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

unsigned char* WindowDump()
{
	int width = size_window_w, height = size_window_h;
	FILE *fptr;
	static int counter = 0; // This supports animation sequences 
	char fname[32];
	unsigned char *image;
	// Allocate our buffer for the image 
	if ((image = (unsigned char*)malloc(3 * width * height * sizeof(char))) == NULL) {
		fprintf(stderr, "Failed to allocate memory for image\n");
		//return(FALSE);
	}
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	return image;

}



void save_position(nana::string name){
	cout << "111111\n";
	wfstream file;
	cout << "222222\n";
	file.open(name.c_str(), std::wfstream::out);

	file << rot.r <<  L" " << rot.phi << L" " << rot.teta << L"\n";
	file << at.x << " " << at.y << L" " << at.z << L"\n";
	
	file << gl_data.axis_font_size << L" " << gl_data.looking_at << L" " << gl_data.ratio << L"\n";
	file << gl_data.step_font.x << L" " << gl_data.step_font.y << L" " << gl_data.step_font.z << L"\n";
	file << gl_data.step_axis.x << " " << gl_data.step_axis.y << L"\n" ;
	
	file << s_param.zoom << L" " << s_param.NC << L"\n";

	file << m_c_s.alfa << L" " << m_c_s.dx1 << L" " << m_c_s.dx2 << L" " << m_c_s.dy1 << L" " << m_c_s.dy2 << L" " << m_c_s.dz << L"\n";

	file << gist_d.end_x << L" " << gist_d.end_y << L" " << gist_d.start_x << L" " << gist_d.start_y << "\n";
	file << gist_d.l_l.x << L" " << gist_d.l_l.y << L" " << gist_d.t_r.x << L" " << gist_d.t_r.y << L"\n";
	file << gist_d.size_x << L" " << gist_d.size_y << L"\n";
	cout << "33333\n";
	file.close();
}

std::string wchar2char(const std::wstring & p_in, const std::locale & p_loc) 
{
	typedef std::wstring::traits_type::state_type       TsType;
	typedef std::codecvt<wchar_t, char, TsType>         Tcvt;

	try{
		std::string     l_out(p_in.length(), char());
		const Tcvt &    l_cvt = std::use_facet<Tcvt>(p_loc);
		const wchar_t*  l_from_nxt;
		char*           l_to_nxt;
		TsType          l_state = TsType();
		l_cvt.out(l_state, &p_in[0], &p_in[0] + p_in.size(), l_from_nxt, &l_out[0], &l_out[0] + l_out.size(), l_to_nxt);
		return l_out;
	}
	catch (...)
	{
		return std::string();
	}
};


char const *russian_locale_designator = "rus";

void load_position(nana::string name, bool full){
	wfstream file;
	file.open(name.c_str(), std::wfstream::in);

	file >> rot.r >> rot.phi >> rot.teta;
	file >> at.x >> at.y >> at.z;

	file >> gl_data.axis_font_size >> gl_data.looking_at >> gl_data.ratio;
	file >> gl_data.step_font.x >> gl_data.step_font.y >> gl_data.step_font.z;
	file >> gl_data.step_axis.x >> gl_data.step_axis.y;

	file >> s_param.zoom >> s_param.NC;

	file >> m_c_s.alfa >> m_c_s.dx1 >> m_c_s.dx2 >> m_c_s.dy1 >> m_c_s.dy2 >> m_c_s.dz;
	if (full){
		file >> gist_d.end_x >> gist_d.end_y >> gist_d.start_x >> gist_d.start_y;
		file >> gist_d.l_l.x >> gist_d.l_l.y >> gist_d.t_r.x >> gist_d.t_r.y;
		file >> gist_d.size_x >> gist_d.size_y;
	}
	file.close();
}

void fm_param_gl(){
	cout << "3333333333\n";
	std::string path_pic;
	//----------------------------------------------------------------------//
	//---------------------------------FORM---------------------------------//
	//----------------------------------------------------------------------//
	form fm, fm1;
	//fm.move(0, 0);
	fm1.size({ 0, 0 });
	
	fm1.move(screen().get_primary().workarea().width, screen().get_primary().workarea().width);
	fm.move(0, 0);
	fm.size({ 420, 280 });
	fm.caption(L"DATA");
	//-----------------------------------------------------------------------//
	//--------------------------------PICTURE--------------------------------//
	//-----------------------------------------------------------------------//
	
	picture pic(fm1);
	nana::drawing dw1(pic);
	int schet = 0;
	pic.size(nana::size{ (unsigned int)size_window_h, (unsigned int)size_window_w });
	dw1.draw([&pic, &path_pic, &schet](nana::paint::graphics& graph){
		nana::paint::graphics gr;
		//gr.(graph.size());
		gr.make(nana::size(size_window_w, size_window_h));
		pic.size(nana::size{ (unsigned int)size_window_h, (unsigned int)size_window_w });
		nana::color col = { 0, 0, 0 };
		unsigned char *image = WindowDump();
		int width = size_window_w, height = size_window_h;
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				color a;
				a = { image[3 * j * width + 3 * i + 0], image[3 * j * width + 3 * i + 1], image[3 * j * width + 3 * i + 2] };
				gr.set_pixel(i, height - j, a);
			}
		}
		cout << "44444444444\n";
		std::string name = "gistograms\\3D\\" + to_string(schet) + ".bmp";
		gr.save_as_file(name.c_str());
	});

	place pl1(fm1);
	pl1.div("<pic>");
	pl1.field("pic") << pic;
	pl1.collocate();
	fm1.events().click([&dw1](){ dw1.update(); });
	//------------------------------------------------------------------------//
	//---------------------------------BUTTON---------------------------------//
	//------------------------------------------------------------------------//
	button btn_save(fm, L"Save position"), btn_load(fm, L"Load position"), btn_save_pic(fm, L"Save picture");
	
	btn_save_pic.events().click([&dw1, &fm1, &path_pic, &schet](){
		
		schet++;
		fm1.show();
		dw1.update();
	});
	
	btn_load.events().click([](){
		filebox fb(0, true);
		fb.add_filter(L"Save position", L"*.gldat;");
		if (fb())
		load_position( fb.file() , false);
	});

	btn_save.events().click([](){
		filebox fb(0, false);
		fb.add_filter(L"Save position", L"*.gldat;");
		if (fb()){
			nana::string path_file = fb.file();
			save_position(path_file);
			wcout << path_file << endl;
		}
	});

	//------------------------------------------------------------------------//
	//---------------------------------COMBOX---------------------------------//
	//------------------------------------------------------------------------//
	combox cm_looking_at(fm), cm1(fm), cm2(fm);
	cm_looking_at.caption(L"NORTH");
	cm_looking_at.push_back(L"NORTH");
	cm_looking_at.push_back(L"SOUTH");
	cm_looking_at.push_back(L"EAST");
	cm_looking_at.push_back(L"WEST");
	cm_looking_at.events().selected([&cm_looking_at](const arg_combox &ar_cbx){ gl_data.looking_at = ar_cbx.widget.caption(); });
	//-----------------------------------------------------------------------//
	//---------------------------------LABEL---------------------------------//
	//-----------------------------------------------------------------------//
	
	label lbl1_1(fm);
	lbl1_1.caption(L"     Step font X:");

	label lbl2_1(fm);
	lbl2_1.caption(L"     Step font Y:");

	label lbl3_1(fm);
	lbl3_1.caption(L"          Step Z:");

	label lbl3_2(fm);
	lbl3_2.caption(L"     Ratio axis:");

	label lbl1_2(fm);
	lbl1_2.caption(L"     Step axis X:");
	
	label lbl2_2(fm);
	lbl2_2.caption(L"     Step axis Y:");
	
	label lbl1_3(fm);
	lbl1_3.caption(L"     Looking at:");

	label lbl_pust1(fm, L" "), lbl_pust2(fm, L" "), lbl_pust3(fm, L" "), lbl_pust4(fm, L" ");
	//-----------------------------------------------------------------------//
	//---------------------------------TEXTBOX-------------------------------//
	//-----------------------------------------------------------------------//

	textbox step_x(fm), step_y(fm), step_z(fm), step_axis_x(fm), step_axis_y(fm), ratio_axis(fm);
	step_x.append(L"0.5", true); step_y.append(L"0.5", true); step_z.append(L"0.05", true); 
	step_axis_x.append(L"0.1", true); step_axis_y.append(L"0.1", true);
	ratio_axis.append(L"1.0", true);

	ratio_axis.events().text_changed([&ratio_axis](){
		if (ratio_axis.to_double() > -0.01){
			gl_data.ratio = ratio_axis.to_double();
		}
	});

	step_axis_x.events().text_changed([&step_axis_x](){
		if (step_axis_x.to_double() > 0.01){
			gl_data.step_axis.x = step_axis_x.to_double();
		}
	});
	
	step_axis_y.events().text_changed([&step_axis_y](){
		if (step_axis_y.to_double() > 0.01){
			gl_data.step_axis.y = step_axis_y.to_double();
		}
	});

	step_x.events().text_changed([&step_x](){
		if (step_x.to_double()>0.01){
			gl_data.step_font.x = step_x.to_double();
		}
	});

	step_z.events().text_changed([&step_z](){
		if (step_z.to_double()>0.01){
			gl_data.step_font.z = step_z.to_double();
		}
	});
	
	step_y.events().text_changed([&step_y](){
		if (step_y.to_double()>0.01){
			gl_data.step_font.y = step_y.to_double();
		}
	});
	API::eat_tabstop(step_x, false);
	API::eat_tabstop(step_y, false);
	API::eat_tabstop(step_z, false);
	API::eat_tabstop(step_axis_x, false);
	API::eat_tabstop(step_axis_y, false);
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
		"<weight=50 vert <" 
			"<vert gap=10 lbl_s1>" 
			" < weight=35 vert gap=10 step1>" 
			" <vert gap=10 lbl_s2>"
			"< weight=35 vert gap=10 step2>"
			"<vert gap=10 lbl_s3>"
			"< weight=35 vert gap=10 step3>  > >"
		"<weight=10 vert>"
		"< weight=20  vert  <" 
			"<vert gap=10 lbl>  "
			"<vert gap=10 eve> "
			"<vert gap=10 lbl2>"
			"<vert gap=10 eve2> > >  >"
		"< weight=60 vert < <weight=10 vert> < vert gap=10 btn1> < weight=15> <vert gap=10 btn2> > > <weight=10 vert>"
		">"
	"<weight = 10>");
	//pl.div("<weight = 5> < weight=90% vert < <weight=10 vert <ara> > <> > > <weight = 5>");
	pl.field("eve")  <<  cm_looking_at;// << cm1 << cm2;
	pl.field("eve2") << lbl_pust3 ;// << cm1 << cm2;

	pl.field("lbl")   << lbl1_3;
	pl.field("lbl2")  << lbl_pust2;
	
	pl.field("btn1") << btn_load << lbl_pust1;
	pl.field("btn2") << btn_save << btn_save_pic;

	pl.field("step1") << step_x << step_axis_x;
	pl.field("step2") << step_y << step_axis_y;
	pl.field("step3") << step_z << ratio_axis;

	pl.field("lbl_s1") << lbl1_1 << lbl1_2;
	pl.field("lbl_s2") << lbl2_1 << lbl2_2;
	pl.field("lbl_s3") << lbl3_1 << lbl3_2;
	cout << "55555\n";
	pl.collocate();
	fm.show();
	bar_chart_gl();
	exec();
}