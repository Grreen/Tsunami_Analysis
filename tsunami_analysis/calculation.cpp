#define _USE_MATH_DEFINES 

#include "calculation.h"

#include "Mareograph.h"
#include "interface_nana.h"
#include <cstdio>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>  
#include <algorithm>
#include <locale>
#include <codecvt>
#include <string>

//#include <nana/gui/wvl.hpp>
#define M_G 9.81
#define PATH "export_files_diplom\\Mareograms\\"//"WORK DECEMBER\\2017MINI2\\Mareograms2\\"//"WORK SEPTEMBER\\2017MINI\\Mareograms\\"
typedef  double scalar;
void find_aqua();
void bamboo();
void swap_v_c_c();
void find_begin();
//void checking_mareographs();
void read_data_mareographs(const std::string& file_path);
long inking_count = 0;
//double *dat_x;
constexpr double length_of_equator = 40076;
constexpr double time_of_turn_earth = 365 * 24 * 60 * 60 + 6 * 60 * 60 + 9 * 60 + 9;
constexpr double speed_of_earth = length_of_equator / time_of_turn_earth;
double koef_Koriolisa = 0;
double Koef_Koriolisa = 0;
int variant_delta_t = 1;
int terr_cnt = 0;     // Count of bricks
int point_cnt = 0;    // Count of points
//int t = 0; int time_moments = 1000;
double** CComp; bool **bool_CComp;
double h_max; // Deeper place (metres)
double delta_x_m, *delta_y_m;
double** eta;     // array of heigth of wave
double** eta_old; // previous value of eta
double** u;       // array of u-speed of wave
double** u_old;   // previous value of u
double** v;       // array of v-speed of wave
double** v_old;   // previous value of v
double** bottom;  // earth relief
double** h;       // Bottom relief for calculating
double** maxVis;  //
double** minVis;  //
int **terr_up;    // Brick1 = 1, Brick2 = 2, ...
int *terr_number_interval_time;    // Brick1 = 1, Brick2 = 2, ...
double **t_h_v_up;
double **terr_points;    // Brick's corners coordinates
double **point_points;   // Point's corners coordinates
double **point_Sputnik;   // Point's corners coordinates
double **sputnik_positions; // Current position of sputniks;
double **a; // for fill_pBitmap
double **area; // area for calculation
double *inking; // inking
double *delta_t;
double start_x, end_x;      // Range X (parallels) (degrees)
double start_y, end_y;
int size_y, size_x;
double delta_x, delta_y;

int t = 0;
int time_moments = 1000;
double izobata = -5;
int output_time = 1;

int hour_line_prev = 0;
double** point_array;

set_area_data gist_d;

int use_koef_Sh = 0;
//const double inf = std::numeric_limits<double>().infinity();
const double inf = 23456;

vector <Mareograph> mareographs;
using namespace std;

struct brick_point
{
	scalar x;
	scalar y;
	brick_point(scalar X, scalar Y) : x(X), y(Y) {};
};
struct brick_up
{
	int brick_up_t;
	scalar height_up;
	brick_up(int a, scalar b) : brick_up_t(a), height_up(b) {};
};
struct brick_data
{
	scalar begin_t;
	vector <brick_point> points;
	vector <brick_up> number_up;
};

brick_data *brick_calc;

template <typename T> void output_array(string name, T **a) {
	fstream file;
	file.open(name, std::fstream::out);

	for (int i = size_y - 1; i > -1; i--) {
		for (int j = 0; j < size_x; j++)  file << a[i][j] << " ";
			//fixed << setprecision(10) <<
		file << "\n";
	}
}
void output_comp(const std::string& name, int s) {
	fstream file;
	//coord a;
	file.open(name, std::fstream::out);
	for (const auto& i : f_c_c[s])
	{
		file << i.x << " " << i.y << "\n";
	}
}
template <typename T> void output_array_2(string name, T *a)
{
	fstream file;
	const char * path = name.c_str();;
	file.open(path, std::fstream::out);
	for (int i = 0; i < size_y; i++)
	{
		file << a[i] << "\n";
	}


	file.close();
}

