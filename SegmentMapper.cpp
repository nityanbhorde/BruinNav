#include "provided.h"
#include "MyMap.h"
#include <vector>
#include "support.h"
using namespace std;
class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private :
	MyMap<GeoCoord, vector<StreetSegment>> map;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	for (int i = 0; i < ml.getNumSegments(); i++) {
		StreetSegment seg;
		ml.getSegment(i, seg); // get the segment
		GeoCoord start = seg.segment.start;
		if (map.find(start) == nullptr) { // if its not a nullptr insert a new street segment vector and associate it and push back
			vector<StreetSegment> temp;
			temp.push_back(seg);
			map.associate(start, temp);
		}
		else { // otherwise we will push back our street seg and reassociate it
			vector<StreetSegment> *temp = map.find(start);
			temp->push_back(seg);
			map.associate(start, *temp);
		}
		GeoCoord end = seg.segment.end;
		if (map.find(end) == nullptr) {
			vector<StreetSegment> temp;
			temp.push_back(seg);
			map.associate(end, temp);
		}
		else {
			vector<StreetSegment> *temp = map.find(end);
			temp->push_back(seg);
			map.associate(end, *temp);
		}
		for (int j = 0; j < seg.attractions.size(); j++) { //same thing except we have to loop through since there are multiple attractions
			GeoCoord attract = seg.attractions[j].geocoordinates;
			if (map.find(attract) == nullptr) {
				vector<StreetSegment> temp;
				temp.push_back(seg);
				map.associate(attract,temp);
			}
			else {
				vector<StreetSegment> *temp = map.find(attract);
				temp->push_back(seg);
				map.associate(attract, *temp);
			}
		}

	}
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	if (map.find(gc) == nullptr) { // if its a nullptr return an empty vector
		vector<StreetSegment> temptyv;
		return temptyv;
	}
	return *(map.find(gc));
		
}

//******************** SegmentMapper functions ********************************



SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
