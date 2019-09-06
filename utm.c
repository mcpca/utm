// This file is part of utm.

// (c) Copyright 2019 Charles Taylor, Alexander Hajnal, Miguel Aguiar.
//
// Original Javascript by Charles Taylor:
// 	http://home.hiwaay.net/~taylorc/toolbox/geography/geoutm.html
// Port to C++ by Alexander Hajnal:
// 	http://alephnull.net/software/gis/UTM/UTM.h
// This C version was adapted from the C++ version by Miguel Aguiar.
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#define _XOPEN_SOURCE 700
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "utm/utm.h"

// Ellipsoid model constants (actual values here are for WGS84)
static double const sm_a = 6378137.0;
static double const sm_b = 6356752.314;
// static double const sm_ecc_squared = 6.69437999013e-03;

static double const utm_scale_factor = 0.9996;

static double deg_to_rad(double deg) { return (deg / 180.0 * M_PI); }

static double rad_to_deg(double rad) { return (rad / M_PI * 180.0); }

// Computes the ellipsoidal distance from the equator to a point at a
// given latitude.
//
// Reference:
// 	Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
// 	GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
// 	phi	Latitude of the point, in radians.
//
// Globals:
// 	sm_a	Ellipsoid model major axis.
// 	sm_b	Ellipsoid model minor axis.
//
// Returns:
// 	The ellipsoidal distance of the point from the equator, in meters.
static double arc_length_of_meridian(double phi)
{
	/* Precalculate n */
	double const n = (sm_a - sm_b) / (sm_a + sm_b);

	/* Precalculate alpha */
	double const alpha = ((sm_a + sm_b) / 2.0) *
			     (1.0 + (pow(n, 2.0) / 4.0) + (pow(n, 4.0) / 64.0));

	/* Precalculate beta */
	double const beta = (-3.0 * n / 2.0) + (9.0 * pow(n, 3.0) / 16.0) +
			    (-3.0 * pow(n, 5.0) / 32.0);

	/* Precalculate gamma */
	double const gamma =
	    (15.0 * pow(n, 2.0) / 16.0) + (-15.0 * pow(n, 4.0) / 32.0);

	/* Precalculate delta */
	double const delta =
	    (-35.0 * pow(n, 3.0) / 48.0) + (105.0 * pow(n, 5.0) / 256.0);

	/* Precalculate epsilon */
	double const epsilon = (315.0 * pow(n, 4.0) / 512.0);

	/* Now calculate the sum of the series and return */
	return alpha *
	       (phi + (beta * sin(2.0 * phi)) + (gamma * sin(4.0 * phi)) +
		(delta * sin(6.0 * phi)) + (epsilon * sin(8.0 * phi)));
}

// Determines the central meridian for the given UTM zone.
//
// Inputs:
// 	zone	An integer value designating the UTM zone, range [1,60].
//
// Returns:
// 	The central meridian for the given UTM zone, in radians
// 	Range of the central meridian is the radian equivalent of [-177,+177].
static double utm_central_meridian(int zone)
{
	return deg_to_rad(-183.0 + (double)(6 * zone));
}

// Computes the footpoint latitude for use in converting transverse
// Mercator coordinates to ellipsoidal coordinates.
//
// Reference:
// 	Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
// 	GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
// 	y	The UTM northing coordinate, in meters.
//
// Returns:
// 	The footpoint latitude, in radians.
static double footpoint_latitude(double y)
{
	/* Precalculate n (Eq. 10.18) */
	double const n = (sm_a - sm_b) / (sm_a + sm_b);

	/* Precalculate alpha_ (Eq. 10.22) */
	/* (Same as alpha in Eq. 10.17) */
	double const alpha_ = ((sm_a + sm_b) / 2.0) *
			      (1 + (pow(n, 2.0) / 4) + (pow(n, 4.0) / 64));

	/* Precalculate y_ (Eq. 10.23) */
	double const y_ = y / alpha_;

	/* Precalculate beta_ (Eq. 10.22) */
	double const beta_ = (3.0 * n / 2.0) + (-27.0 * pow(n, 3.0) / 32.0) +
			     (269.0 * pow(n, 5.0) / 512.0);

	/* Precalculate gamma_ (Eq. 10.22) */
	double const gamma_ =
	    (21.0 * pow(n, 2.0) / 16.0) + (-55.0 * pow(n, 4.0) / 32.0);

	/* Precalculate delta_ (Eq. 10.22) */
	double const delta_ =
	    (151.0 * pow(n, 3.0) / 96.0) + (-417.0 * pow(n, 5.0) / 128.0);

	/* Precalculate epsilon_ (Eq. 10.22) */
	double const epsilon_ = (1097.0 * pow(n, 4.0) / 512.0);

	/* Now calculate the sum of the series (Eq. 10.21) */
	return y_ + (beta_ * sin(2.0 * y_)) + (gamma_ * sin(4.0 * y_)) +
	       (delta_ * sin(6.0 * y_)) + (epsilon_ * sin(8.0 * y_));
}

