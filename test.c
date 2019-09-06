// This file is part of utm.

// (c) Copyright 2019 Miguel Aguiar.
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

#include "utm/utm.h"
#include <stdio.h>

#include "greatest.h"

#define TEST_TOLERANCE_DEG (1e-6)
#define TEST_TOLERANCE_M (0.01)

TEST test_utm_to_lat_lon_n(void)
{
	double lat, lon;
	int const sh = 0;

	ASSERT_EQ(utm_to_lat_lon(234000, 712398, 24, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(6.439349839009083, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-41.40485722864011, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(498129, 3908457, 3, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(35.319332918415085, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-165.02058402610695, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(649282, 1293870, 54, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(11.701152956338074, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(142.3697214371168, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(344509, 90812, 12, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(0.8213581392892807, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-112.397361571286, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(240989, 1298731, 26, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(11.738499978895081, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-29.37642755394301, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(500918, 5001989, 29, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(45.1713809074954, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-8.988317635735969, lon, TEST_TOLERANCE_DEG);

	PASS();
}

TEST test_utm_to_lat_lon_s(void)
{
	double lat, lon;
	int const sh = 1;

	ASSERT_EQ(utm_to_lat_lon(364980, 1239888, 6, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(-78.84668384971482, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-153.2641590470919, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(801239, 8102939, 48, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(-17.13840803300152, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(107.83117176701103, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(350029, 2193879, 17, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(-70.31677158840408, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-84.99200976423859, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(698711, 4028939, 27, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(-53.84996759976053, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(-17.97896312219287, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(246098, 9007879, 44, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(-8.968079052679851, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(78.6907948671293, lon, TEST_TOLERANCE_DEG);

	ASSERT_EQ(utm_to_lat_lon(355987, 3451980, 60, sh, &lat, &lon), 0);
	ASSERT_IN_RANGE(-59.047252269304884, lat, TEST_TOLERANCE_DEG);
	ASSERT_IN_RANGE(174.4895290221281, lon, TEST_TOLERANCE_DEG);

	PASS();
}

TEST test_utm_to_lat_lon_invalid(void)
{
	double lat, lon;

	ASSERT_EQ(utm_to_lat_lon(355987, 3451980, 60, 1, &lat, NULL), -1);
	ASSERT_EQ(utm_to_lat_lon(355987, 3451980, 60, 1, NULL, &lon), -1);
	ASSERT_EQ(utm_to_lat_lon(355987, 3451980, 60, 1, NULL, NULL), -1);

	PASS();
}

SUITE(test_utm_to_lat_lon)
{
	RUN_TEST(test_utm_to_lat_lon_n);
	RUN_TEST(test_utm_to_lat_lon_s);
	RUN_TEST(test_utm_to_lat_lon_invalid);
}

TEST test_lat_lon_to_utm_nozone(void)
{
	double easting, northing;

	ASSERT_EQ(
	    lat_lon_to_utm(-28.234982, 79.293801, NULL, &easting, &northing),
	    44);
	ASSERT_IN_RANGE(332593.76, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(6875587.59, northing, TEST_TOLERANCE_M);

	ASSERT_EQ(
	    lat_lon_to_utm(89.123980, 1.238790, NULL, &easting, &northing), 31);
	ASSERT_IN_RANGE(496994.11, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(9900204.20, northing, TEST_TOLERANCE_M);

	ASSERT_EQ(
	    lat_lon_to_utm(29.109890, -9.237811, NULL, &easting, &northing),
	    29);
	ASSERT_IN_RANGE(476861.73, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(3220183.95, northing, TEST_TOLERANCE_M);

	ASSERT_EQ(
	    lat_lon_to_utm(34.123080, 19.237891, NULL, &easting, &northing),
	    34);
	ASSERT_IN_RANGE(337498.55, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(3777205.02, northing, TEST_TOLERANCE_M);

	ASSERT_EQ(
	    lat_lon_to_utm(-33.298711, 127.000999, NULL, &easting, &northing),
	    52);
	ASSERT_IN_RANGE(313878.33, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(6313814.18, northing, TEST_TOLERANCE_M);

	ASSERT_EQ(
	    lat_lon_to_utm(60.109830, 18.238791, NULL, &easting, &northing),
	    34);
	ASSERT_IN_RANGE(346526.84, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(6666849.93, northing, TEST_TOLERANCE_M);

	PASS();
}

TEST test_lat_lon_to_utm_wzone(void)
{
	double easting, northing;
	int zone;

	zone = 53;
	ASSERT_EQ(
	    lat_lon_to_utm(87.012113, 133.198711, &zone, &easting, &northing),
	    53);
	ASSERT_IN_RANGE(489518.85, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(9664537.05, northing, TEST_TOLERANCE_M);

	zone = 8;
	ASSERT_EQ(
	    lat_lon_to_utm(45.333988, -134.982133, &zone, &easting, &northing),
	    8);
	ASSERT_IN_RANGE(501399.99, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(5020053.48, northing, TEST_TOLERANCE_M);

	zone = 45;
	ASSERT_EQ(
	    lat_lon_to_utm(-27.298790, 89.011000, &zone, &easting, &northing),
	    45);
	ASSERT_IN_RANGE(699015.55, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(6978868.08, northing, TEST_TOLERANCE_M);

	zone = 32;
	ASSERT_EQ(
	    lat_lon_to_utm(-78.123978, 11.037809, &zone, &easting, &northing),
	    32);
	ASSERT_IN_RANGE(546806.68, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(1326979.69, northing, TEST_TOLERANCE_M);

	zone = 29;
	ASSERT_EQ(
	    lat_lon_to_utm(32.871032, -10.923898, &zone, &easting, &northing),
	    29);
	ASSERT_IN_RANGE(320002.44, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(3638630.26, northing, TEST_TOLERANCE_M);

	zone = 1;
	ASSERT_EQ(
	    lat_lon_to_utm(0.129899, -178.129381, &zone, &easting, &northing),
	    1);
	ASSERT_IN_RANGE(374320.30, easting, TEST_TOLERANCE_M);
	ASSERT_IN_RANGE(14360.55, northing, TEST_TOLERANCE_M);

	PASS();
}

TEST test_lat_lon_to_utm_invalid(void)
{
	double easting, northing;
	int zone;

	ASSERT_EQ(lat_lon_to_utm(0.129899, -178.129381, NULL, NULL, &northing),
		  -1);
	ASSERT_EQ(lat_lon_to_utm(0.129899, -178.129381, NULL, &easting, NULL),
		  -1);
	ASSERT_EQ(lat_lon_to_utm(0.129899, -178.129381, NULL, NULL, NULL), -1);

	zone = 0;
	ASSERT_EQ(lat_lon_to_utm(0.129899, -178.129381, &zone, NULL, NULL), -1);

	zone = 78;
	ASSERT_EQ(lat_lon_to_utm(0.129899, -178.129381, &zone, NULL, NULL), -1);

	PASS();
}

SUITE(test_lat_lon_to_utm)
{
	RUN_TEST(test_lat_lon_to_utm_nozone);
	RUN_TEST(test_lat_lon_to_utm_wzone);
	RUN_TEST(test_lat_lon_to_utm_invalid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
	GREATEST_MAIN_BEGIN();

	RUN_SUITE(test_utm_to_lat_lon);
	RUN_SUITE(test_lat_lon_to_utm);

	GREATEST_MAIN_END();
}
