#define _USE_MATH_DEFINES 

#include "calculation.h"

#include "Mareograph.h"
#include "interface_nana.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <set>
#include <cmath>
#include <fstream>
#include <iomanip>  
#include <algorithm>
#include <queue>
#include <locale>
#include <codecvt>
#include <string>
//#include <nana/gui/wvl.hpp>
#define M_G 9.81
#define PATH "../../result/mareograms/mareogram_"
#define MAREOGRAPHS "mareographs.mrg"

void find_aqua();
void bamboo();
void swap_v_c_c();
void find_begin();
void checking_mareographs();
void read_data_mareographs(std::string file_path);
long inking_count = 0;
//double *dat_x;
double length_of_equator = 40076;
double time_of_turn_earth = 365 * 24 * 60 * 60 + 6 * 60 * 60 + 9 * 60 + 9;
double speed_of_earth = length_of_equator / time_of_turn_earth;
double Koef_Koriolisa = 0;
int variant_delta_t = 9;
int terr_cnt = 0;     // Count of bricks
int point_cnt = 0;    // Count of points
//int t = 0; int time_moments = 1000;
double** CComp; bool **bool_CComp;
double h_max; // Deeper place (metres)
double delta_x_m, *delta_y_m;
vector< vector<float>> eta;     // array of heigth of wave
vector<float> eta1, eta2, eta3,eta4;// ãðàíèöû
vector<vector<float>> u;       // array of u-speed of wave
vector<vector<float>> v;       // array of v-speed of wave
float** h;       // Bottom relief for calculating
float** maxVis;  //
float** minVis;  //
vector<vector<int>> terr_up;    // Brick1 = 1, Brick2 = 2, ...
float**area; // area for calculation
vector<float> delta_t;
float start_x, end_x;      // Range X (parallels) (degrees)
float start_y, end_y;
int size_y, size_x;
float delta_x, delta_y;

vector<vector<float>> bottom;  // earth relief
int t = 0; 
int time_moments = 170000;
double izobata = -10;
int output_time = 1000;

int hour_line_prev = 0;

set_area_data gist_d;

int use_koef_Sh = 0;
//const double inf = std::numeric_limits<double>().infinity();
const double inf = 23456;

using namespace std;

#define NEED_ALIGN(start, end) { end > 180}

Settings settings;
bricks oBricks;
std::vector<Mareograph> arMareographs;

template <typename T> void output_array(const string& name, T **a){
	fstream file;
	const char * path = name.c_str();;
	file.open(path, std::fstream::out);

	for (int i = size_y - 1; i > -1; i--){
		for (int j = 0; j < size_x; j++)  file << fixed << setprecision(10) << a[i][j] << " ";
		file << "\n";
	}
}
void output_comp(const string& name, int s){
	fstream file;
	//coord a;
	const char * path = name.c_str();
	file.open(path, std::fstream::out);
	for (int i = 0; i < int(f_c_c[s].size()); i++)
	{
		file << f_c_c[s][i].x << " " << f_c_c[s][i].y << "\n";
	}
}
template <typename T> void output_array_2(const string& name, T *a)
{
	fstream file;
	const char * path = name.c_str();;
	file.open(path, std::fstream::out);
	for (int i = 0; i < size_y; i++)
	{
		file << fixed << setprecision(10) << a[i] << "\n";
	}


	file.close();
}

double** create_array(int y, int x)
{
	double **m = new double *[y + 1];
	for (int j = 0; j < y; ++j)
	{
		m[j] = new double[x];
		for (int i = 0; i < x; ++i) m[j][i] = 0.0;
	}
	return m;
}

float** create_arrayF(int y, int x)
{
	float** m = new float* [y + 1];
	for (int j = 0; j < y; ++j)
	{
		m[j] = new float[x];
		for (int i = 0; i < x; ++i) m[j][i] = 0.0;
	}
	return m;
}
//---------------------------------------------------------------------------
// FUNCTION delete_array
//---------------------------------------------------------------------------
// Main: free memory
//---------------------------------------------------------------------------
void delete_array(double **_m)
{
	if (_m == nullptr)
		return;
	for (int j = 0; _m[j] != nullptr; ++j) {
		delete[] _m[j];
		_m[j] = nullptr;
	}
	delete[] _m;
	_m = nullptr;
}

