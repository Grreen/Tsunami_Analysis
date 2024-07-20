//#define _CRT_SECURE_NO_WARNINGS 
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
#include <nana/gui/widgets/textbox.hpp>

#define DEFAULT_SETTINGS_PATH L"Default_Settings.ini"

using namespace std;
nana::string p_dat;
nana::string p_barchart;
nana::string p_brick;// double pi = 1;
void fm_calc();
void fm_gist();
void fm_select_area();
void fm_param_gl();
void mn_enabled_check(nana::menubar mn, bool enbl){
	
}

nana::drawing* pMainPicture = NULL;

// Главный метод, который отвечает за создание main-menu окна
// Также тут создаются все сопутсвующие компоненты меню и кнопок
void fm_main()
{
	
	using namespace nana;
	nana::threads::pool pool_bar_chart;
	form fm_main_menu(API::make_center(450, 300), appear::decorate<>());
	fm_main_menu.caption(L"Tsunami analysis: Main menu");
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
	tb_izobata.append(to_wstring(izobata) , 0);
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
	
	tb_time_moments.append(to_wstring(time_moments), 0);
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
	tb_output_time.append(to_wstring(output_time), 0);
	tb_output_time.events().text_changed([&tb_output_time]
	{
		if (tb_output_time.edited()){
			output_time = tb_output_time.to_int();
		}
	});
	//---------------MENU_BAR---------------//
	nana::menubar mn(fm_main_menu);


	mn.push_back(L"File");
	mn.push_back(L"Show");
	mn.push_back(L"Сalculation");
	//---------------MENU_BAR_0_0---------------//
	mn.at(0).append(L"Open DAT file", [&fm_main_menu, &mn](nana::menu::item_proxy& ip) {
		
		filebox fb(0, true);
		fb.add_filter(L"Map dat file", L"*.dat;");
		if (fb())
		{
			nana::string p_dat = fb.file();
			read_dat(p_dat);

			mn.at(0).enabled(1, true);
			mn.at(0).enabled(2, true);
			mn.at(0).enabled(3, true);

			settings.ParseFromFile(DEFAULT_SETTINGS_PATH);
			fm_calc();
		}
	});
	//---------------MENU_BAR_0_1---------------//
	mn.at(0).append(L"Read MaxVis", [&fm_main_menu, &mn](nana::menu::item_proxy& ip) {
		filebox fb(0, true);
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
	mn.at(0).append(L"Read Brick table", [&fm_main_menu](nana::menu::item_proxy& ip) {
		filebox fb(0, true);
		fb.add_filter(L"Map brick file", L"*.txt;");
		if (fb())
		{
			nana::string p_brick = fb.file();
			read_brick_file(p_brick);
			if (NULL != pMainPicture)
				pMainPicture->update();
		}
	});
	//---------------MENU_BAR_0_3---------------//
	mn.at(0).append(L"Read Mareograms", [&fm_main_menu, &mn](nana::menu::item_proxy& ip)
		{
			filebox fb(0, true);
			fb.add_filter(L"Mareograms file", L"*.mrg;");
			if (fb())
			{
				nana::string p_mareograms = fb.file();
				read_data_mareographs(p_mareograms);
				if (NULL != pMainPicture)
					pMainPicture->update();

				mn.at(1).enabled(0, true);
			}
		});
	//---------------MENU_BAR_0_4---------------//
	mn.at(0).append(L"Read Settings", [&fm_main_menu](nana::menu::item_proxy& ip)
		{
			filebox fb(0, true);
			fb.add_filter(L"Settings file", L"*.ini;");
			if (fb())
			{
				settings.ParseFromFile(fb.file());
				if (NULL != pMainPicture)
					pMainPicture->update();
			}
		});

	mn.at(0).enabled(1, false);
	mn.at(0).enabled(2, false);
	mn.at(0).enabled(3, false);

	//---------------MENU_BAR_1_0---------------//
	mn.at(1).append(L"Mareograph's", [&fm_main_menu](nana::menu::item_proxy& ip)
	{
		nana::form fMareograms{ API::make_center(600, 500) };
		fMareograms.caption(L"Mareograph's");

		nana::size oNewSize;

		std::vector<std::unique_ptr<label>> arNames;
		std::vector<std::unique_ptr<label>> arX;
		std::vector<std::unique_ptr<label>> arY;
		std::vector<std::unique_ptr<textbox>> arLatitudes;
		std::vector<std::unique_ptr<textbox>> arLongitudes;

		nana::paint::font oFont(settings.wsFontName.c_str(), settings.GetFontSize(500), true);

		unsigned int unMaxLength = 0;

		for (Mareograph& oMaregram : arMareographs)
		{
			arNames.emplace_back(new label(fMareograms));
			arNames.back()->caption(oMaregram.getLocationNameUTF8());
			arNames.back()->typeface(oFont);

			unMaxLength = max(unMaxLength, oMaregram.getLocationNameUTF8().length());

			arX.emplace_back(new label(fMareograms));
			arX.back()->caption(L"X:");
			arX.back()->typeface(oFont);

			arY.emplace_back(new label(fMareograms));
			arY.back()->caption(L" Y:");
			arY.back()->typeface(oFont);

			arLatitudes.emplace_back(new textbox(fMareograms));
			textbox* p_tb_Latitude = arLatitudes.back().get();
			p_tb_Latitude->append(std::to_wstring(oMaregram.getLatitude()), true);

			API::eat_tabstop(*p_tb_Latitude, false);

			arLongitudes.emplace_back(new textbox(fMareograms));
			textbox* p_tb_Longitude = arLongitudes.back().get();
			p_tb_Longitude->append(std::to_wstring(oMaregram.getLongitude()), true);

			API::eat_tabstop(*p_tb_Longitude, false);
		}

		place pl(fMareograms);

		std::string sArrage = std::to_string(oFont.size() * 1.5);

		oNewSize.width = unMaxLength * oFont.size() + 20 + 34 * oFont.size();
		oNewSize.height = oFont.size() * 2.5 * arMareographs.size();
		fMareograms.size(oNewSize);

		std::string sDiv = "<><weight=90% vert <> <weight=90%  <weight=100% vert <<weight=" + std::to_string(unMaxLength * oFont.size()) + " <arrange = " + sArrage + " gap = 5 vert lb_names >> <<weight=" + std::to_string(2 * oFont.size()) + " <arrange = " + sArrage + " gap = 5 vert lb_x >> <weight = " + std::to_string(10 * oFont.size()) + " <arrange = " + sArrage + " gap = 5 vert tb_lon >> <<weight=" + std::to_string(2 * oFont.size()) + " <arrange = " + sArrage + " gap = 5 vert lb_y >> <weight = " + std::to_string(10 * oFont.size()) + " <arrange = " + sArrage + " gap = 5 vert tb_lat >>> <vert arrange = 20> > > <> <vert bt_save>><>";

		pl.div(sDiv.c_str());

		for (UINT index = 0; index < arMareographs.size(); ++index)
		{
			pl.field("lb_names") << (*arNames[index]);
			pl.field("lb_x") << (*arX[index]);
			pl.field("tb_lon") << (*arLongitudes[index]);
			pl.field("lb_y") << (*arY[index]);
			pl.field("tb_lat") << (*arLatitudes[index]);
		}

		button oSave(fMareograms, L"Save");
		
		oSave.events().click([&arLatitudes, &arLongitudes]()
		{
			for (UINT index = 0; index < arMareographs.size(); ++index)
			{
				arMareographs[index].setLatitude(arLatitudes[index]->to_double());
				arMareographs[index].setLongitude(arLongitudes[index]->to_double());
			}
		});

		pl.field("lb_names") << oSave;

		pl.collocate();
		fMareograms.show();
		nana::exec();
	});
	//---------------MENU_BAR_1_1---------------//
	mn.at(1).append(L"Gistogram 2D", [&fm_main_menu](nana::menu::item_proxy& ip)	
	{
		fm_gist();
	});
	//---------------MENU_BAR_1_2---------------//
	mn.at(1).append(L"Gistogram 3D", [&fm_main_menu](nana::menu::item_proxy& ip)	{
		fm_param_gl();
	});

	
	mn.at(1).enabled(0, false);
	mn.at(1).enabled(1, false);
	mn.at(1).enabled(2, false);

	mn.at(2).append(L"Pause", [&fm_main_menu](nana::menu::item_proxy& ip)	{ std::system("pause"); });
	place pl_menu(fm_main_menu);
	pl_menu.div("<><weight=80% vert <> <weight=80%  <weight=120 vert <<weight=50 <arrange=20 gap=10 vert lb>> <weight=60 <arrange=20 gap=10 vert tb>>> <vert arrange=20> > > <> ><>");
	pl_menu.field("lb") << lbl_izobata << lbl_time << lbl_output_time;
	pl_menu.field("tb") << tb_izobata << tb_time_moments << tb_output_time;

	pl_menu.collocate();
	fm_main_menu.show();
	nana::exec();
}
