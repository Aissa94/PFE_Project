#ifndef MINUTIAE_H
#define MINUTIAE_H
#include "opencv2/core/core.hpp"
#include "math.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

class Minutiae:public cv::KeyPoint
{
public:
		enum Type { RIDGEENDING = 0, BIFURCATION = 1 };
		Minutiae();
		Minutiae(int x, int y, double or, double dir, Type type);
		virtual ~Minutiae();
		double getOr();
		double getDir();
        Type getType();
        void setMarkTrue();
        bool getMark();
		double euclideanDistance(Minutiae minutiae);
private:
		double or;
		double dir;
        Type type;
        bool markedForErasing;
};

#endif // MINUTIAE_H