void delete_arrayF(float** _m)
{
	if (_m == nullptr)
		return;
	for (int j = 0; _m[j] != nullptr; ++j) {
		delete[] _m[j];
		_m[j] = nullptr;
	}
	delete[] _m;
	_m = nullptr;
}


//---------------------------------------------------------------------------
// FUNCTION copy_d
//---------------------------------------------------------------------------
// Main: copy src to dest matrix
//---------------------------------------------------------------------------
int copy_array(double **_dest, double **_src, int _y, int _x)
{
	if (_dest == nullptr)
		return 1;
	if (_src == nullptr)
		return 1;
	for (int j = 0; j < _y; j++)
	if (!memcpy(_dest[j], _src[j], _x*sizeof(double)))
		return 1;
	return 0;
}

//---------------------------------------------------------------------------
// FUNCTION swap_d
//---------------------------------------------------------------------------
// Main: swap m1 and m2
//---------------------------------------------------------------------------
void swap_array(double ***_m1, double ***_m2) {
	//   double **tmp = *_m1;
	*_m1 = *_m2;
	//   *_m2 = tmp;
}
void deleteMainArrays() {
	if (h != nullptr) {
		delete_arrayF(h);
		h = nullptr;
	};
	if (maxVis != nullptr) {
		delete_arrayF(maxVis);
		maxVis = nullptr;
	};
	if (minVis != nullptr) {
		delete_arrayF(minVis);
		minVis = nullptr;
	};
	
	/*
	if (point_array != nullptr) {
	delete_array(point_array);
	point_array = nullptr;
	};
	*/
	if (delta_y_m != nullptr) {
		delete delta_y_m;
		delta_y_m = nullptr;
	}

	/*
	if (terr_up != nullptr) {
	for (int j=0; terr_up[j] != nullptr ; j++) {
	delete terr_up[j];
	terr_up[j] = nullptr;
	}
	delete terr_up;
	terr_up = nullptr;
	}
	*/
}
int initMainArrays(int size_y, int size_x)
{
	deleteMainArrays();
	maxVis = create_arrayF(size_y, size_x);
	minVis = create_arrayF(size_y, size_x);

	h = create_arrayF(size_y, size_x);

	area = create_arrayF(size_y, size_x);

	delta_y_m = new double[size_y];
	delta_t = vector<float>(size_y) ;

	if (bottom.empty() ||
		h == nullptr ||
		eta.empty() ||
		u.empty() ||
		v.empty() ||
		minVis == nullptr ||
		maxVis == nullptr ||
		area == nullptr ||
		delta_y_m == nullptr ||
		delta_t.empty() ||
		terr_up.empty())
	{
		return 1;
	};
	return 0;
}
void init_old_arrays() {
	h_max = 10000;
	for (int y = 0; y < size_y; y++) {

		for (int x = 0; x < size_x; x++) {
			if (bottom[y][x] <= 0 && bottom[y][x] >= izobata) {
				h[y][x] = izobata;
				area[y][x] = 0;
			}
			else
			if (bottom[y][x] >= 0) {
				h[y][x] = -10;
				area[y][x] = 0;
			}
			else {
				h[y][x] = bottom[y][x];
				area[y][x] = 1;
				if (bottom[y][x] < -1000)
					area[y][x] = 2;
			}

			eta[y][x] = 0;
			u[y][x] = 0;
			v[y][x] = 0;
			terr_up[y][x] = 0;
			maxVis[y][x] = -5000;
			minVis[y][x] = 5000;

			if (h_max > bottom[y][x])
				h_max = bottom[y][x];
		}
	}
	//çàïîìèíàåì ãðàíèöû
	eta1 = eta[1];
	eta2 = eta[size_y - 3];
	for (int i = 0; i < size_y; i++)
		eta3.push_back(eta[i][1]);
	for (int i = 0; i < size_y; i++)
		eta4.push_back(eta[i][size_x - 3]);

}

void set_delta(int v) {
	delta_x_m = delta_x * M_PI * 6365500 / 180;
	for (int j = 0; j < size_y; ++j) 
	{
		delta_y_m[j] = delta_x_m * cos((start_y + j * delta_y) / 180.0 * M_PI);
		if (v == 0)	delta_t[j] = 1;
		if (v == 1)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 8. / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 2)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 16. / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 3)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 1. / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 4)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 2. / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 5)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 4. / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 6)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 0.9 / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 7)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 1.5 / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 8)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 2.225 / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 9)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 1.357 / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 10)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 1.377 / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		if (v == 11)
			delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 1.337 / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
	}
}



