#ifndef MINUTIAE_FEATURES_H
#define MINUTIAE_FEATURES_H
#include "opencv2/core/core.hpp"
#include "math.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include "minutie.h"

class Minutiae_Features : public std::vector<Minutie>
{
};

#endif // MINUTIAE_FEATURES_H