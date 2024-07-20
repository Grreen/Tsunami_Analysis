#ifndef CALCULATION_H
#define CALCULATION_H
#include <iostream>
#include <vector>
#include <nana/gui/wvl.hpp>
#include "geometry.h"
#include "Mareograph.h"
#include <nana/basic_types.hpp>

typedef  float scalar;
using namespace std;

extern int variant_delta_t;
extern int size_x, size_y;
extern int output_time, time_moments, t;
extern double izobata;
extern vector<vector<float>> u;
extern vector<vector<float>> v;
extern vector<vector<float>> bottom;
extern vector<vector<float>> eta;
extern float** maxVis;
extern float** minVis;
extern double** CComp; 
extern bool **bool_CComp;
extern float start_x, start_y, end_x, end_y;
extern float delta_x, delta_y;

struct Settings
{
	static constexpr wchar_t DEFAULT_PATH_TO_SAVE[] = L"../../result/";

	static constexpr wchar_t DEFAULT_FONT_NAME[] = L"Times New Roman";
	static constexpr double DEFAULT_FONT_SIZE = 14;

	static constexpr double DEFAULT_STEP_X = 1;
	static constexpr double DEFAULT_STEP_FONT_X = 0.5;
	static constexpr double DEFAULT_STEP_Y = 1;
	static constexpr double DEFAULT_STEP_FONT_Y = 0.5;

	static constexpr unsigned DEFAULT_MAX_SIZE = 1000;
	//Paths
	std::wstring wsPathToSave;

	//Font
	std::wstring wsFontName;
	double dFontSize;
	bool bFrontPepcentage;

	//Steps
	bool   bUseSteps;
	double dStepX;
	double dStepFontX;
	double dStepY;
	double dStepFontY;

	//Size
	unsigned uMaxSize;

	//Saves
	bool bSaveMaregrams;
	bool bSaveMaxHeight;

	//
	bool bDrawDirectionWave;
	bool bDrawGrid;

	//
	enum
	{
		LANGUAGE_EN,
		LANGUAGE_RU
	} eLanguage;

	struct colorBar
	{
		std::map<double, nana::color> mMap;
		double dTick;
		std::wstring wsUnitMeasure;
		colorBar() : dTick{ 0. }, wsUnitMeasure{ L"м"} {};

		void Clear()
		{
			mMap.clear();
			dTick = 0.;
		}

		double GetTick() const
		{
			return dTick;
		}
	};

	colorBar height;
	colorBar front;
	colorBar maxHeight;

	Settings();
	void ParseFromFile(const nana::string& sFiePath);
	double GetFontSize(double dHeight) const;
};

extern Settings settings;
extern std::vector<Mareograph> arMareographs;

struct brick_point
{
	scalar x;
	scalar y;
	brick_point(scalar X, scalar Y) : x(X), y(Y) {};
	bool operator<(const brick_point& point) const
	{
		if (x != point.x)
			return x < point.x;

		return y < point.y;
	}
};
struct brick_up
{
	scalar brick_up_start_t;
	scalar brick_up_end_t;
	scalar height_up;
	brick_up(scalar a, scalar b, scalar c) : brick_up_start_t(a), brick_up_end_t(b), height_up(c) {};
};
struct brick_data
{
	scalar begin_t;
	vector <brick_point> points;
	vector <brick_up> number_up;
	nana::color oColor;

	brick_data()
		: begin_t(0.), oColor(rand() % 256, rand() % 256, rand() % 150, 150)
	{
	}
};
struct bricks
{
	vector<brick_data> brick_calc;

	scalar dMinX;
	scalar dMinY;
	scalar dMaxX;
	scalar dMaxY;

	bricks()
		: dMinX(99999999), dMinY(99999999), dMaxX(0), dMaxY(0)
	{}

	bool Empty() const
	{
		return brick_calc.empty();
	}

	void Clear()
	{
		dMinX = 99999999;
		dMinY = 99999999;
		dMaxX = 0;
		dMaxY = 0;

		brick_calc.clear();
	}

	void AddBrick(const brick_data& oBrick)
	{
		for (const brick_point& oPoint : oBrick.points)
		{
			dMinX = min(dMinX, oPoint.x);
			dMinY = min(dMinY, oPoint.y);
			dMaxX = max(dMaxX, oPoint.x);
			dMaxY = max(dMaxY, oPoint.y);
		}

		brick_calc.push_back(oBrick);
	}
};

extern bricks oBricks;

enum EarthqakeEvent {
	DEFAULT,
	MEXICO_BIG,
	PACIFIC_CUT
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

extern vector <double> average;
extern vector <double> max_from_components;
extern set_area_data gist_d;
extern vector <vector <coord>> v_c_c;
extern vector <vector <coord>> f_c_c;
//void dfs(coord start);
coord get_coord(float x, float y);
double** create_array(int y, int x);
std::vector<real_point> GetCoordsMareographs();
void checking_mareographs();
void saveMareographs();
void convert_to_dat(const nana::string& path_file);
void find_comps();
void read_array(const nana :: string& file_path);
void read_dat(const nana::string& file_path);
void read_brick_file(const nana::string& file_path);
void read_data_mareographs(const nana::string& file_path);
void set_delta(int v);
void run_calc();
double max_h(int s);
#endif