#ifndef MINUTIE_H
#define MINUTIE_H
#include "opencv2/core/core.hpp"
#include "math.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

class Minutie
{
public:
	Minutie();
	Minutie(int _x, int _y, double _or, double _dir, bool _end);
	int get_x();
	int get_y();
	double get_or();
	double get_dir();
	double Eucld_Distance(Minutie minutie);
	bool isEnd();
private:
	int x;
	int y;
	double or;
	double dir;
	bool end;
};

#endif // MINUTIE_H