double** create_array(int y, int x)
{
	const auto m = new double *[y + 1];
	for (int j = 0; j < y; ++j)
	{
		m[j] = new double[x];
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
	if (eta != nullptr) {
		delete_array(eta);
		eta = nullptr;
	};
	if (eta_old != nullptr) {
		delete_array(eta_old);
		eta_old = nullptr;
	};
	if (u != nullptr) {
		delete_array(u);
		u = nullptr;
	};
	if (u_old != nullptr) {
		delete_array(u_old);
		u_old = nullptr;
	};
	if (v != nullptr) {
		delete_array(v);
		v = nullptr;
	};
	if (v_old != nullptr) {
		delete_array(v_old);
		v_old = nullptr;
	};
	if (h != nullptr) {
		delete_array(h);
		h = nullptr;
	};
	if (maxVis != nullptr) {
		delete_array(maxVis);
		maxVis = nullptr;
	};
	if (minVis != nullptr) {
		delete_array(minVis);
		minVis = nullptr;
	};
	/*
	if (point_array != nullptr) {
	delete_array(point_array);
	point_array = nullptr;
	};
	*/
	if (a != nullptr) {
		delete_array(a);
		a = nullptr;
	};
	if (delta_y_m != nullptr) {
		delete delta_y_m;
		delta_y_m = nullptr;
	}
	if (delta_t != nullptr) {
		delete delta_t;
		delta_t = nullptr;
	}
	if (terr_points != nullptr) {
		delete terr_points;
		terr_points = nullptr;
	}
	if (sputnik_positions != nullptr) {
		delete sputnik_positions;
		sputnik_positions = nullptr;
	}
	if (t_h_v_up != nullptr) {
		delete t_h_v_up;
		t_h_v_up = nullptr;
	}
	if (inking != nullptr) {
		delete inking;
		inking = nullptr;
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
	if (terr_number_interval_time != nullptr) {
		delete terr_number_interval_time;
		terr_number_interval_time = nullptr;
	}
}
int initMainArrays(int size_y, int size_x) {
	deleteMainArrays();
	a = create_array(size_y, size_x);
	maxVis = create_array(size_y, size_x);
	minVis = create_array(size_y, size_x);
	//CComp = create_array((size_y ), (size_x ));
	//bool_CComp = create_array_bool((size_y ), (size_x ));
	//traces("minVis is initialized...");
	//Application->ProcessMessages();
	eta = create_array(size_y, size_x);
	//traces("eta is initialized...");
	//Application->ProcessMessages();
	eta_old = create_array(size_y, size_x);
	//traces("eta_old is initialized...");
	//Application->ProcessMessages();
	u = create_array(size_y, size_x);

	u_old = create_array(size_y, size_x);

	v = create_array(size_y, size_x);

	v_old = create_array(size_y, size_x);

	h = create_array(size_y, size_x);

	point_array = create_array(size_y, size_x);

	area = create_array(size_y, size_x);

	delta_y_m = new double[size_y];
	delta_t = new double[size_y];
	terr_points = create_array(8, terr_cnt);
	inking = new double[size_y * 100];
	t_h_v_up = create_array(70, terr_cnt);
	sputnik_positions = create_array(2, 30); // 30 - maximum number of sputniks
	terr_number_interval_time = new int[30];
	terr_up = new int*[size_y + 1];
	for (int j = 0; j < size_y; j++) {
		terr_up[j] = new int[size_x];
	}
	terr_up[size_y] = nullptr;

	if (bottom == nullptr ||
		h == nullptr ||
		eta == nullptr ||
		eta_old == nullptr ||
		u == nullptr ||
		v == nullptr ||
		u_old == nullptr ||
		v_old == nullptr ||
		minVis == nullptr ||
		maxVis == nullptr ||
		a == nullptr ||
		area == nullptr ||
		point_array == nullptr ||
		terr_points == nullptr ||
		t_h_v_up == nullptr ||
		sputnik_positions == nullptr)
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
					h[y][x] = -5;
					area[y][x] = 0;
				}
				else {
					h[y][x] = bottom[y][x];
					area[y][x] = 1;
					if (bottom[y][x] < -1000)
						area[y][x] = 2;
				}

				eta_old[y][x] = 0;
				u_old[y][x] = 0;
				v_old[y][x] = 0;
				terr_up[y][x] = 0;
				maxVis[y][x] = -5000;
				minVis[y][x] = 5000;

				if (h_max > bottom[y][x])
					h_max = bottom[y][x];
		}
		
	}
	//output_array("area", area);
}

