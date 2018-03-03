#include "MinutiaeToKeyPointAdapter.h"

MinutiaeToKeyPointAdapter::MinutiaeToKeyPointAdapter()
{

}

void MinutiaeToKeyPointAdapter::calculateMinutiaeMagnitudeAngle(std::vector<Minutiae> minutiaes, std::vector<float> &magnitudes, std::vector<float> &angles){
		// Calculates the magnitude and angle of minutiaes.
		// getting X and Y vecors of minutiaes
		std::vector<float> minutiaeXs, minutiaeYs;
		for each (Minutiae minutiae in minutiaes)
		{
			minutiaeXs.push_back(minutiae.pt.x);
			minutiaeYs.push_back(minutiae.pt.y);
		}
		// The angles are calculated with accuracy about 0.3 degrees.For the point(0, 0), the angle is set to 0.
		adaptingTime = (double)cv::getTickCount();
		cv::cartToPolar(minutiaeXs, minutiaeYs, magnitudes, angles, true);
		adaptingTime = ((double)cv::getTickCount() - adaptingTime) / cv::getTickFrequency();
}

double MinutiaeToKeyPointAdapter::adapt(std::vector<Minutiae> &minutiaes){
	// Returns adpating time
		// Calculates the magnitude and angle of minutiaes.
		std::vector<float> magnitudes, angles;
		calculateMinutiaeMagnitudeAngle(minutiaes, magnitudes, angles);

		// Affect magnitude and angle
		for (int i = 0; i < minutiaes.size();i++)
		{
			minutiaes[i].size = magnitudes[i];
			minutiaes[i].angle = angles[i];
		}
		return adaptingTime;
}

