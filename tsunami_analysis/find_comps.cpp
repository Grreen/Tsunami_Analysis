#include "calculation.h"

#include "interface_nana.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>  
#include <algorithm>
#include <queue>
#include <locale>
#include <codecvt>
#include <string>
using namespace std;

vector <double*> gist2d_value;


void find_aqua();
void save_cut_max(std::string path);

struct gist2d_data{
	int count, NC;
	std::wstring looking_at;
};

gist2d_data g2d_dat;

vector <int> ** create_array_vector(int y, int x)
{
	vector <int> **m = new vector <int> *[y + 1];
	for (int j = 0; j < y; ++j)
	{
		m[j] = new vector <int>[x];
	}
	return m;
}

bool** create_array_bool(int y, int x)
{
	bool **m = new bool *[y + 1];
	for (int j = 0; j < y; ++j)
	{
		m[j] = new bool[x];
		for (int i = 0; i < x; ++i) m[j][i] = false;
	}
	return m;
}

void read_array(const nana::string& file_path)
{
	CComp = create_array((gist_d.size_y + 2), (gist_d.size_x + 2));
	
	bool_CComp = create_array_bool((gist_d.size_y + 2), (gist_d.size_x + 2));
	for (int i = 0; i < gist_d.size_y + 2; i++)
	{
		for (int j = 0; j < gist_d.size_x + 2; j++)
		{
			CComp[i][j] = -1.;
		}
	}
	wfstream file;
	file.open(file_path.c_str(), std::wfstream::in);
	int i1 = 1, j1 = 1; int cn = 0;
	for (int i = 0; i < size_y; i++){
		bool fl = false;
		for (int j = 0; j < size_x; j++)
		{
			if ((i >= gist_d.t_r.y && i <= gist_d.l_l.y) && (j >= gist_d.l_l.x && j <= gist_d.t_r.x)){
				file >> CComp[i1][j1]; cn++;
		//		if (CComp[i1][j1] >= 3.8) CComp[i1][j1] /= 1.4;
			//	else CComp[i1][j1] /= 1.5;
				
				j1++;
				fl = true;
			}
			else{
				double a; file >> a; cn++;
			}
		}
		if (fl) i1++;
		j1 = 1;

		if (i1 > gist_d.size_y) break;
	}

	for (int i = 1; i < gist_d.size_y + 2 - 1; i++){
		for (int j = 1; j < gist_d.size_x + 2 - 1; j++){

			if (CComp[i][j] < 0){
				double a = -0.1;

				if (CComp[i + 1][j] < a && CComp[i][j + 1] < a
					&& CComp[i + 1][j + 1] < a && CComp[i - 1][j + 1] < a &&
					CComp[i - 1][j] < a && CComp[i][j - 1] < a &&
					CComp[i - 1][j - 1] < a && CComp[i + 1][j - 1] < a)
					CComp[i][j] = -1.;

			}
		}
	}
}

vector <double> average;
vector <double> max_from_components;
vector <vector <coord>> v_c_c; //= new vector <coord>[MAXN];// [MAXN];
vector <vector <coord>> f_c_c;
vector<coord> buf;//(20000);// (5000);
vector <int> **fp;
bool point_own(int y, int x, int n){
	//	cout << "1 ";

	for (int i = 0; i < int(fp[y][x].size()); i++){

		if (fp[y][x][i] == n) return true;
	}


	return false;
}

int dx[4] = { 1, 0, -1, 0 };
int dy[4] = { 0, 1, 0, -1 };

void dfs(int x, int y, int n){
	bool_CComp[y][x] = true;
	coord a = { x, y };
	buf.push_back(a);
	for (int i = 0; i < 4; i++){
		if (y + dy[i] == buf[0].y && x + dx[i] == buf[0].x) {
			coord b;  b.x = x + dx[i];  b.y = y + dy[i]; buf.push_back(b); buf.push_back(a);
		}
		//cout << "size: " << buf.size() << "  | " << y + dy[i] << "  | " << x + dx[i] << endl;
		//point_own(y + dy[i], x + dx[i], n);
		//cout << " 2"<<endl;
		if (a.y + dy[i] < gist_d.size_y + 2 && a.x + dx[i] < gist_d.size_x + 2 && a.y + dy[i] >= 0 && a.x + dx[i] >= 0){
			if (/*CComp[y + dy[i]][x + dx[i]] > 0 &&*/
				point_own(y + dy[i], x + dx[i], n) &&
				!bool_CComp[y + dy[i]][x + dx[i]]){
				dfs(x + dx[i], y + dy[i], n);
				buf.push_back(a);
				//cout << CComp[a.y][a.] << endl;
			}
		}
	}
}
void bfs(coord start)
{
	vector <coord> tmp;

	queue <coord> q;
	bool_CComp[start.y][start.x] = true;
	q.push(start);
	while (!q.empty())
	{
		coord bu;
		coord v = q.front();
		q.pop();
		tmp.push_back(v);
		int dx[8] = { 1, 0, -1, 0, 1, -1, 1, -1 };
		int dy[8] = { 0, 1, 0, -1, 1, -1, -1, 1 };
		for (int i = 0; i < 8; i++){
			if (v.y + dy[i] < gist_d.size_y + 2 && v.x + dx[i] < gist_d.size_x + 2)
				if (CComp[v.y + dy[i]][v.x + dx[i]] < -1 && !bool_CComp[v.y + dy[i]][v.x + dx[i]]){
					bu.y = v.y + dy[i];  bu.x = v.x + dx[i];
					q.push(bu);
					bool_CComp[v.y + dy[i]][v.x + dx[i]] = true;
				}
		}
	}
	v_c_c.push_back(tmp);
	tmp.clear();
}