coord get_coord(float x, float y)
{
	int i, j;
	i = (int)((x - start_x) / delta_x);
	j = size_y - 1 - (int)(((y - start_y) / delta_y));

	return { min(max(0, i), size_x - 1), min(max(0, j), size_y - 1) };
}

void read_dat(const nana::string& file_path)
{
	vector <float> depth;
	FILE* infile;
	const wchar_t * path = file_path.c_str();
	infile = _wfopen(path, L"rt");

	if (NULL == infile)
		return;

	scalar minx;
	scalar maxx;
	scalar miny;
	scalar maxy;
	scalar dat_x; //latitude
	scalar dat_y; //longitude
	scalar dat_z; //depth

	set<float> sLat, sLon;
	while (fscanf(infile, "%f %f %f", &dat_x, &dat_y, &dat_z) == 3)
	{
		sLat.insert(dat_x);
		sLon.insert(dat_y);
		depth.push_back(dat_z);
	}

	fclose(infile);

	size_x = sLat.size();
	size_y = sLon.size();
	maxx = *(--sLat.end());
	maxy = *(--sLon.end());
	minx = *(sLat.begin());
	miny = *(sLon.begin());
	delta_x = (maxx - minx) / (size_x -1);
	delta_y = (maxy - miny) / (size_y -1);
	start_x = minx - delta_x / 2.;
	start_y = miny - delta_y / 2.;
	end_x = maxx + delta_x / 2.;
	end_y = maxy + delta_y / 2.;
	sLat.clear();
	sLon.clear();

	for (int i = 0; i < size_y; i++)
	{
		bottom.push_back(vector<float>(size_x));
		for (int j = 0; j < size_x; j++)
			bottom[i][j] = depth[(size_y - i) * size_x + j];
		
	}
	u = bottom;
	v = bottom;
	eta = bottom;
	for (int i = 0; i < size_y+1; i++)
		terr_up.push_back(vector<int>(size_x));

	initMainArrays(size_y, size_x);
	init_old_arrays();
}

void calculation_value_on_boundaries() {
	try {
		
		for (int i = 1; i < size_x; i++) 
		{
			int temp = (int)(i * size_y / size_x);
			v[0][i] = sqrt(abs(-M_G * h[0][i])) * eta[0][i] / (eta[1][i] - h[0][i]);
			v[size_y - 2][i] = sqrt(abs((-M_G * h[size_y - 2][i]))) * eta[size_y - 3][i] / (eta[size_y - 3][i] - h[size_y - 2][i]);
			v[size_y - 1][i] = v[size_y - 2][i];
			eta[0][i] = eta[0][i] - sqrt(abs(-h[0][i] * M_G)) * (delta_t[temp] / delta_y_m[temp]) * (eta[0][i] - eta1[i]);
			eta[size_y - 2][i] = eta[size_y - 2][i] - sqrt(abs((-h[size_y - 2][i] * M_G))) * (delta_t[temp] / delta_y_m[temp]) * (eta[size_y - 2][i] - eta2[i]);
			eta[size_y - 1][i] = eta[size_y - 2][i];
		}
		for (int j = 1; j < size_y; j++) {
			u[j][0] = sqrt((-M_G * h[j][0])) * eta[j][1] / (eta[j][1] - h[j][0]);
			u[j][size_x - 2] = u[j][size_x - 3];
			u[j][size_x - 1] = u[j][size_x - 2];
			eta[j][0] = eta[j][0] - sqrt(abs((-h[j][0] * M_G))) * (delta_t[j] / delta_x_m) * (eta[j][0] - eta3[j]);
			eta[j][size_x - 2] = eta[j][size_x - 2] - sqrt(abs((-h[j][size_x - 2] * M_G))) * (delta_t[j] / delta_x_m) * (eta[j][size_x - 2] - eta4[j]);
			eta[j][size_x - 1] = eta[j][size_x - 2];
		}
	}
	catch (...) {
		
	}
}


void converting_motion_blocks(const int j, const int i) 
{
	for (int b = 0; b < terr_cnt; b++) 
	{
		if (terr_up[j][i] == b + 1 && t > oBricks.brick_calc[b].begin_t)
		{
			for (int d = 0; d < oBricks.brick_calc[b].number_up.size(); d++)
			{
				if (t > oBricks.brick_calc[b].number_up[d].brick_up_start_t && t <= oBricks.brick_calc[b].number_up[d].brick_up_end_t && 0. != oBricks.brick_calc[b].number_up[d].height_up)
				{
					eta[j][i] += oBricks.brick_calc[b].number_up[d].height_up / (oBricks.brick_calc[b].number_up[d].brick_up_end_t - oBricks.brick_calc[b].number_up[d].brick_up_start_t);
					break;
				}
			}
		}
	}
}


