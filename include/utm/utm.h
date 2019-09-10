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

#ifndef UTM_HEADER_GUARD_
#define UTM_HEADER_GUARD_

// Converts a latitude/longitude pair to x and y coordinates in the
// Universal Transverse Mercator projection.
//
// Inputs:
// 	lat	Latitude of the point, in degrees.
// 	lon	Longitude of the point, in degrees.
// 	zone	UTM zone to be used for calculating values for x and y.
// 		If zone is null, the routine will determine the appropriate zone
// 		from the value of lon.
//
// Outputs:
// 	x	The x coordinate (easting) of the computed point. (in meters)
// 	y	The y coordinate (northing) of the computed point. (in meters)
//
// Returns:
// 	The UTM zone, or -1 if x or y is null or the passed zone is invalid.
int lat_lon_to_utm(
    double lat, double lon, int const *zone, double *easting, double *northing);

// Converts x and y coordinates in the Universal Transverse Mercator
// projection to a latitude/longitude pair.
//
// Inputs:
// 	x		The easting of the point, in meters.
// 	y		The northing of the point, in meters.
// 	zone		The UTM zone in which the point lies.
// 	southhemi	Greater than zero if the point is in the south
// 			hemisphere.
//
// Outputs:
// 	lat	The latitude of the point, in degrees.
// 	lon	The longitude of the point, in degrees.
//
// Returns:
// 	Zero, or -1 if lat or lon is null.
int utm_to_lat_lon(double easting,
		   double northing,
		   int zone,
		   int southhemi,
		   double *lat,
		   double *lon);

#endif