void set_delta(int v) {
	delta_x_m = delta_x * M_PI * 6365500 / 180;
	for (int j = 0; j < size_y; ++j) {
		delta_y_m[j] = delta_x_m * cos((start_y + j * delta_y) / 180.0 * M_PI);
		delta_t[j] = 1;
		//if (v == 0)	delta_t[j] = 1;
		//if (v == 1) {
			if (delta_t[j] > sqrt(delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m) / sqrt(2 * M_G * 3000))
				delta_t[j] = sqrt(delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m) / sqrt(2 * M_G * 3000);
		//}
		//delta_t[j] = //(abs(delta_y_m[j] * delta_x_m) / 8. / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
		// TODO - delta_t

		//if (v == 2)
		//	delta_t[j] = (abs(delta_y_m[j] * delta_x_m) / 16. / sqrt((M_G * abs(h_max) * (delta_y_m[j] * delta_y_m[j] + delta_x_m * delta_x_m))));
	}
}



coord get_coord(double x, double y)
{
	return{ (int)((x - start_x) / delta_x), size_y - 1 - (int)(((y - start_y) / delta_y)) };
}

void read_dat(const nana::string& file_path)
{
	vector <double> longitude;//y
	vector <double> latitude;//x
	vector <double> depth;
	//std::locale utf8_locale(std::locale(), new std::codecvt_utf8 <wchar_t>());
	const wchar_t * path = file_path.c_str();
	FILE* infile = _wfopen(path, L"rt");
	if (!infile)
		abort();

	scalar dat_x;// latitude
	scalar dat_y; //longitude
	scalar dat_z; // depth

	while (fscanf(infile, "%lf%lf%lf", &dat_x, &dat_y, &dat_z) == 3)
	{
		latitude.push_back(dat_x);
		longitude.push_back(dat_y);
		depth.push_back(dat_z);
	}

	fclose(infile);

	vector <double> lat_uniq(latitude), long_uniq(longitude);
	std::sort(lat_uniq.begin(), lat_uniq.end());
	std::sort(long_uniq.begin(), long_uniq.end());
	size_x = (std::unique(lat_uniq.begin(), lat_uniq.end()) - lat_uniq.begin());
	size_y = (std::unique(long_uniq.begin(), long_uniq.end()) - long_uniq.begin());
	const scalar maxx = *std::max_element(latitude.begin(), latitude.end());
	const scalar maxy = *std::max_element(longitude.begin(), longitude.end());
	const scalar minx = *std::min_element(latitude.begin(), latitude.end());
	const scalar miny = *std::min_element(longitude.begin(), longitude.end());
	delta_x = (maxx - minx) / (size_x - 1);
	delta_y = (maxy - miny) / (size_y - 1);
	start_x = minx - delta_x / 2.;
	start_y = miny - delta_y / 2.;
	end_x = maxx + delta_x / 2.;
	end_y = maxy + delta_y / 2.;

	bottom = create_array(size_y, size_x);
	delta_t = new double[size_y];
	for (int i = 0; i < size_y; i++) delta_t[i] = 0.0;
	/*		printf ("size_x: %d\n", size_x);
	printf ("size_y: %d\n", size_y);
	printf ("delta_x: %lf\n", delta_x);
	printf ("delta_y: %lf\n", delta_y);
	printf ("start_x: %lf\n", start_x);
	printf ("start_y: %lf\n", start_y);*/
	double prev = bottom[0][0] = 0;

	for (int k = 0; k < (int)depth.size(); k++)
	{
		const coord crd = get_coord(latitude[k], longitude[k]);
		const int i = crd.x, j = crd.y;
		/*if (bottom[0][0] != prev) {
			prev = bottom[0][0];
			}*/
		bottom[j][i] = depth[k];
	}
	initMainArrays(size_y, size_x);
	init_old_arrays();
	read_data_mareographs("mareographs.mrg");
	/*for (int i = 0; i < 10; i++){
		checking_mareographs();
		}
		for (int i = 0; i < maregraphs.size(); i++){
		maregraphs[i].writeToFileMareograph(maregraphs[i].getLocationNameASCII());
		}*/

}

