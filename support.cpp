#include "support.h"


bool operator ==(const GeoCoord &a, const GeoCoord &b) {
	if (a.latitude == b.latitude && a.longitude == b.longitude) { // compare on latitude and longitude
		return true;
	}
	return false;
}
bool  operator >(const GeoCoord &a, const GeoCoord &b) {
	if (a.latitude > b.latitude) { // compare latitude then longitude 
		return true;
	}
	else if (a.latitude == b.latitude) {
		if (a.longitude > b.longitude) {
			return true;
		}
	}
	return false;
}
bool operator < (const GeoCoord &a, const GeoCoord &b) {
	if (a.latitude < b.latitude) { // same compare latitude then longitude
		return true;
	}
	if (a.latitude == b.latitude) {
		if (a.longitude < b.longitude) {
			return true;
		}
	}
	return false;

}
bool operator==(const GeoSegment& x, const GeoSegment& y) {
	return (x.start == y.start && x.end == y.end);
}
bool operator ==(const StreetSegment &a, const StreetSegment &b) { // make sure name, start, and end are the saem
	if (a.streetName == b.streetName) {
		if (a.segment.start == b.segment.start) {
			if (a.segment.end == b.segment.end)
				return true;
		}
	}
	return false;
}
std::string directionOfLine(const GeoSegment &gs) { // helper function that takes a double and will return the direction
	double x = angleOfLine(gs); // based on the spec
	if (x <= 22.5 && x >= 0) {
		return "east";
	}
	else if (x > 22.5 && x <= 67.5) {
		return "northeast";
	}
	else if (x > 67.5 && x <= 112.5) {
		return "north";
	}
	else if (x > 112.5 && x <= 157.5) {
		return "northwest";
	}
	else if (x > 157.5 && x <= 202.5) {
		return "west";
	}
	else if (x > 202.5 && x <= 247.5) {
		return "southwest";
	}
	else if (x > 247.5 && x <= 292.5) {
		return "south";
	}
	else if (x > 292.5 && x <= 337.5) {
		return "southeast";
	}
	else
		return "east";
}