void save_all_components(){
	fstream file;
	
	for (int i = 0; i < v_c_c.size(); i++){
		std::string path = "data_components\\" + std::to_string(i) + ".dat";
		file.open(path.c_str(), std::fstream::out);
		double sum = 0;
		double max = 0;
		for (int j = 0; j < v_c_c[i].size(); j++){
			if (max < CComp[v_c_c[i][j].y][v_c_c[i][j].x]) max = CComp[v_c_c[i][j].y][v_c_c[i][j].x];
			sum += CComp[v_c_c[i][j].y][v_c_c[i][j].x];
			file << CComp[v_c_c[i][j].y][v_c_c[i][j].x] << " ";
		}
		file.close();
		max_from_components.push_back(max);
		average.push_back( sum / v_c_c[i].size() );
	}
	//v_c_c[][]
}	

void find_comps()
{
	coord bu;
	for (int i = 1; i < gist_d.size_y + 2 - 1; i++){
		for (int j = 1; j < gist_d.size_x + 2 - 1; j++){
			if (CComp[i][j] < -1 && bool_CComp[i][j] == false) { bu.y = i; bu.x = j; bfs(bu); }
		}
	}
	find_aqua();
	save_all_components();
	//calc_average();

	for (int n = 0; n < int(v_c_c.size()); n++){
		for (int i = 0; i < gist_d.size_y + 2; i++)
		{
			for (int j = 0; j < gist_d.size_x + 2; j++)
			{
				bool_CComp[i][j] = false;
			}
		}
		if (!v_c_c[n].empty()) dfs(v_c_c[n][0].x, v_c_c[n][0].y, n);
		f_c_c.push_back(buf);
		buf.clear();
	}
	//save_cut_max("MAX_PROLIV1.dat");
}
const double eps = 1e-6;

void find_aqua()
{
	vector <coord> tmp;
	/*int begin = find_begin(v_c_c[i]);
	v_c_c[i][begin];*/
	fp = create_array_vector(gist_d.size_y + 2, gist_d.size_x + 2);
	for (int i = 0; i < int(v_c_c.size()); i++){
		coord a;

		for (int j = 0; j < int(v_c_c[i].size()); j++)
		{
			for (int xt = -1; xt < 2; xt++)
			{
				for (int yt = -1; yt < 2; yt++){
					bool b = false;
					if (xt == 0 && yt == 0) continue;
					//if (v_c_c[i][j].y + yt == size_y || v_c_c[i][j].x + xt == size_x) continue;
					for (int it = 0; it < int(fp[v_c_c[i][j].y + yt][v_c_c[i][j].x + xt].size()); it++){
						if (fp[v_c_c[i][j].y + yt][v_c_c[i][j].x + xt][it] == i) b = true;
					}
					if (CComp[v_c_c[i][j].y + yt][v_c_c[i][j].x + xt] > -eps && !b){
						a.y = v_c_c[i][j].y + yt;
						a.x = v_c_c[i][j].x + xt;
						fp[a.y][a.x].push_back(i);
						tmp.push_back(a);
						bool_CComp[v_c_c[i][j].y + yt][v_c_c[i][j].x + xt] = true;
					}
				}
			}
		}
		v_c_c[i].clear();
		v_c_c[i] = tmp;
		tmp.clear();
	}

}
void save_cut_max(std::string path) {
	fstream file;
	double** tmp = create_array((gist_d.size_y + 2), (gist_d.size_x + 2));
	file.open(path.c_str(), std::fstream::out);
	for (int i = 0; i < gist_d.size_y + 2; i++){
		for (int j = 1; j < gist_d.size_x + 2; j++){
			if (CComp[i][j] < -0.001) tmp[i][j] = -5;
			else tmp[i][j] = 0;
		}
	}
	for (int i = 0; i < (int)f_c_c[0].size(); i++) {
		tmp[f_c_c[0][i].y][f_c_c[0][i].x] = CComp[f_c_c[0][i].y][f_c_c[0][i].x];
	}

	for (int i = 1; i < gist_d.size_y + 2 - 1; i++){
		for (int j = 1; j < gist_d.size_x + 2 - 1; j++){
			if (tmp[i][j] < -4) file << "NaN" << " ";
			else file << tmp[i][j] << " ";
		}
		file << "\n";
	}
}
/*void gist2d_set_value(vector <std::wstring> ){
	for (int i = 0; i < g2d_dat.count;){

		}
	
	}
}*/
/*double *buf;
if (g2d_dat.looking_at == L"NORTH"){
buf = new double[gist_d.size_x];
memset(buf, 0., gist_d.size_x * sizeof(int));
for (int j = 0; j < f_c_c[g2d_dat.NC].size(); j++){
if (f_c_c[g2d_dat.NC][j].x > a[])
}*/