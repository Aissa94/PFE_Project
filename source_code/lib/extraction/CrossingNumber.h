#ifndef CROSSINGNUMBER_H_INCLUDED
#define CROSSINGNUMBER_H_INCLUDED

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "Minutiae.h"

namespace CrossingNumber {

	std::vector<Minutiae> getMinutiae(cv::Mat& im, int border);

	void filterMinutiae(std::vector<Minutiae>& minutiae, double distanceThreshBetweenMinutiaes = 7.0);

	double euclideanDistance(int x1, int y1, int x2, int y2);

}

#endif // CROSSINGNUMBER_H_INCLUDED
