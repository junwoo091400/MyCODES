#ifndef __JunwooHWANG_geoloc_H__
#define __JunwooHWANG_geoloc_H__

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.0174532925199432957f
#define RAD_TO_DEG 57.295779513082320876f
#endif

struct geoloc {
	long lat_l;
	long lon_l;
	float lat_f;
	float lon_f;
};

geoloc Vehicle_geoloc;
geoloc Home_geoloc;

//1
// http://www.movable-type.co.uk/scripts/latlong.html
float geoDistance(struct geoloc &a, struct geoloc &b) {
	a.lat_f = ((float)a.lat_l / 10000000);
	a.lon_f = ((float)a.lon_l / 10000000);
	b.lat_f = ((float)b.lat_l / 10000000);
	a.lon_f = ((float)b.lon_l / 10000000);

	const float R = 6371000; // m
	float p1 = a.lat_f * DEG_TO_RAD;
	float p2 = b.lat_f * DEG_TO_RAD;
	float dp = (b.lat_f - a.lat_f) * DEG_TO_RAD;
	float dl = (b.lon_f - a.lon_f) * DEG_TO_RAD;

	float x = sin(dp / 2) * sin(dp / 2) + cos(p1) * cos(p2) * sin(dl / 2) * sin(dl / 2);
	float y = 2 * atan2(sqrt(x), sqrt(1 - x));

	return R * y;
}

//2
float geoBearing(struct geoloc &a, struct geoloc &b) {
	a.lat_f = ((float)a.lat_l / 10000000);
	a.lon_f = ((float)a.lon_l / 10000000);
	b.lat_f = ((float)b.lat_l / 10000000);
	a.lon_f = ((float)b.lon_l / 10000000);
	float y = sin((b.lon_f - a.lon_f)*DEG_TO_RAD) * cos(b.lat_f*DEG_TO_RAD);
	float x = cos(a.lat_f*DEG_TO_RAD)*sin(b.lat_f*DEG_TO_RAD)
		- sin(a.lat_f*DEG_TO_RAD)*cos(b.lat_f*DEG_TO_RAD)*cos((b.lon - a.lon_f)*DEG_TO_RAD);
	return atan2(y, x) * RAD_TO_DEG;
}

#endif