//#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include "declaration.h"
#include "interface_nana.h"
#include "interface_GL.h"
#include "calculation.h"
#include "displaying.h"
//#include "physics.h"
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <nana/gui/wvl.hpp>
#include <cstdlib>

#include <vector>
#include <cmath>

using namespace std;


/*int main()
{
form_main();
}*/
//
void init_files(){
	//setlocale(LC_ALL, "Russian");
	nana::string  a, b, c;
	std::wfstream load;
	std::locale utf8_locale(std::locale(), new std::codecvt_utf8 <wchar_t>());
	load.imbue(utf8_locale);
	load.open(L"data.INI", std::wfstream::in);
	
	load >> p_dat >> p_brick >> p_barchart >> a>> b >> c;
	load.close();
	const wchar_t * iz= a.c_str();
	izobata = _wtof(iz);
	const wchar_t * tm = b.c_str();
	time_moments = _wtoi(tm);
	const wchar_t * ot = c.c_str();
	output_time = _wtoi(ot);
	
	//load << time_moments;
	//load << izobata;
	//load << output_time;
	//setlocale(LC_ALL, "Russian");
	//std::wcout << L"ûôûsss" << a << L"\n" << b << endl << c << endl;
	//cout << izobata << endl << time_moments << endl << output_time << endl;
}

int main()
{
	//fm_param_gl();
	//setlocale(LC_ALL, "Russian");
	//init_files();
//	fm_param_nana();
	//fm_set_area();
	fm_main();
	//wcout << a << endl;
	//wprintf_s(L"%s\n", n_dat);
	//system("pause");

	return 0;
}
/*void init_files(){
	std::wfstream load;
	std::locale utf8_locale(std::locale(), new std::codecvt_utf8 <wchar_t>());
	load.imbue(utf8_locale);
	load.open(L"data.INI", std::wfstream::in);
	load << L"C:\\Users\\Òþíòÿåâ\\Documents\\Work\\my_program\\tsunami_analysis\\tsunami_analysis\\work.dat\n";
	load << L"C:\\Users\\Òþíòÿåâ\\Documents\\Work\\my_program\\tsunami_analysis\\tsunami_analysis\\brick2.txt\n";
	load << L"C:\\tmp\\MVis_main.mtx\n-5\n25000\n100";
	*/