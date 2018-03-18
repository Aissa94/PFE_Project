#include "CrossingNumber.h"

using namespace cv;

namespace crossingNumber {

std::vector<Minutiae> getMinutiae(Mat& im, int border){
	std::vector<Minutiae> minutiae;

	if(border < 1){
        //std::cout<<"The border area chosen is too small!" << std::endl;
        border = 2;
    }

    int ridgeEndingCount = 0;
    int bifurcationCount = 0;
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
                    ridgeEndingCount++;
                }else if(cn == 3){
					Minutiae minut(i, j, 0, 0, Minutiae::Type::BIFURCATION);
                    minutiae.push_back(minut);
                    bifurcationCount++;
                }
            }
        }
    }
    /*std::cout<<"Number of Ridge ending found: " << ridgeEndingCount << std::endl;
	std::cout << "Number of Bifurcation found: " << bifurcationCount << std::endl;*/
	return minutiae;
}

}