void calculation_main_value() 
{
	vector<float> v1mo(v[0]);
	for (int j = 1; j < size_y - 1; ++j) 
	{
		Koef_Koriolisa = 2 * speed_of_earth * cos((start_y + j * delta_y) / 180.0 * M_PI);
		float uo1m = u[j][0];
		for (int i = 1; i < size_x - 1; ++i) 
		{
			float uoo = u[j][i], voo = v[j][i];
			float u1po = u[j + 1][i], vo1p = v[j][i + 1];
			float u01p = u[j][i + 1], v1po = v[j + 1][i];
			if (area[j][i]) 
			{
				try {
					double temp;
					if (i < size_x - 2 && j < size_y - 2) {
						eta[j][i] = eta[j][i]
							+ delta_t[j] * (0.5 / delta_x_m * (u1po * (h[j + 2][i] + h[j + 1][i])
							- uoo * (h[j + 1][i] + h[j][i]))
							+ 0.5 / delta_y_m[j] * (vo1p * (h[j][i + 2] + h[j][i + 1])
							- voo * (h[j][i + 1] + h[j][i])));
						if (maxVis[j][i] < eta[j][i]) {
							maxVis[j][i] = eta[j][i];
						}
						if (minVis[j][i] > eta[j][i]) {
							minVis[j][i] = eta[j][i];
						}

					}
					if (i > 0 && j > 0) {
						double k01 = use_koef_Sh * 2 * 0.026 * 9.81 * sqrt(abs(uoo * uoo + voo * voo)) / ((eta[j][i] + h[j][i]));
						u[j][i] = uoo - (M_G / (2 * delta_x_m)*(eta[j][i] - eta[j - 1][i])
							- (Koef_Koriolisa)*voo - k01* uoo) * delta_t[j];
						v[j][i] = voo - (M_G / (2 * delta_y_m[j])*(eta[j][i] - eta[j][i - 1])
							+ (Koef_Koriolisa)*uoo - k01* voo) * delta_t[j];
					}

					converting_motion_blocks(j, i);

					if (eta[j][i] > 15) { 
						int rand_ = rand() % 50;
						eta[j][i] = 4 - double(rand_) / 100;
						
						u[j][i] = 0.25 * (uo1m + u01p);
						v[j][i] = 0.25 * (v1mo[i] + v1po);
					}
					if (eta[j][i] < -15) {
						int rand_ = rand() % 200;
						eta[j][i] = 12 + double(rand_) / 100;
						u[j][i] = 0.25 * (uo1m + u01p);
						v[j][i] = 0.25 * (v1mo[i] + v1po);
					}
				}
				catch (...)
				{

				}
			}

			uo1m = u[j][i];
		}
		 v1mo=v[j];
		
	}
	v1mo.clear();
	calculation_value_on_boundaries();
	eta1 = eta[1];
	eta2 = eta[size_y - 3];
	for (int i = 0; i < size_y; i++)
		eta3[i]=(eta[i][1]);
	for (int i = 0; i < size_y; i++)
		eta4[i] = (eta[i][size_x - 3]);
}

bool isInside(const vector<coord>& polygon, int x, int y) 
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

void fill_polygon(vector<vector<int>>& terr, int v, const vector<coord>& g)
{
	if (g.size() < 3 || terr.empty())
		return;

	unsigned int minX = size_x - 1;
	unsigned int maxX = 0;
	unsigned int minY = size_y - 1;
	unsigned int maxY = 0;

	for (const coord& oCoord : g)
	{
		minX = min(oCoord.x, minX);
		maxX = max(oCoord.x, maxX);
		minY = min(oCoord.y, minY);
		maxY = max(oCoord.y, maxY);
	}

	for (unsigned int x = minX; x <= maxX; ++x)
	{
		for (unsigned int y = minY; y <= maxY; ++y)
		{
			if (isInside(g, x, y))
				terr[y][x] = v;
		}
	}
}

void get_closest()
{

}

bool cmp(brick_point& p1, brick_point& p2) 
{
	return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
}


