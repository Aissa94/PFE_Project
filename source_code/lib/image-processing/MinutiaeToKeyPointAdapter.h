#ifndef MINUTIAETOKEYPOINTADAPTER
#define MINUTIAETOKEYPOINTADAPTER

#include "extraction\Minutiae.h"

class MinutiaeToKeyPointAdapter {
public:
	MinutiaeToKeyPointAdapter();
	double adapt(std::vector<Minutiae> &minutiaes);

private:
	double calculateMinutiaeMagnitudeAngle(std::vector<Minutiae> minutiaes, std::vector<float> &magnitudes, std::vector<float> &angles);
};

#endif // MINUTIAETOKEYPOINTADAPTER