int calculation_value_on_boundaries() {
	try {
		for (int i = 1; i < size_x; i++) {
			const int temp = (int)(i*size_y / size_x);
			v[0][i] = sqrt(abs(-M_G*h[0][i]))*eta[0][i] / (eta[1][i] - h[0][i]);
			v[size_y - 2][i] = sqrt(abs((-M_G*h[size_y - 2][i])))*eta[size_y - 3][i] / (eta[size_y - 3][i] - h[size_y - 2][i]);;
			v[size_y - 1][i] = v[size_y - 2][i];
			eta[0][i] = eta_old[0][i] - sqrt(abs(-h[0][i] * M_G))*(delta_t[temp] / delta_y_m[temp])*(eta_old[0][i] - eta_old[1][i]);
			eta[size_y - 2][i] = eta_old[size_y - 2][i] - sqrt(abs((-h[size_y - 2][i] * M_G)))*(delta_t[temp] / delta_y_m[temp])*(eta_old[size_y - 2][i] - eta_old[size_y - 3][i]);
			eta[size_y - 1][i] = eta[size_y - 2][i];
		}
		for (int j = 1; j < size_y; j++) {
			u[j][0] = sqrt((-M_G*h[j][0]))*eta[j][1] / (eta[j][1] - h[j][0]);
			u[j][size_x - 2] = u[j][size_x - 3];
			u[j][size_x - 1] = u[j][size_x - 2];
			eta[j][0] = eta_old[j][0] - sqrt(abs((-h[j][0] * M_G)))*(delta_t[j] / delta_x_m)*(eta_old[j][0] - eta_old[j][1]);
			eta[j][size_x - 2] = eta_old[j][size_x - 2] - sqrt(abs((-h[j][size_x - 2] * M_G)))*(delta_t[j] / delta_x_m)*(eta_old[j][size_x - 2] - eta_old[j][size_x - 3]);
			eta[j][size_x - 1] = eta[j][size_x - 2];
		}
	}
	catch (...) {

	}
	return 0;
}

void converting_motion_blocks(const int j, const int i) {
	double temp_speed; // speed of terr on every interval of time
	for (int b = 0; b < terr_cnt; b++) {
		if (terr_up[j][i] == b + 1) {
			if (terr_number_interval_time[b] == 1) {
				if (t * delta_t[j] <= brick_calc[b].number_up[0].brick_up_t && //if (t*delta_t[j] <= t_h_v_up[10][b] &&
					t * delta_t[j] >= brick_calc[b].begin_t)
				{
					temp_speed = brick_calc[b].number_up[0].height_up / (brick_calc[b].number_up[0].brick_up_t - brick_calc[b].begin_t);
					eta[j][i] = eta[j][i] + temp_speed * delta_t[j];
				}
			}
			else { // if there is any interval of time
				if (t * delta_t[j] <= brick_calc[b].number_up[0].brick_up_t &&
					t * delta_t[j] >= brick_calc[b].begin_t)
				{
					temp_speed = brick_calc[b].number_up[0].height_up / (brick_calc[b].number_up[0].brick_up_t - brick_calc[b].begin_t);
					eta[j][i] = eta[j][i] + temp_speed * delta_t[j];
				}
				else
				{
					if (t * delta_t[j] >= brick_calc[b].number_up[0].brick_up_t)
						for (int d = 0; d < terr_number_interval_time[b]; d++) {
							if (t * delta_t[j] <= brick_calc[b].number_up[d + 1].brick_up_t) { //  t_h_v_up[12 + d * 2][b]) {
								temp_speed = (brick_calc[b].number_up[d + 1].height_up - brick_calc[b].number_up[d].height_up) / (brick_calc[b].number_up[d + 1].brick_up_t - brick_calc[b].number_up[d].brick_up_t);
								eta[j][i] = eta[j][i] + temp_speed * delta_t[j];
								break;
							}
						}
				}
			}
		}
	}
	//output_array("c:\\tmp\\eta2.txt", eta);
}


