#ifndef MAREOGRAPH_H
#define MAREOGRAPH_H
//#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
class Mareograph
{
public:
	Mareograph();
	Mareograph(double _latitude, double _longitude, int _stepTime, std::string _locationName):
		longitude(_longitude), latitude(_latitude), indexX(0), indexY(0), stepTime(_stepTime),
		locationNameASCII(_locationName)
	{
		std::wstring locationName(_locationName.begin(), _locationName.end());
		locationNameUTF8 = std::move( locationName);
		maxHeight = 0;
		minHeight = 0;
		startTime = 0;
	}

	Mareograph(double _latitude, double _longitude, int _stepTime, std::wstring _locationName):
		longitude(_longitude), latitude(_latitude), indexX(0), indexY(0), stepTime(_stepTime), startTime(0),
		maxHeight(0),
		minHeight(0),
		locationNameUTF8(_locationName)
	{
		std::string locationName(_locationName.begin(), _locationName.end());
		locationNameASCII = std::move( locationName);
	}

	Mareograph(int _stepTime) : longitude(0), latitude(0), indexX(0), indexY(0), stepTime(_stepTime), startTime(0),
	                            maxHeight(0),
	                            minHeight(0)
	{
	}

	virtual ~Mareograph();

	//Mareograph(Mareograph&) = delete;
	//Mareograph(const Mareograph&) = delete;
	//Mareograph(Mareograph&&) = delete;

	//Mareograph& operator=(Mareograph&) = delete;
	//const Mareograph& operator=(const Mareograph&) = delete;
	//Mareograph&& operator=(Mareograph&&) = delete;


	//-----------------//
	/* getter methods */
	//----------------//
	
	double getLongitude() const { return longitude; }
	double getLatitude() const { return latitude; }
	double getMaxHeight() const { return maxHeight; }
	double getMinHeight() const { return minHeight; }
	int getIndexX() const { return indexX; }
	int getIndexY() const { return indexY; }
	int getStepTime() const { return stepTime; }
	int getStartTime() const { return startTime; }
	std::string getLocationNameASCII() const { return locationNameASCII; }
	std::wstring getLocationNameUTF8() const { return locationNameUTF8; }
	double getHeight(int index) const { return heights[index]; }
	unsigned int getSizeHeights() const { return heights.size(); }
	
	//----------------//
	/* setter methods */
	//----------------//
	
	void setLongitude(double _longitude)  { longitude = _longitude; }
	void setLatitude(double _latitude) { latitude = _latitude; }	
	void setStepTime(int _stepTime){ stepTime = _stepTime; }
	void setLocationName(const std::string& _locationName) {
		const std::wstring locationName(_locationName.begin(), _locationName.end());
		locationNameUTF8 = locationName;
		locationNameASCII = _locationName;
	}
	void setLocationName(const std::wstring& _locationName) {
		const std::string locationName(_locationName.begin(), _locationName.end());
		locationNameUTF8 = _locationName;
		locationNameASCII = locationName;
	}

	void setIndexX(double startX, double endX, double deltaX)
	{ indexX =(int)((longitude - startX) / deltaX);	}

	void setIndexX(int index)
	{
		indexX = index;
	}
	
	void setIndexY( double startY, double endY, double deltaY, int sizeY)
	{ indexY = sizeY - 1 - (int)(((latitude - startY) / deltaY));}

	void setIndexY(int index)
	{
		indexY = index;
	}

	//----------------//
	/*  push  height  */
	//----------------//
	
	void pushHeight(double height) { 
		if (height > maxHeight){
			if (startTime == 0 && height > 0.0001) startTime = stepTime * heights.size();
			maxHeight = height;
		}
		if (height < minHeight){
			if (startTime == 0 && height < 0.0001) startTime = stepTime * heights.size();
			minHeight = height;
		}
		heights.push_back(height); 
	}
	void pushHeightFromArray(double** eta) { heights.push_back(eta[indexX][indexY]); }
	
	//----------------//
	/* Wtite to file  */
	//----------------//
	
