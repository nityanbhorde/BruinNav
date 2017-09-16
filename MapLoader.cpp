#include <iostream>
#include <fstream>
#include "provided.h"
#include <string>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	vector<StreetSegment> strsegs;

};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream infile(mapFile);
	if (infile.is_open()) { // if we opened it
		string name;
		while (getline(infile, name)) { // while we can get a name
			StreetSegment street;
			string  geocord;
			street.streetName = name; //  set the name geocoord and number of attractions 
			getline(infile, geocord);
			string startlat1, startlong1;
			string startlat2, startlong2;
			int x = geocord.find(','); // sift through looking for commas or spaces to distinguish between lats and longs
			startlat1 = geocord.substr(0,x);
			geocord.erase(0, x+1);
			while(geocord[0] == ' ') { // delete spaces where they shouldnt be 
				geocord.erase(0, 1);
			}
			int y = geocord.find(' ');
			startlong1 = geocord.substr(0, y);
			geocord.erase(0, y + 1);
			int a = geocord.find(',');
			startlat2 = geocord.substr(0,a);
			geocord.erase(0,a+1);
			while(geocord[0] == ' ') {
				geocord.erase(0, 1);
			}
			startlong2 = geocord;
			GeoSegment gs(GeoCoord(startlat1, startlong1), GeoCoord(startlat2, startlong2));
			street.segment = gs;
			string numattractionlines;
			double numattractions;
			getline(infile, numattractionlines);
			numattractions = stod(numattractionlines);
			if (numattractions != 0) { // if we find some attractions 
				string attractions;
				while (numattractions != 0) {
					getline(infile, attractions); // get the attractions and loop through them finding their appropriate 
					string attractname; // names etc..
					int c = attractions.find('|');
					attractname = attractions.substr(0, c);
					attractions.erase(0,c + 1);
					int d = attractions.find(',');
					string attractlat = attractions.substr(0, d);
					attractions.erase(0, d + 1);
					while (attractions[0] == ' ') {
						attractions.erase(0, 1);
					}
					string attractlong = attractions;
					Attraction fun;
					fun.name = attractname;
					GeoCoord geo(attractlat, attractlong);
					fun.geocoordinates = geo;
					street.attractions.push_back(fun);
					numattractions--;
				}
			}
			strsegs.push_back(street); //push back the street
		}
		return true;
	}
	return false;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return strsegs.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < getNumSegments()) { // if we are within our range
		seg = strsegs[segNum];// set it equal to the corresponding street seg
		return true;
	}
	return false;  
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
