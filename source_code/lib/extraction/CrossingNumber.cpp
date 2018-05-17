#include "CrossingNumber.h"

using namespace cv;

namespace CrossingNumber {

	std::vector<Minutiae> getMinutiae(Mat& im, int border){
		std::vector<Minutiae> minutiae;

		if(border < 1){
			//std::cout<<"The border area chosen is too small!" << std::endl;
			border = 1;
		}

		//int ridgeEndingCount = 0;
		//int bifurcationCount = 0;
		for(int i = border; i<=(im.cols-border); i++){
			for(int j = border; j<=(im.rows-border); j++){
				int cn = 0;

				//black pixel found?
				if(im.at<uchar>(cv::Point(i, j)) == 0){
					//iterate through 8-Neighborhood and count all transitions from black to white and reverse
					cn = cn + abs(im.at<uchar>(cv::Point(i-1, j-1))/255 - im.at<uchar>(cv::Point(i  , j-1))/255);
					cn = cn + abs(im.at<uchar>(cv::Point(i  , j-1))/255 - im.at<uchar>(cv::Point(i+1, j-1))/255);
					cn = cn + abs(im.at<uchar>(cv::Point(i+1, j-1))/255 - im.at<uchar>(cv::Point(i+1, j  ))/255);
					cn = cn + abs(im.at<uchar>(cv::Point(i+1, j  ))/255 - im.at<uchar>(cv::Point(i+1, j+1))/255);
					cn = cn + abs(im.at<uchar>(cv::Point(i+1, j+1))/255 - im.at<uchar>(cv::Point(i  , j+1))/255);
					cn = cn + abs(im.at<uchar>(cv::Point(i  , j+1))/255 - im.at<uchar>(cv::Point(i-1, j+1))/255);
					cn = cn + abs(im.at<uchar>(cv::Point(i-1, j+1))/255 - im.at<uchar>(cv::Point(i-1, j  ))/255);
					cn = cn + abs(im.at<uchar>(cv::Point(i-1, j  ))/255 - im.at<uchar>(cv::Point(i-1, j-1))/255);
					cn = cn/2;
					//std::cout<<"cn = " << cn << std::endl;

					if(cn == 1){
						Minutiae minut(i, j, 0, 0, Minutiae::Type::RIDGEENDING);
						minutiae.push_back(minut);
						//ridgeEndingCount++;
					}else if(cn == 3){
						Minutiae minut(i, j, 0, 0, Minutiae::Type::BIFURCATION);
						minutiae.push_back(minut);
						//bifurcationCount++;
					}
				}
			}
		}
		/*std::cout<<"Number of Ridge ending found: " << ridgeEndingCount << std::endl;
		std::cout << "Number of Bifurcation found: " << bifurcationCount << std::endl;*/
		return minutiae;
	}

	void filterMinutiae(std::vector<Minutiae>& minutiae, double distanceThreshBetweenMinutiaes){
		bool same = true;
		//calculate for every minutiae the distance to all other minutiae
		for (std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
			for (std::vector<Minutiae>::size_type j = 0; j<minutiae.size(); j++) {
				if (minutiae[j].getType() == /*Minutiae::Type::RIDGEENDING &&*/ minutiae[i].getType()/* == Minutiae::Type::RIDGEENDING*/ &&
					euclideanDistance(minutiae[i].pt.x, minutiae[i].pt.y, minutiae[j].pt.x, minutiae[j].pt.y) < distanceThreshBetweenMinutiaes){
					//... then mark minutiae for erasing
					minutiae[i].setEliminated();
					minutiae[j].setEliminated();
				}
			}
		}
		//int cnt = 0;
		std::vector<Minutiae> minutiaeNew;
		//erase the marked minutiae
		for (std::vector<Minutiae>::size_type i = 0; i<minutiae.size(); i++){
			if (!minutiae[i].isEliminated()){
				minutiaeNew.push_back(minutiae[i]);
			}/*else{
			 cnt++;
			 }*/

		}
		minutiae = minutiaeNew;
		//std::cout<<"Nb of eliminated minutiaes: " << cnt << std::endl;
	}

	double euclideanDistance(int x1, int y1, int x2, int y2){
		return sqrt(((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)));
	}
}
