
#ifndef support_h
#define support_h
#include "provided.h"

bool operator ==(const GeoCoord &a, const GeoCoord &b); 
bool  operator >(const GeoCoord &a, const GeoCoord &b); 
bool operator < (const GeoCoord &a, const GeoCoord &b); 
bool operator==(const GeoSegment& x, const GeoSegment& y);
bool operator ==(const StreetSegment &a, const StreetSegment &b);
std::string directionOfLine(const GeoSegment &gs);
#endif // !support_h