	static void writeToFile(const std::string& path, const std::vector <Mareograph> &M, bool writeHeights = true){
		if (M.empty()) return;
		std::fstream file;
		file.open(path, std::fstream::out);
		file << M.size() << "\n";
		file << M[0].getStepTime() << "\n";

		const size_t size = M.size();

		 for (size_t j = 0; j < size; j++){
			file << M[j].getLocationNameASCII().c_str() << "\t";
		}
		file << "\n";

		for (size_t j = 0; j < size; j++){
			file << M[j].getLongitude() << "\t";
		}
		file << "\n";

		for (size_t j = 0; j < size; j++){
			file << M[j].getLatitude() << "\t";
		}
		if (writeHeights){
			if (!M[0].getSizeHeights()) file << "\n";

			for (int i = 0; i < (int)M[0].getSizeHeights(); i++){

				for (size_t j = 0; j < size; j++){
					file << M[j].getHeight(i) << "\t";
				}
				file << "\n";
			}
		}
	}

	void writeToFileMareograph(const std::string& path) const
	{
		std::cout << "Saving... " << path << std::endl;
		int size = (int)heights.size();
		if (size == 0) return;
		std::fstream file;
		file.open(path, std::fstream::out);
		file << locationNameASCII.c_str() << "\n";		
		file << size << "\n";
		for (int i = 0; i < size; i++){
			file << heights[i] << "\n";
		}
	}

	std::string getStartTimeString(int t) {
		int time = t - (t / 60 * 60);
		std::string clock_sec = std::to_string((time));
		time = t / 60 - (t / 60 / 60 * 60);
		std::string clock_min = std::to_string((time));
		time = t / 60 / 60;
		std::string clock_hour = std::to_string((time));
		if (clock_sec.size() == 1) { clock_sec = "0" + clock_sec; }
		if (clock_min.size() == 1) { clock_min = "0" + clock_min; }
		if (clock_hour.size() == 1) { clock_hour = "0" + clock_hour; }
		std::string clock_time = clock_hour + ":" + clock_min + ":" + clock_sec;
		return clock_time;
	}

	void writeToParametersMareograph(const std::string& path, const std::vector <Mareograph>  &M){
		if (M.empty()) return;
		const size_t size = M.size();
		std::fstream file;
		file.open(path, std::fstream::out);
		file << "Location Name\tMax Height\tMinHeight\tStart Time\n";
		for (size_t i = 0; i < size; i++){
			

			file << M[i].getLocationNameASCII() << "\t" << M[i].getMaxHeight()
				<< "\t" << M[i].getMinHeight() << "\t" << getStartTimeString(M[i].getStartTime()) << "\n";
		}
	}
		
	static void writeToFile(const std::wstring& path, const std::vector <Mareograph>& M, bool writeHeights = true){
		const std::string _path(path.begin(), path.end());
		writeToFile(_path, M, writeHeights);
	}

	static std::vector<Mareograph> readFromFile(const std::string& path,
	                                            double startX, double startY, double endX, double endY,
	                                            double deltaX, double deltaY, int sizeY, bool readHeights = true){
		std::vector<Mareograph> M;
		std::fstream file;
		file.open(path, std::fstream::in);
		int count, _stepTime;
		file >> count >> _stepTime;
		
		for (int i = 0; i < count; i++){
			M.emplace_back(_stepTime);
			std::string name;
			file >> name;
			M[i].setLocationName(name);
		}
		for (int i = 0; i < count; i++){
			double _longitude;
			file >> _longitude;
			M[i].setLongitude(_longitude);
			M[i].setIndexX(startX, endX, deltaX);
		}
		for (int i = 0; i < count; i++){
			double _latitude;
			file >> _latitude;
			M[i].setLatitude(_latitude);
			M[i].setIndexY(startY, endY, deltaY, sizeY);
		}
		if (readHeights){
			double height;
			while (file >> height){
				for (int j = 0; j < count; j++){
					M[j].pushHeight(height);
				}
			}
		}
		return M;
	}
	
	static std::vector<Mareograph> readFromFile(const std::wstring& path,
	                                            double startX, double startY, double endX, double endY,
	                                            double deltaX, double deltaY, int sizeY, bool readHeights = true){
		std::string _path(path.begin(), path.end());
		std::vector<Mareograph> M = readFromFile(_path, startX, startY, endX, endY, deltaX, deltaY, sizeY, readHeights);
		return M;
	}


private:
	double longitude, latitude;
	int indexX, indexY;
	int stepTime, startTime;
	double maxHeight, minHeight;
	

	std::string locationNameASCII;
	std::wstring locationNameUTF8;
	std::vector <double> heights;
};
#endif
