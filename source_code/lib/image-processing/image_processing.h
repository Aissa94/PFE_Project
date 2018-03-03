#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H
#include "opencv2/core/core.hpp"
#include "math.h"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "extraction\Minutiae.h"

class Image_processing
{
public:
	Image_processing();
	//    void Real_Mat_size(cv::Mat mt,int &real_cols, int &real_rows);
	std::vector<Minutiae> Final_Minutiae_Set_Extraction(cv::Mat img);
	cv::Mat Image_Transform(cv::Mat img, double tx, double ty, double d_or, double scale);
	void Draw_minutiae_Features(bool col, cv::Mat &img, std::vector<Minutiae> minutiae);

	//cv::Mat  Gradient_Orientation_Image(cv::Mat img, int bloc_size);
	// cv::Mat Image_Enhancement(cv::Mat img, int bloc_size, double sigma_x, double sigma_y, int gabor_filter_size);
	cv::Mat static thinning(cv::Mat input, cv::Mat &enhancedImage, cv::Mat &segmentedImage);
};

#endif // IMAGE_PROCESSING_H
