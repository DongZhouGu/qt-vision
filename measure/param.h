#ifndef __PARAM_H
#define __PARAM_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//#define I27
#define I34

//#define NSEG_C 10
//#define NSEG_R 2

//const float param[NSEG_R][NSEG_C] = { { 0.13, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.13},
//                                      { 0.13, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.16, 0.13}, };
//const float THRES_GAIN = 2.4;
//const float THRES_GAIN = 1.8;

#ifdef I34
	const double tvecs[3] = { -4.717657202505377, -2.42591793684592, 317.2031585147339 };
	const double rvecs[3][3] =
	{ { -0.01998492421844622, 0.995161621105208, 0.09619745673996631 },
	{ 0.9991571286307771, 0.0164287956135768, 0.03761817354668284 },
	{ 0.03585575421440716, 0.09686817100545495, -0.9946511561023663 } };
	const double distCoeff[5] = { 2.216997092953576, -1589.942337200744, -0.005971402924256916, -0.01494100668058785, 0 };
	const double camatrix[3][3] = { 29350.86461758066, 0, 2115.652256048091, 0, 29354.83122013948, 1170.340649025034, 0, 0, 1 };
#endif
#ifdef I27
	const double tvecs[3] = { -3.75915532047755, -13.72324776951093, 360.7311320912808 };
	const double rvecs[3][3] =
	{ { 0.01338507312548465, 0.9816745433773945, -0.1900945309634377 },
	{ 0.998514884571165, -0.00308198722028763, 0.05439233994421533 },
	{ 0.05280970656288973, -0.1905402640901863, -0.9802578959912456 } };
	const double distCoeff[5] = { 5.317399333397013, -2377.02718184039, -0.02674880672291302, -0.01979910000036963, 0 };
	const double camatrix[3][3] = { 45130.14640702437, 0, 2078.911757124603, 0, 43762.45172284707, 2610.211508689918, 0, 0, 1 };
#endif

#endif