#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include "support.h"
using namespace std;

struct Node { 
	// a container that holds a geocoord and the distance it took to get there
	// we are going to use a priorityqueue to sort these, which sorts its objects from greatest to least
// however since we want the coord closest to the end we are going to reverse the function of the < operator
Node(GeoCoord gc, double dis) {
	geo = gc;
	distance = dis;
}
Node() {
	distance = 0;
}
GeoCoord geo;
double distance;
bool operator < (const Node &other) const { // compare it based on distance
	return distance > other.distance;
}
bool operator ==(const Node &other) const {
	return distance == other.distance;
}
};
class NavigatorImpl
{
public:
	NavigatorImpl();
	~NavigatorImpl();
	bool loadMapData(string mapFile);
	NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	AttractionMapper amapper;
	SegmentMapper segmapper;
};
NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{ // create a map loader and init our private mapper classes and return true if we can load it
	MapLoader mp;
	if (mp.load(mapFile)) {
		segmapper.init(mp);
		amapper.init(mp);
		return true;
	}
	return false;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	// A * begins
	// each "Node" is a GeoCoord at the start or end of the streetsegment
	// We will move from node to node until we are on the correct end streetsegment, at which point we will move to that GeoCoord
	// Along this way we will be keeping track of the distance or cost it took to get there
	MyMap<GeoCoord, GeoCoord> prevCoord; // associate geoCoord with the one that came before it on given path
	MyMap<GeoCoord, double> currentDistance; // associate geoCoord with the cost it took to get there

	GeoCoord startCoord;
	if (!amapper.getGeoCoord(start, startCoord)) {
		return NAV_BAD_SOURCE;
	}
	GeoCoord endCoord;
	if (!amapper.getGeoCoord(end, endCoord)) {
		return NAV_BAD_DESTINATION;
	}
	priority_queue<Node> p_queue;

	Node startNode(startCoord, distanceEarthMiles(startCoord, endCoord));
	p_queue.push(startNode);

	prevCoord.associate(startCoord, startCoord); //associate start node to itself since this will indicate when we are done later on
	currentDistance.associate(startCoord, 0); // no cost for first node

	vector<StreetSegment> endsegments = segmapper.getSegments(endCoord);

	while (!p_queue.empty()) {
		Node top = p_queue.top();
		p_queue.pop();

		if (top.geo == endCoord) { // if we at the end, we are going to have to piece together our path,backwards
			GeoCoord last = endCoord;
			bool incomplete = true;
			while(incomplete) {
				cout << "hello" << endl;
				GeoCoord secondToLast = *(prevCoord.find(last));
				bool found = false;

				vector<StreetSegment> streetsFromLast = segmapper.getSegments(endCoord);
				vector<StreetSegment> streetsFromSecondToLast = segmapper.getSegments(secondToLast);

				for (int i = 0; i < streetsFromLast.size(); i++) {
					for (int j = 0; j < streetsFromSecondToLast.size(); j++) {
						if (!found) {
							if (streetsFromLast[i].segment == streetsFromSecondToLast[j].segment) //we found the right segment
							{
								GeoSegment newS(secondToLast, last);
								NavSegment nav;
								nav.m_geoSegment = newS;
								nav.m_direction = directionOfLine(newS);
								nav.m_distance = distanceEarthMiles(last, secondToLast);
								nav.m_streetName = streetsFromLast[i].streetName;
								directions.insert(directions.begin(), nav);
								found = true; 
							}
						}
					}
				}
				last = secondToLast;
				// everything is done we are just adding turn segments
				if (*prevCoord.find(last) == last) { 
					vector<NavSegment>::iterator it = directions.begin();

					for (int i = 0; i < directions.size() - 1; i++) {
						it++;
						if (directions[i].m_streetName != directions[i + 1].m_streetName) {
							string dir;
							if (angleBetween2Lines(directions[i].m_geoSegment, directions[i + 1].m_geoSegment) > 180)
								dir = "right";
							else
								dir = "left";
							it = directions.insert(it, NavSegment(dir, directions[i + 1].m_streetName));
						}
					}
					incomplete = false;
				}
			}
			return NAV_SUCCESS;
		}



		vector<StreetSegment> topSegments = segmapper.getSegments(top.geo);
		// *** Special case for the end segments matching with our current one ***
		for (int i = 0; i < topSegments.size(); i++) {
			for (int j = 0; j < endsegments.size(); j++) {
				if (topSegments[i] == endsegments[j]) { // if we have a match
					if (!(*(prevCoord.find(top.geo)) == endCoord)) { // make sure we arent back-tracking
						 // the new cost is going to be what it took to get here, plus an estimate of what it will be to the end
						double cost = *(currentDistance.find(top.geo)) + distanceEarthMiles(top.geo, endCoord);
						if (currentDistance.find(endCoord) == nullptr || cost < *(currentDistance.find(endCoord))) { //if less efficient to get to the next node than we have already done in another path ignore it
							currentDistance.associate(endCoord, cost);  // associate our distance and put it into q
							Node finalNode(endCoord, cost);
							p_queue.push(finalNode);
							prevCoord.associate(endCoord, top.geo); // our next coord is going to be the last one 
						}
					}
				}
			}
		}
		// *** Most cases for random street segments in our algorithm ***
		for (int i = 0; i < topSegments.size(); i++) {
			StreetSegment nextStreet = topSegments[i];
			GeoSegment nextSeg = topSegments[i].segment;

			if (nextSeg.start == top.geo) { // if our start coordinate is the same as where we are, visit the end coord of that segment
				if (!(*(prevCoord.find(top.geo)) == nextSeg.end)) { // make sure we arent back-tracking
					double cost = *(currentDistance.find(top.geo)) + distanceEarthMiles(top.geo, nextSeg.end);
					if (currentDistance.find(nextSeg.end) == nullptr || cost < *(currentDistance.find(nextSeg.end))) {
						currentDistance.associate(nextSeg.end, cost);
						Node nextNode(nextSeg.end, cost);
						p_queue.push(nextNode);
						prevCoord.associate(nextSeg.end, top.geo); // our next coord is going to be the last one 
					}
				}
			}

			else if (nextSeg.end == top.geo) { // if our end coordinate is the same as where we are, visit the start coord of that segment
				if (!(*(prevCoord.find(top.geo)) == nextSeg.start)) { // make sure we arent back-tracking
					double cost = *(currentDistance.find(top.geo)) + distanceEarthMiles(top.geo, nextSeg.start);
					if (currentDistance.find(nextSeg.start) == nullptr || cost < *(currentDistance.find(nextSeg.start))) {
						currentDistance.associate(nextSeg.start, cost);
						Node nextNode(nextSeg.start, cost);
						p_queue.push(nextNode);
						prevCoord.associate(nextSeg.start, top.geo); // our next coord is going to be the last one 
					}
				}
			}
			// This last case should only happen for our start coordinate, in which case we need to visit both sides of the segment
			else {
				if (!(*(prevCoord.find(top.geo)) == nextSeg.end)) {
					double cost = *(currentDistance.find(top.geo)) + distanceEarthMiles(top.geo, nextSeg.end);
					if (currentDistance.find(nextSeg.end) == nullptr || cost < *(currentDistance.find(nextSeg.end))) {
						currentDistance.associate(nextSeg.end, cost);
						Node nextNode(nextSeg.end, cost);
						p_queue.push(nextNode);
						prevCoord.associate(nextSeg.end, top.geo);
					}
				}
				if (!(*(prevCoord.find(top.geo)) == nextSeg.start)) {
					double cost = *(currentDistance.find(top.geo)) + distanceEarthMiles(top.geo, nextSeg.start);
					if (currentDistance.find(nextSeg.start) == nullptr || cost < *(currentDistance.find(nextSeg.start))) {
						currentDistance.associate(nextSeg.start, cost);
						Node nextNode(nextSeg.start, cost);
						p_queue.push(nextNode);
						prevCoord.associate(nextSeg.start, top.geo); // our next coord is going to be the last one 
					}
				}
			}

		}
	}
	//if we reach this point it means our queue was empty and we couldnt get a route
	return NAV_NO_ROUTE;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