// Converts a latitude/longitude pair to x and y coordinates in the
// Transverse Mercator projection.  Note that Transverse Mercator is not
// the same as UTM; a scale factor is required to convert between them.
//
// Reference:
// 	Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
// 	GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
// 	phi	Latitude of the point, in radians.
// 	lambda	Longitude of the point, in radians.
// 	lambda0	Longitude of the central meridian to be used, in radians.
//
// Outputs:
// 	x	The x coordinate of the computed point.
// 	y	The y coordinate of the computed point.
//
// Returns:
// 	The function does not return a value.
void map_lat_lon_to_xy(
    double phi, double lambda, double lambda0, double *x, double *y)
{
	/* Precalculate ep2 */
	double const ep2 = (pow(sm_a, 2.0) - pow(sm_b, 2.0)) / pow(sm_b, 2.0);

	/* Precalculate nu2 */
	double const nu2 = ep2 * pow(cos(phi), 2.0);

	/* Precalculate N */
	double const N = pow(sm_a, 2.0) / (sm_b * sqrt(1 + nu2));

	/* Precalculate t */
	double const t = tan(phi);
	double const t2 = t * t;

	/* Precalculate l */
	double const l = lambda - lambda0;

	/* Precalculate coefficients for l**n in the equations below
	   so a normal human being can read the expressions for easting
	   and northing
	   -- l**1 and l**2 have coefficients of 1.0 */
	double const l3coef = 1.0 - t2 + nu2;

	double const l4coef = 5.0 - t2 + 9 * nu2 + 4.0 * (nu2 * nu2);

	double const l5coef =
	    5.0 - 18.0 * t2 + (t2 * t2) + 14.0 * nu2 - 58.0 * t2 * nu2;

	double const l6coef =
	    61.0 - 58.0 * t2 + (t2 * t2) + 270.0 * nu2 - 330.0 * t2 * nu2;

	double const l7coef =
	    61.0 - 479.0 * t2 + 179.0 * (t2 * t2) - (t2 * t2 * t2);

	double const l8coef =
	    1385.0 - 3111.0 * t2 + 543.0 * (t2 * t2) - (t2 * t2 * t2);

	/* Calculate easting (x) */
	*x = N * cos(phi) * l +
	     (N / 6.0 * pow(cos(phi), 3.0) * l3coef * pow(l, 3.0)) +
	     (N / 120.0 * pow(cos(phi), 5.0) * l5coef * pow(l, 5.0)) +
	     (N / 5040.0 * pow(cos(phi), 7.0) * l7coef * pow(l, 7.0));

	/* Calculate northing (y) */
	*y = arc_length_of_meridian(phi) +
	     (t / 2.0 * N * pow(cos(phi), 2.0) * pow(l, 2.0)) +
	     (t / 24.0 * N * pow(cos(phi), 4.0) * l4coef * pow(l, 4.0)) +
	     (t / 720.0 * N * pow(cos(phi), 6.0) * l6coef * pow(l, 6.0)) +
	     (t / 40320.0 * N * pow(cos(phi), 8.0) * l8coef * pow(l, 8.0));
}

