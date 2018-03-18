#include "Minutiae.h"

Minutiae::~Minutiae()
{
	//dtor
}

Minutiae::Minutiae()
{
	
}

Minutiae::Minutiae(int x, int y, double or, double dir, Type type)
         : or(or), dir(dir), type(type)
{
	pt.x = x;
	pt.y = y;
    Minutiae::markedForErasing = false;
}

double Minutiae::getOr()
{
	return or;
}
double Minutiae::getDir()
{
	return dir;
}

Minutiae::Type Minutiae::getType()
{
    return type;
}

void Minutiae::setMarkTrue()
{
    Minutiae::markedForErasing = true;
}

bool Minutiae::getMark()
{
    return Minutiae::markedForErasing;
}

double Minutiae::euclideanDistance(Minutiae minutiae){
	return sqrt(((pt.x - minutiae.pt.x)*(pt.x - minutiae.pt.x)) + ((pt.y - minutiae.pt.y)*(pt.y - minutiae.pt.y)));
}