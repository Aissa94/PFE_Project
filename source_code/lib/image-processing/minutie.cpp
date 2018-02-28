#include "minutie.h"

int x; 
int y;
double or;
double dir;
bool end;

Minutie::Minutie()
{

}

Minutie::Minutie(int _x, int _y, double _or, double _dir, bool _end)
{
	x = _x;
	y = _y;
	or = _or;
	dir = _dir;
	end = _end;
}

int Minutie::get_x(){ return x; }
int Minutie::get_y(){ return y; }
double Minutie::get_or(){ return or; }
double Minutie::get_dir(){ return dir; }
bool Minutie::isEnd(){ return end; }

double Minutie::Eucld_Distance(Minutie minutie)
{
	return sqrt(pow((x - minutie.get_x()), 2) + pow((y - minutie.get_y()), 2));
}