void calculation_main_value() {
	for (int j = 1; j < size_y - 1; ++j) {
		Koef_Koriolisa = 2 * speed_of_earth * cos((start_y + j * delta_y) / 180.0 * M_PI);
		for (int i = 1; i < size_x - 1; ++i) {
			if (area[j][i] != 0.0) {
				//try {
					//double temp;
					//if (area[j][i] == 1) {
					//	temp = delta_t[j];
					//	delta_t[j] = delta_t[j] * 4;
					//	//               delta_t[j] = (delta_y_m[j]/sqrt(-2*M_G*h[j]						//               delta_t[j] = (abs(delta_y_m[j]*delta_x_m)/(double)8
					//	//                       /sqrt(((double)M_G*(double)abs(h[j][i])*
					//	//                       ((double)delta_y_m[j]*(double)delta_y_m[j]+(double)delta_x_m*(double)delta_x_m))));
					//}
					if (i < size_x - 2 && j < size_y - 2) {
						eta[j][i] = eta_old[j][i] - delta_t[j] * ((1 / (2 * delta_x_m)) *
							(u_old[j + 1][i] * (-h[j + 1][i] + -h[j][i]) -
							u_old[j][i] * (-h[j][i] + -h[j - 1][i])) + (1 / (2 * delta_y_m[j]) *
							(v_old[j][i + 1] * (-h[j][i + 1] + -h[j][i]) -
							v_old[j][i] * (-h[j][i - 1] + -h[j][i]))));

						/*
						eta[j][i] = eta_old[j][i]
						+ delta_t[j] * (0.5 / delta_x_m * (u_old[j + 1][i] * (h[j + 2][i] + h[j + 1][i])
						- u_old[j][i] * (h[j + 1][i] + h[j][i]))
						+ 0.5 / delta_y_m[j] * (v_old[j][i + 1] * (h[j][i + 2] + h[j][i + 1])
						- v_old[j][i] * (h[j][i + 1] + h[j][i])));
						*/
						if (maxVis[j][i] < eta[j][i]) {
							maxVis[j][i] = eta[j][i];
						}
						if (minVis[j][i] > eta[j][i]) {
							minVis[j][i] = eta[j][i];
						}

					}
					//if (area[j][i] == 1) { delta_t[j] = delta_t[j] / 4; }
					//if (i > 0 && j > 0) {

					//	double k01 = use_koef_Sh * 2 * 0.026 * 9.81 * sqrt(abs(u_old[j][i] * u_old[j][i] + v_old[j][i] * v_old[j][i])) / ((eta[j][i] + h[j][i]));
					//	u[j][i] = u_old[j][i] - (M_G / (2 * delta_x_m)*(eta[j][i] - eta[j - 1][i])
					//		- (Koef_Koriolisa)* v_old[j][i] - k01* u_old[j][i]) * delta_t[j];
					//	v[j][i] = v_old[j][i] - (M_G / (2 * delta_y_m[j])*(eta[j][i] - eta[j][i - 1])
					//		+ (Koef_Koriolisa)* u_old[j][i] - k01* v_old[j][i]) * delta_t[j];

					//	/*
					//	double k01 = 0.026 * use_koef_Sh * M_G * sqrt(u_old[j][i]*u_old[j][i] + v_old[j][i]*v_old[j][i]) / ((eta[j][i] + h[j][i]));
					//	u[j][i] = u_old[j][i] - M_G * delta_t[j] / (2 * delta_x_m) * (eta[j][i] - eta[j-1][i])
					//	- Koef_Koriolisa * v_old[j][i] - k01 * u_old[j][i] * u_old[j][i];
					//	v[j][i] = v_old[j][i] - M_G * delta_t[j] / (2 * delta_y_m[j]) * (eta[j][i] - eta[j][i-1])
					//	+ Koef_Koriolisa * u_old[j][i] - k01 * v_old[j][i] * v_old[j][i];
					//	*/
					//}

					//if (area[j][i] == 1) {
					//	delta_t[j] = temp;
					//}

					converting_motion_blocks(j, i);

					//if (eta[j][i] > 12) {
					//	int rand_ = rand() % 200;
					//	eta[j][i] = 12 - double(rand_) / 100;

					//	//               u[j][i] = 0.25 * (u_old[j-1][i] + u_old[j+1][i] + u_old[j][i-1] + u_old[j][i+1]);
					//	//               v[j][i] = 0.25 * (v_old[j-1][i] + v_old[j+1][i] + v_old[j][i-1] + v_old[j][i+1]);
					//	u[j][i] = 0.25 * (u_old[j][i - 1] + u_old[j][i + 1]);
					//	v[j][i] = 0.25 * (v_old[j - 1][i] + v_old[j + 1][i]);
					//	//               u[j][i] = 0;
					//	//               v[j][i] = 0;
					//}
					//if (eta[j][i] < -12) {
					//	int rand_ = rand() % 200;
					//	eta[j][i] = 12 + double(rand_) / 100;
					//	//               u[j][i] = 0.25 * (u_old[j-1][i] + u_old[j+1][i] + u_old[j][i-1] + u_old[j][i+1]);
					//	//               v[j][i] = 0.25 * (v_old[j-1][i] + v_old[j+1][i] + v_old[j][i-1] + v_old[j][i+1]);
					//	u[j][i] = 0.25 * (u_old[j][i - 1] + u_old[j][i + 1]);
					//	v[j][i] = 0.25 * (v_old[j - 1][i] + v_old[j + 1][i]);
					//	//               u[j][i] = 0;
					//	//               v[j][i] = 0;
					//}

					if (eta[j][i] > 16) {
						const int rand_ = rand() % 100;
						eta[j][i] = 4 - double(rand_) / 100;

						//               u[j][i] = 0.25 * (u_old[j-1][i] + u_old[j+1][i] + u_old[j][i-1] + u_old[j][i+1]);
						//               v[j][i] = 0.25 * (v_old[j-1][i] + v_old[j+1][i] + v_old[j][i-1] + v_old[j][i+1]);
						u[j][i] = 0.25 * (u_old[j][i - 1] + u_old[j][i + 1]);
						v[j][i] = 0.25 * (v_old[j - 1][i] + v_old[j + 1][i]);
						//               u[j][i] = 0;
						//               v[j][i] = 0;
					}
					if (eta[j][i] < -16) {
						const int rand_ = rand() % 100;
						eta[j][i] = -4 + double(rand_) / 100;
						//               u[j][i] = 0.25 * (u_old[j-1][i] + u_old[j+1][i] + u_old[j][i-1] + u_old[j][i+1]);
						//               v[j][i] = 0.25 * (v_old[j-1][i] + v_old[j+1][i] + v_old[j][i-1] + v_old[j][i+1]);
						u[j][i] = 0.25 * (u_old[j][i - 1] + u_old[j][i + 1]);
						v[j][i] = 0.25 * (v_old[j - 1][i] + v_old[j + 1][i]);
						//               u[j][i] = 0;
						//               v[j][i] = 0;
					}
				//}
				//catch (...)
				//{
				//	//	traces("Exception!");
				//}
			} // end if (area)

		}
	}
	for (int j = 1; j < size_y - 1; ++j) {
		//Koef_Koriolisa = 2 * speed_of_earth * cos((start_y + j * delta_y) / 180.0 * M_PI);
		double Ch = 0.0025;
		for (int i = 1; i < size_x - 1; ++i) {
			if (i < size_x && j < size_y && area[j][i] )
			{

				// формула из разностной схемы
				const double Ch = 0.0025;
				u[j][i] = u_old[j][i] - M_G *delta_t[j] / delta_x_m * (eta[j][i] - eta[j - 1][i]) - delta_t[j] * Ch / (eta[j][i] + -h[j][i]) * fabs(u_old[j][i]) * u_old[j][i];

				v[j][i] = v_old[j][i] - M_G * delta_t[j] / delta_y_m[j] * (eta[j][i] - eta[j][i - 1]) - delta_t[j] * Ch / (eta[j][i] + -h[j][i]) * fabs(v_old[j][i]) * v_old[j][i];


				/* uu[i][j] = uold[i][j] * (1.0 - (DELTAT/(2*DELTAX)) * (uold[i+1][j] - uold[i-1][j]))
				+ vold[i][j] * ( - (DELTAT/(2*DELTAY)) * (uold[i][j+1] - uold[i][j-1]))
				- G * (DELTAT/(2*DELTAX)) * (eta[i+1][j] - eta[i-1][j]);

				vv[i][j] = vold[i][j] * (1.0 - (DELTAT/(2*DELTAY)) * (vold[i][j+1] - vold[i][j-1]))
				+ uu[i][j] * (  - (DELTAT/(2*DELTAX))* (vold[i+1][j] - vold[i-1][j]))
				- G * (DELTAT/(2*DELTAY)) * (eta[i][j+1] - eta[i][j-1]);
				*/
			}
		}
	}

	for (int j = 1; j < size_y - 1; ++j) {
		for (int i = 1; i < size_x - 1; ++i) {
			{
				//if (area[j][i] == 0) {
				//	u[j][i] = 0;
				//	v[j][i] = 0;
				//}
				if (((area[j + 1][i - 1] == 0) || (area[j][i - 1] == 0) || (area[j - 1][i - 1] == 0) 
					|| (area[j + 1][i] == 0) || (area[j - 1][i] == 0) ||
					(area[j + 1][i + 1] == 0) || (area[j][i + 1] == 0) || (area[j - 1][i + 1] == 0)) && (area[j][i] >= 1))
				{
					u[j][i] = 0;
					v[j][i] = 0;
				}
			}
		}
	}

	
	for (int j = 0; j < size_y; j++) {
		for (int i = 0; i < size_x; i++) {
			eta_old[j][i] = eta[j][i];
			u_old[j][i] = u[j][i];
			v_old[j][i] = v[j][i];
		}
	}

	calculation_value_on_boundaries();
}


