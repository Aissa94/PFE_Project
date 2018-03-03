#ifndef CROSSINGNUMBER_H_INCLUDED
#define CROSSINGNUMBER_H_INCLUDED

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "Minutiae.h"

namespace crossingNumber {

	std::vector<Minutiae> getMinutiae(cv::Mat& im, int border);

}

#endif // CROSSINGNUMBER_H_INCLUDED
