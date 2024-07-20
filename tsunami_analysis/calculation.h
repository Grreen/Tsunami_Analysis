#ifndef CALCULATION_H
#define CALCULATION_H
#include <iostream>
#include <vector>
#include <nana/gui/wvl.hpp>
using namespace std;

extern int variant_delta_t;
extern int size_x, size_y;
extern int output_time, time_moments, t;
extern double izobata;
extern double** bottom;
extern double** eta;
extern double** maxVis;
extern double** minVis;
extern double** CComp; 
extern bool **bool_CComp;
extern double start_x, start_y, end_x, end_y, delta_x, delta_y;

enum EarthqakeEvent {
	DEFAULT,
	PACIFIC_CUT,
	MEXICO_BIG
};

struct coord
{
	int x, y;
};

struct set_area_data
{
	coord l_l;//lower left  corner
	//coord l_r;//lower right  corner
	//coord t_l;//top left corner
	coord t_r;//top right corner
	double start_x, start_y, end_x, end_y;
	int size_x, size_y;
};

struct mareogram{
	//real_point position;
	std::wstring name;
	int time_step;
	double* h;
};
extern vector <double> average;
extern vector <double> max_from_components;
extern set_area_data gist_d;
extern vector <vector <coord>> v_c_c;
extern vector <vector <coord>> f_c_c;
//void dfs(coord start);
coord get_coord(double x, double y);
double** create_array(int y, int x);
double getDistantionOfCenterMareographs(double x, double y, double maxDist);
void checking_mareographs();
void saveMareographs();
void convert_to_dat(const nana::string& path_file);
void find_comps();
void read_array(const nana::string& file_path);
void read_dat(const nana::string& file_path);
void read_brick_file(const nana::string& file_path);
void set_delta(int v);
void run_calc();
double max_h(int s);
#endif