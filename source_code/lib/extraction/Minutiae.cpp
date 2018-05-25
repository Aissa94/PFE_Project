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
	angle = dir;
	size = 3;
	Minutiae::eliminated = false;
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

void Minutiae::setEliminated()
{
	Minutiae::eliminated = true;
}

bool Minutiae::isEliminated()
{
	return Minutiae::eliminated;
}

double Minutiae::euclideanDistance(Minutiae minutiae){
	return sqrt(((pt.x - minutiae.pt.x)*(pt.x - minutiae.pt.x)) + ((pt.y - minutiae.pt.y)*(pt.y - minutiae.pt.y)));
}