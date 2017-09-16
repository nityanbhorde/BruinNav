#include "provided.h"
#include "MyMap.h"
#include <iostream>
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> mapofAttractions;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	for (int i = 0; i < ml.getNumSegments(); i++) {
		StreetSegment seg;
		ml.getSegment(i,seg); // get the street segments
		for (int j = 0; j < seg.attractions.size(); j++) {
			GeoCoord g = seg.attractions[j].geocoordinates; // and its geocoords
			string lower = "";
			for (int r = 0; r < seg.attractions[j].name.size(); r++) {
				lower += tolower(seg.attractions[j].name[r]); // lower everything
			}
			mapofAttractions.associate(lower, g);
		}
	}
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	for (int r = 0; r < attraction.size(); r++) {
		attraction[r] = tolower(attraction[r]); // lower everything
	}
	const GeoCoord * temp;
	temp = (mapofAttractions.find(attraction)); // look for geocoord if its not there its a nulptr otherwise set the geocoord
	if (temp != nullptr) {
		gc = *temp;
		return true;
	}
	return false;
	
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