double cross_product(const brick_point& O, const brick_point& A, const brick_point& B) {
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

std::vector<brick_point> convexHull(std::vector<brick_point>& points) {
	if (points.size() <= 3) return points;

	std::sort(points.begin(), points.end(), cmp);

	std::vector<brick_point> upperHull;
	for (const auto& p : points) {
		while (upperHull.size() >= 2 && cross_product(upperHull[upperHull.size() - 2], upperHull.back(), p) <= 0) 
			upperHull.pop_back();
		upperHull.push_back(p);
	}

	std::vector<brick_point> lowerHull;
	for (auto it = points.rbegin(); it != points.rend(); ++it) {
		const auto& p = *it;
		while (lowerHull.size() >= 2 && cross_product(lowerHull[lowerHull.size() - 2], lowerHull.back(), p) <= 0)
			lowerHull.pop_back();

		lowerHull.push_back(p);
	}

	upperHull.pop_back();
	lowerHull.pop_back();
	upperHull.insert(upperHull.end(), lowerHull.begin(), lowerHull.end());

	return upperHull;
}
void read_brick_file(const nana::string& file_path)
{
	FILE* infile;
	infile = _wfopen(file_path.c_str(), L"rt");

	if (!infile)
		return;

	int count_angles = 0;
	int count_brick_up = 0;
	scalar x = 0., y = 0.;
	scalar tmp_height_up = 0.;
	scalar tmp_brick_up_t = 0.;

	fscanf(infile, "%d", &terr_cnt);

	oBricks.Clear();

	for (int i = 0; i < terr_cnt; i++)
	{
		brick_data oBrick;
		fscanf(infile, "%d", &count_angles);
		for (int j = 0; j < count_angles; j++)
		{
			fscanf(infile, "%f %f", &x, &y);
			oBrick.points.push_back({ x, y });
		}

		oBrick.points = convexHull(oBrick.points);

		fscanf(infile, "%f", &oBrick.begin_t);
		fscanf(infile, "%d", &count_brick_up);

		for (int j = 0; j < count_brick_up; j++)
		{
			fscanf(infile, "%f %f", &tmp_brick_up_t, &tmp_height_up);
			scalar dStartTime = (oBrick.number_up.empty()) ? oBrick.begin_t : oBrick.number_up.back().brick_up_end_t;
			oBrick.number_up.push_back({ dStartTime, dStartTime + tmp_brick_up_t, tmp_height_up });
		}

		oBricks.AddBrick(oBrick);

		vector<coord> g(count_angles);

		for (int j = 0; j < count_angles; j++)
			g[j] = get_coord(oBrick.points[j].x, oBrick.points[j].y);

		fill_polygon(terr_up, i + 1, g);
	}

	fclose(infile);
	//freopen("CON", "w", stdout);
}
void read_data_mareographs(const nana::string& file_path)
{
	fstream file;
	const wchar_t* path = file_path.c_str();;
	file.open(path, std::fstream::in);

	arMareographs.clear();

	int count;
	file >> count;
	for (int i = 0; i < count; i++) 
	{
		string location;
		double x, y;
		file >> location >> x >> y;
		Mareograph m(y, x, 25, location);
		coord point = get_coord(x, y);

		std::cout << location << " H: " << bottom[point.y][point.x] << std::endl;

		m.setIndexX(point.x);
		m.setIndexY(point.y);
		arMareographs.push_back(m);
	}
}

void gettime(const int tt, int& h, int& m, int& s) {
	int i;
	for (i = 1; i < 60; i++) {
		if (tt - i * 3600 < 0) {
			break;
		}
	}
	h = i - 1;
	for (i = 1; i < 60; i++) {
		if (tt - h * 3600 - i * 60 < 0)
			break;
	}
	m = i - 1;
	s = tt - h * 3600 - m * 60;
}

/*vector <vector <coord>> v_c_c; //= new vector <coord>[MAXN];// [MAXN];
vector <vector <coord>> f_c_c;
vector<coord> buf;
vector <int> **fp;*/

void run_calc()
{

	
	calculation_main_value();
	

	/*int h, m, s;
	gettime(delta_t[0] * t, h, m, s); //tak bilo
	if (h < 5 && h > hour_line_prev) {
	hour_line_prev++;
	find_inking();
	}*/
	//t++;

}

void convert_to_dat(const nana::string& path_file)
{
	//string name;
	string a;
	char b[201];
	//cout << "file name: ";
	//cin >> name;
	const wchar_t *path = path_file.c_str();
	FILE * in;
	in = _wfopen(path, L"r");
	FILE * out;
	
	//name += "dat";
	//const char *path2 = name.c_str();
	out = fopen("out.dat", "w");

	while (fgets(b, sizeof(b), in))
	{
		for (int i = 0; b[i] != '\0'; i++){
			if (b[i] == ',') b[i] = '.';

		}
		fputs(b, out);
	}
	fclose(in);
	fclose(out);
}

void checking_mareographs()
{
	if (arMareographs.size() == 0) return;
	for (int i = 0; i < arMareographs.size(); i++)
		arMareographs[i].pushHeight(eta[arMareographs[i].getIndexY()][arMareographs[i].getIndexX()]);
}
void saveMareographs()
{
	for (int i = 0; i < arMareographs.size(); i++)
		arMareographs[i].writeToFileMareograph(settings.wsPathToSave + L"mareograms/mareogram" + std::to_wstring(i));
}

std::vector<real_point> GetCoordsMareographs()
{
	std::vector<real_point> arCoords(arMareographs.size());

	for (unsigned int i = 0; i < arMareographs.size(); ++i)
		arCoords[i] = { arMareographs[i].getLongitude(), arMareographs[i].getLatitude() };

	return arCoords;
}

double convertToDouble(const std::wstring& sValue) 
{
	double dResult;
	std::wistringstream iss(sValue);

	if (!(iss >> dResult))
		return 0;
	
	return dResult;
}

unsigned convertToUnsigned(const std::wstring& sValue)
{
	unsigned uResult;

	std::wistringstream iss(sValue);

	if (!(iss >> uResult))
		return 0;

	return uResult;
}

Settings::Settings()
	: wsPathToSave(DEFAULT_PATH_TO_SAVE),
	  wsFontName(DEFAULT_FONT_NAME),
	  dFontSize(DEFAULT_FONT_SIZE),
	  bFrontPepcentage(false),
	  bUseSteps(false),
	  dStepX(DEFAULT_STEP_X),
	  dStepFontX(DEFAULT_STEP_FONT_X),
	  dStepY(DEFAULT_STEP_Y),
	  dStepFontY(DEFAULT_STEP_FONT_Y),
	  uMaxSize(DEFAULT_MAX_SIZE),
	  bSaveMaregrams(true),
	  bSaveMaxHeight(true),
	  bDrawDirectionWave(false),
	  bDrawGrid(false)
{
	// Land
	height.mMap.insert({ 6000, { 214, 214, 214 } });
	height.mMap.insert({ 1500, { 121, 83,  83  } });
	height.mMap.insert({ 800,  { 160, 55,  0   } });
	height.mMap.insert({ 400,  { 201, 180, 102 } });
	height.mMap.insert({ 0,    { 0,   91,  65  } });

	// Bottom
	height.mMap.insert({ -1,    { 0, 255, 255 } });
	height.mMap.insert({ -1000, { 0, 204, 255 } });
	height.mMap.insert({ -2000, { 0, 153, 255 } });
	height.mMap.insert({ -3000, { 0, 102, 255 } });
	height.mMap.insert({ -4000, { 0, 50,  255 } });
	height.mMap.insert({ -5000, { 0, 0,   255 } });
	height.mMap.insert({ -6000, { 0, 0,   204 } });
	height.mMap.insert({ -7000, { 0, 0,   153 } });
	height.mMap.insert({ -8000, { 0, 0,   102 } });

	// Front
	front.mMap.insert({ -10, { 0,   0,   102 } });
	front.mMap.insert({ -0,  { 100, 160, 250 } });
	front.mMap.insert({ 0.01,{ 254, 201, 0   } });
	front.mMap.insert({ 3,   { 255, 146, 1   } });
	front.mMap.insert({ 6,   { 254, 36,  0   } });
	front.mMap.insert({ 9,   { 233, 1,   1   } });
	front.mMap.insert({ 12,  { 166, 0,   0   } });
	front.mMap.insert({ 15,  { 100, 1,   1   } });

	// MaxHeight
	maxHeight.mMap.insert({ 0,   { 255, 255, 255 } });
	maxHeight.mMap.insert({ 0.1, { 0,   0,   255 } });
	maxHeight.mMap.insert({ 0.2, { 153, 255, 255 } });
	maxHeight.mMap.insert({ 0.4, { 100, 255, 0   } });
	maxHeight.mMap.insert({ 0.5, { 255, 255, 0   } });
	maxHeight.mMap.insert({ 0.6, { 255, 100, 50  } });
	maxHeight.mMap.insert({ 0.8, { 255, 30,  15  } });
	maxHeight.mMap.insert({ 0.9, { 250, 50,  0   } });
	maxHeight.mMap.insert({ 1,   { 200, 100, 200 } });
	maxHeight.mMap.insert({ 3.5, { 165, 70,  190 } });
	maxHeight.mMap.insert({ 6,   { 130, 40,  180 } });
	maxHeight.mMap.insert({ 11,  { 100, 25,  150 } });
	maxHeight.mMap.insert({ 16,  { 75,  10,  125 } });
	maxHeight.mMap.insert({ 21,  { 50,  0,   100 } });
}

void Settings::ParseFromFile(const nana::string& sFiePath)
{
	FILE* file = _wfopen(sFiePath.c_str(), L"r");

	if (!file)
		return;

	constexpr size_t KEY_SIZE = 32;
	constexpr size_t VALUE_SIZE = 512;
	constexpr size_t STRING_SIZE = (KEY_SIZE + VALUE_SIZE);

	wchar_t key[KEY_SIZE];
	wchar_t value[VALUE_SIZE];
	wchar_t fullString[STRING_SIZE];

	while (fgetws(fullString, STRING_SIZE, file))
	{
		const std::wstring wsFormat = L"%" + std::to_wstring(KEY_SIZE) + L"[^=]=\"%" + std::to_wstring(VALUE_SIZE) + L"[^\"]\"";
		if (2 == swscanf(fullString, wsFormat.c_str(), key, value))
		{
			if (0 == std::wcscmp(L"path_to_save", key))
				wsPathToSave = value;
			else if (0 == std::wcscmp(L"font_name", key))
				wsFontName = value;
			else if (0 == std::wcscmp(L"font_size", key))
			{
				double dTempFontSize = convertToDouble(value);
				bFrontPepcentage = std::find(value, value + VALUE_SIZE - 1, L'%');
				if (bFrontPepcentage)
					dFontSize = dTempFontSize / 100;
				else if (dTempFontSize > 1.)
					dFontSize = dTempFontSize;
			}
			else if (0 == std::wcscmp(L"save_step_x", key))
			{
				double dTempStepX = convertToDouble(value);
				if (dTempStepX > DBL_EPSILON)
				{
					dStepX = dTempStepX;
					bUseSteps = true;
				}
			}
			else if (0 == std::wcscmp(L"save_step_font_x", key))
			{
				double dTempStepFontX = convertToDouble(value);
				if (dTempStepFontX > DBL_EPSILON)
				{
					dStepFontX = dTempStepFontX;
					bUseSteps = true;
				}
			}
			else if (0 == std::wcscmp(L"save_step_y", key))
			{
				double dTempStepY = convertToDouble(value);
				if (dTempStepY > DBL_EPSILON)
				{
					dStepY = dTempStepY;
					bUseSteps = true;
				}
			}
			else if (0 == std::wcscmp(L"save_step_font_y", key))
			{
				double dTempStepFontY = convertToDouble(value);
				if (dTempStepFontY > DBL_EPSILON)
				{
					dStepFontY = dTempStepFontY;
					bUseSteps = true;
				}
			}
			else if (0 == std::wcscmp(L"max_size", key))
			{
				unsigned uTempMaxSize = convertToUnsigned(value);
				if (0 < uTempMaxSize)
					uMaxSize = uTempMaxSize;
			}
			else if (0 == std::wcscmp(L"colorbar", key))
			{
				wchar_t colorbarType[16];
				wchar_t colorbarValues[512];
				if (2 == swscanf(value, L"%16s %512[^\n]", colorbarType, colorbarValues))
				{
					colorBar* pSrcColorBar = NULL;

					if (0 == std::wcscmp(L"height", colorbarType))
						pSrcColorBar = &height;
					else if (0 == std::wcscmp(L"front", colorbarType))
						pSrcColorBar = &front;
					else if (0 == std::wcscmp(L"maxheight", colorbarType))
						pSrcColorBar = &maxHeight;
					else
						continue;

					pSrcColorBar->Clear();

					if (0 == std::wcscmp(L"height", colorbarValues))
					{
						*pSrcColorBar = height;
						continue;
					}
					else if (0 == std::wcscmp(L"front", colorbarValues))
					{
						*pSrcColorBar = front;
						continue;
					}
					else if (0 == std::wcscmp(L"maxheight", colorbarValues))
					{
						*pSrcColorBar = height;
						continue;
					}

					wchar_t* str = colorbarValues;
					int offset = 0;

					if (1 == swscanf(str, L"tick %lf%n", &(pSrcColorBar->dTick), &offset))
						str += offset + 1;

					double dValue;
					unsigned char chR, chG, chB;
					while (4 == swscanf(str, L"%lf (%hhu,%hhu,%hhu)%n", &dValue, &chR, &chG, &chB, &offset))
					{
						pSrcColorBar->mMap.insert({ dValue, { chR , chG, chB } });
						str += offset;
					}
				}
			}
			else if (0 == std::wcscmp(L"save_maxheight", key))
			{
				if (0 == std::wcscmp(L"true", value))
					bSaveMaxHeight = true;
				else if (0 == std::wcscmp(L"false", value))
					bSaveMaxHeight = false;
			}
			else if (0 == std::wcscmp(L"save_mareograms", key))
			{
				if (0 == std::wcscmp(L"true", value))
					bSaveMaregrams = true;
				else if (0 == std::wcscmp(L"false", value))
					bSaveMaregrams = false;
			}
			else if (0 == std::wcscmp(L"draw_direction_wave", key))
			{
				if (0 == std::wcscmp(L"true", value))
					bDrawDirectionWave = true;
				else if (0 == std::wcscmp(L"false", value))
					bDrawDirectionWave = false;
			}
			else if (0 == std::wcscmp(L"draw_grid", key))
			{
				if (0 == std::wcscmp(L"true", value))
					bDrawGrid = true;
				else if (0 == std::wcscmp(L"false", value))
					bDrawGrid = false;
			}
			else if (0 == std::wcscmp(L"language", key))
			{
				if (0 == std::wcscmp(L"RU", value))
					eLanguage = LANGUAGE_RU;
				else if (0 == std::wcscmp(L"EN", value))
					eLanguage = LANGUAGE_EN;
			}	
		}
	}
}

double Settings::GetFontSize(double dHeight) const
{
	if (bFrontPepcentage && dHeight > DBL_EPSILON)
		return dHeight * dFontSize;

	return dFontSize;
}

/*	for (int i = 0; i < maregraphs.size(); i++){
eta[maregraphs[i].getIndexY()][maregraphs[i].getIndexX()] = 2000;
eta[maregraphs[i].getIndexY() + 1][maregraphs[i].getIndexX() - 1] = 2000;
eta[maregraphs[i].getIndexY() - 1][maregraphs[i].getIndexX()] = 2000;
eta[maregraphs[i].getIndexY() + 1][maregraphs[i].getIndexX()] = 2000;
eta[maregraphs[i].getIndexY()][maregraphs[i].getIndexX() - 1] = 2000;
eta[maregraphs[i].getIndexY()][maregraphs[i].getIndexX() + 1] = 2000;
eta[maregraphs[i].getIndexY() - 1][maregraphs[i].getIndexX() + 1] = 2000;
eta[maregraphs[i].getIndexY() + 1][maregraphs[i].getIndexX() + 1] = 2000;
eta[maregraphs[i].getIndexY() - 1][maregraphs[i].getIndexX() - 1] = 2000;
eta[maregraphs[i].getIndexY()][maregraphs[i].getIndexX()] = 2000;
eta[maregraphs[i].getIndexY() + 2][maregraphs[i].getIndexX() - 2] = 2000;
eta[maregraphs[i].getIndexY() - 2][maregraphs[i].getIndexX()] = 2000;
eta[maregraphs[i].getIndexY() + 2][maregraphs[i].getIndexX()] = 2000;
eta[maregraphs[i].getIndexY()][maregraphs[i].getIndexX() - 2] = 2000;
eta[maregraphs[i].getIndexY()][maregraphs[i].getIndexX() + 2] = 2000;
eta[maregraphs[i].getIndexY() - 2][maregraphs[i].getIndexX() + 2] = 2000;
eta[maregraphs[i].getIndexY() + 2][maregraphs[i].getIndexX() + 2] = 2000;
eta[maregraphs[i].getIndexY() - 2][maregraphs[i].getIndexX() - 2] = 2000;
}*/