// Converts x and y coordinates in the Transverse Mercator projection to
// a latitude/longitude pair.  Note that Transverse Mercator is not
// the same as UTM; a scale factor is required to convert between them.
//
// Reference:
// 	Hoffmann-Wellenhof, B., Lichtenegger, H., and Collins, J.,
// 	GPS: Theory and Practice, 3rd ed.  New York: Springer-Verlag Wien, 1994.
//
// Inputs:
// 	x	The easting of the point, in meters.
// 	y	The northing of the point, in meters.
// 	lambda0	Longitude of the central meridian to be used, in radians.
//
// Outputs:
// 	phi	Latitude in radians.
// 	lambda	Longitude in radians.
//
// Returns:
// 	The function does not return a value.
//
// Remarks:
// 	The local variables Nf, nuf2, tf, and tf2 serve the same purpose as
// 	N, nu2, t, and t2 in MapLatLonToXY, but they are computed with respect
// 	to the footpoint latitude phif.
//
// 	x1frac, x2frac, x2poly, x3poly, etc. are to enhance readability and
// 	to optimize computations.
static void map_xy_to_lat_lon(
    double x, double y, double lambda0, double *phi, double *lambda)
{
	/* Get the value of phif, the footpoint latitude. */
	double const phif = footpoint_latitude(y);

	/* Precalculate ep2 */
	double const ep2 = (pow(sm_a, 2.0) - pow(sm_b, 2.0)) / pow(sm_b, 2.0);

	/* Precalculate cos (phif) */
	double const cf = cos(phif);

	/* Precalculate nuf2 */
	double const nuf2 = ep2 * pow(cf, 2.0);

	/* Precalculate Nf and initialize Nfpow */
	double const Nf = pow(sm_a, 2.0) / (sm_b * sqrt(1 + nuf2));
	double Nfpow = Nf;

	/* Precalculate tf */
	double const tf = tan(phif);
	double const tf2 = tf * tf;
	double const tf4 = tf2 * tf2;

	/* Precalculate fractional coefficients for x**n in the equations
	   below to simplify the expressions for latitude and longitude. */
	double const x1frac = 1.0 / (Nfpow * cf);

	Nfpow *= Nf; /* now equals Nf**2) */
	double const x2frac = tf / (2.0 * Nfpow);

	Nfpow *= Nf; /* now equals Nf**3) */
	double const x3frac = 1.0 / (6.0 * Nfpow * cf);

	Nfpow *= Nf; /* now equals Nf**4) */
	double const x4frac = tf / (24.0 * Nfpow);

	Nfpow *= Nf; /* now equals Nf**5) */
	double const x5frac = 1.0 / (120.0 * Nfpow * cf);

	Nfpow *= Nf; /* now equals Nf**6) */
	double const x6frac = tf / (720.0 * Nfpow);

	Nfpow *= Nf; /* now equals Nf**7) */
	double const x7frac = 1.0 / (5040.0 * Nfpow * cf);

	Nfpow *= Nf; /* now equals Nf**8) */
	double const x8frac = tf / (40320.0 * Nfpow);

	/* Precalculate polynomial coefficients for x**n.
	   -- x**1 does not have a polynomial coefficient. */
	double const x2poly = -1.0 - nuf2;

	double const x3poly = -1.0 - 2 * tf2 - nuf2;

	double const x4poly = 5.0 + 3.0 * tf2 + 6.0 * nuf2 - 6.0 * tf2 * nuf2 -
			      3.0 * (nuf2 * nuf2) - 9.0 * tf2 * (nuf2 * nuf2);

	double const x5poly =
	    5.0 + 28.0 * tf2 + 24.0 * tf4 + 6.0 * nuf2 + 8.0 * tf2 * nuf2;

	double const x6poly =
	    -61.0 - 90.0 * tf2 - 45.0 * tf4 - 107.0 * nuf2 + 162.0 * tf2 * nuf2;

	double const x7poly =
	    -61.0 - 662.0 * tf2 - 1320.0 * tf4 - 720.0 * (tf4 * tf2);

	double const x8poly =
	    1385.0 + 3633.0 * tf2 + 4095.0 * tf4 + 1575 * (tf4 * tf2);

	/* Calculate latitude */
	*phi = phif + x2frac * x2poly * (x * x) +
	       x4frac * x4poly * pow(x, 4.0) + x6frac * x6poly * pow(x, 6.0) +
	       x8frac * x8poly * pow(x, 8.0);

	/* Calculate longitude */
	*lambda = lambda0 + x1frac * x + x3frac * x3poly * pow(x, 3.0) +
		  x5frac * x5poly * pow(x, 5.0) + x7frac * x7poly * pow(x, 7.0);
}

int lat_lon_to_utm(
    double lat, double lon, int const *zone, double *x, double *y)
{
	if (!x || !y)
		return -1;

	int zone_;

	if (!zone)
		zone_ = floor((lon + 180.0) / 6.0) + 1;
	else
		zone_ = *zone;

	if (zone_ < 1 || zone_ > 60)
		return -1;

	map_lat_lon_to_xy(deg_to_rad(lat),
			  deg_to_rad(lon),
			  utm_central_meridian(zone_),
			  x,
			  y);

	/* Adjust easting and northing for UTM system. */
	*x = *x * utm_scale_factor + 500000.0;
	*y = *y * utm_scale_factor;

	if (*y < 0.0)
		*y = *y + 10000000.0;

	return zone_;
}

int utm_to_lat_lon(
    double x, double y, int zone, int southhemi, double *lat, double *lon)
{
	if (!lat || !lon)
		return -1;

	x -= 500000.0;
	x /= utm_scale_factor;

	/* If in southern hemisphere, adjust y accordingly. */
	if (southhemi > 0)
		y -= 10000000.0;

	y /= utm_scale_factor;

	double const cmeridian = utm_central_meridian(zone);
	map_xy_to_lat_lon(x, y, cmeridian, lat, lon);

	*lat = rad_to_deg(*lat);
	*lon = rad_to_deg(*lon);

	return 0;
}