void fill_tetragon(int** terr, int v, coord g[4])
{
	//printf("%d %d | %d %d | %d %d | %d %d\n", i1, j1, i2, j2, i3, j3, i4, j4);
	const int moving = abs(g[0].x - g[1].x) + abs(g[1].x - g[2].x) + abs(g[2].x - g[3].x) + abs(g[3].x - g[0].x) +
		abs(g[0].y - g[1].y) + abs(g[1].y - g[2].y) + abs(g[2].y - g[3].y) + abs(g[3].y - g[0].y);
	if (moving == 0) return;
	double x, y;
	int prom_i, prom_j;

	for (x = 0; x <= 1; x = x + 1.0 / (double)(moving)) {
		for (y = 0; y <= 1; y = y + 1.0 / (double)(moving)) {
			prom_i = int(x * (g[0].x * y + g[1].x * (1 - y)) + (1 - x) * (g[2].x * y + g[3].x * (1 - y)));
			prom_j = int(x * (g[0].y * y + g[1].y * (1 - y)) + (1 - x) * (g[2].y * y + g[3].y * (1 - y)));
			if (prom_i < size_x && prom_j < size_y && prom_i >= 0 && prom_j >= 0) terr[prom_j][prom_i] = v;
		}
	}
}


void read_brick_file(const nana::string& file_path)
{
	FILE* infile = _wfopen(file_path.c_str(), L"rt");
	if (!infile)
		abort();
	int count_angles;
	fscanf(infile, "%d", &terr_cnt);
	fscanf(infile, "%d", &count_angles);
	brick_calc = new brick_data[terr_cnt];
	for (int i = 0; i < terr_cnt; i++)
	{
		int count_brick_up;
		for (int j = 0; j < count_angles; j++)
		{
			scalar x, y;
			fscanf(infile, "%lf%lf", &x, &y);
			brick_calc[i].points.emplace_back(x, y);
		}

		fscanf(infile, "%lf", &brick_calc[i].begin_t);
		fscanf(infile, "%d", &count_brick_up);

		for (int j = 0; j < count_brick_up; j++)
		{
			scalar tmp_height_up;
			int tmp_brick_up_t;
			fscanf(infile, "%d%lf", &tmp_brick_up_t, &tmp_height_up);
			brick_calc[i].number_up.emplace_back(tmp_brick_up_t, tmp_height_up);
		}

		terr_number_interval_time[i] = count_brick_up;
		coord g[4];
		for (int j = 0; j < (int)brick_calc[i].points.size(); j++) {
			g[j] = get_coord(brick_calc[i].points[j].x, brick_calc[i].points[j].y);
		}
		fill_tetragon(terr_up, i + 1, g);
	}

	fclose(infile);
	//freopen("CON", "w", stdout);
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

void find_inking() {
	bool v1 = false;
	bool v2 = false;
	bool v3 = false;
	bool v4 = false;
	const double exp = 0.00001;
	for (int j = 1; j < size_y - 1; ++j) {
		v1 = false;
		v2 = false;
		for (int i = 1; i < size_x - 1; ++i) {
			if (!v1 && abs(eta[j][i]) > exp) {
				inking[inking_count++] = i;
				inking[inking_count++] = j;
				v1 = true;
			}
		}
		for (int i = size_x - 1; i > 0; i--) {
			if (!v2 && abs(eta[j][i]) > exp) {
				inking[inking_count++] = i;
				inking[inking_count++] = j;
				v2 = true;
			}
		}
	}
	for (int i = 1; i < size_x - 1; ++i) {
		v3 = false;
		v4 = false;
		for (int j = 1; j < size_y - 1; ++j) {
			if (!v3 && abs(eta[j][i]) > exp) {
				inking[inking_count++] = i;
				inking[inking_count++] = j;
				v3 = true;
			}
		}
		for (int j = size_y - 1; j > 0; j--) {
			if (!v4 && abs(eta[j][i]) > exp) {
				inking[inking_count++] = i;
				inking[inking_count++] = j;
				v4 = true;
			}
		}
	}
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
	char b[201];
	//cout << "file name: ";
	//cin >> name;
	const wchar_t *path = path_file.c_str();
	FILE* in = _wfopen(path, L"r");
	if (!in)
		abort();
	//name += "dat";
	//const char *path2 = name.c_str();
	FILE* out = fopen("out.dat", "w");
	if (!out)
		abort();

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

void read_data_mareographs(const std::string& file_path){
	fstream file;
	const char * path = file_path.c_str();;
	file.open(path, std::fstream::in);

	int count;
	file >> count;
	for (int i = 0; i < count; i++){
		string location;
		double x, y;
		file >> location >> x >> y;
		Mareograph m(y, x, 25, location);
		coord point = get_coord(x, y);
		m.setIndexX(point.x);
		m.setIndexY(point.y);
		mareographs.push_back(m);
	}
}

void checking_mareographs(){
	for (auto& mareograph : mareographs)
	{
		mareograph.pushHeight(eta[mareograph.getIndexY()][mareograph.getIndexX()]);
	}
}
void saveMareographs(){
	for (size_t i = 0; i < mareographs.size(); i++){
		if (i < 10)
			mareographs[i].writeToFileMareograph(PATH "data\\0" + std::to_string(i));
		else 
			mareographs[i].writeToFileMareograph(PATH "data\\" + std::to_string(i));
	}
	Mareograph mareograph;
	mareograph.writeToParametersMareograph(PATH "Parameters\\parameters.txt", mareographs);
}

double getDistantionOfCenterMareographs(const double x, const double y, const double maxDist) {
	double out = maxDist + 0.00000000000001;
	for (auto& mareograph : mareographs)
	{
		const double x1 = mareograph.getLongitude(), y1 = mareograph.getLatitude();
		//coord a = get_coord(yCoord, xCoord);
		//int dist = abs(c.x - a.x) + abs(c.y - a.y);
		const double dist = sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
		if (dist <= maxDist && dist <= out)
			out = dist;
	}
	return out;
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