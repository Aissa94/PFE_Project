#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H
#include "minutiae_features.h"
#include "opencv2/core/core.hpp"
#include "math.h"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

class Image_processing
{
public:
    Image_processing();
//    void Real_Mat_size(cv::Mat mt,int &real_cols, int &real_rows);
    Minutiae_Features Final_Minutiae_Set_Extraction (cv::Mat img);
    cv::Mat Image_Transform (cv::Mat img,double tx, double ty,double d_or, double scale);
    void Draw_minutiae_Features (bool col, cv::Mat &img, Minutiae_Features minutiae);

    cv::Mat  Gradient_Orientation_Image(cv::Mat img, int bloc_size);
    cv::Mat  Image_Enhancement(cv::Mat img,int bloc_size,double sigma_x,double sigma_y, int gabor_filter_size);
	cv::Mat  Image_Binarisation(cv::Mat img, double threshold);
	cv::Mat  Print_Segmentation(cv::Mat img);
	cv::Mat  Image_Thinnig(cv::Mat img, bool black);
	Minutiae_Features Minutia_Extraction(cv::Mat img, cv::Mat oimg, int bloc_size);
	void Minutiae_Validation(cv::Mat img, Minutiae_Features &minutiae, cv::Mat msk);
	cv::Mat static thinning(cv::Mat input, cv::Mat &enhancedImage, cv::Mat &segmentedImage);
};

#endif // IMAGE_PROCESSING_H
