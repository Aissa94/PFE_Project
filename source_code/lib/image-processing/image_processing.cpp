// By Mrs. Faiçal

#include "image_processing.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "iomanip"
using namespace std;

const double PI_2 = atan(1.0)*2.0;
const double PI = atan(1.0)*4.0;
const double PI_4 = atan(1.0);
const double PI3_4 = PI_2 + PI_4;

Image_processing::Image_processing()
{

}



void Save_Ort_Mat_File(cv::Mat mt, string file_name){

	ofstream myfile;
	myfile.open(file_name.c_str());
	if (myfile){
		for (int i = 0; i<mt.rows; i++){
			for (int j = 0; j<mt.cols; j++)
				myfile << setprecision(8) << ((mt.at<double>(i, j) * 180) / PI) << " | ";
			myfile << "\n";
		}
		myfile.close();
	}
	else{
		cout << "ERROR0";
	}

}


//---------------------------------------------------------------------------
//-------------Calculate Real Mat Size (for binarized Mat)-------------------
void Real_Mat_size(cv::Mat mt, int &real_cols, int &real_rows){
	int _scol = mt.cols;

	int _gcol = 0;
	int _grow = 0;
	int  _srow = mt.rows;
	switch (mt.type()){
	case CV_8U:
		for (int i = 0; i< mt.rows; i++){
			for (int j = 0; j< mt.cols; j++){
				if (mt.at<uchar>(i, j) != 255){
					if (i< _srow) _srow = i;
					if (i> _grow) _grow = i;
					if (j< _scol) _scol = j;
					if (j> _gcol) _gcol = j;
				}
			}
		}
		//cout << "CHR" << endl;
		break;
	case CV_64F:
		for (int i = 0; i< mt.rows; i++){
			for (int j = 0; j< mt.cols; j++){
				if (mt.at<double>(i, j) != 0){
					if (i< _srow) _srow = i;
					if (i> _grow) _grow = i;
					if (j< _scol) _scol = j;
					if (j> _gcol) _gcol = j;
				}
			}
		}
		//cout << "double" << endl;
		break;
	default: cout << "ERREUR" << endl;
		break;
	}
	real_cols = _gcol - _scol;
	real_rows = _grow - _srow;

}

int Max_indice_f(cv::Mat vec){
	int j = 0;
	int size = vec.cols*vec.rows*vec.channels();
	float* p = (float*)vec.data;

	for (int i = 0; i<size; i++){
		if (p[i]> p[j])
		{
			j = i;
			//   cout << p[i] << "   " << p[j];
		}
	}

	return j;
}
//---------------------------------------------------------------------------------
//**************************   NORMALISATION **************************************
//---------------------------------------------------------------------------------

cv::Mat  Normalize_Image(cv::Mat img, int dmean, int dvar){
	/*-------------------------------------------------------------------------------------
	Normalisation: an input fingerprint is normalised so that it has a pre-specified
	mean and variance
	normalisation is a pixel-wise operation. It does not change the clarity of the ridge and
	furrow structures. The main purpose of normalizatoin is to reduce the variantions in grey
	level values along ridges and furrows, which facilitates the subsequent processing steps.
	---------------------------------------------------------------------------------------*/
	cv::Mat nimg = img.clone();
	cv::Scalar mean, var;
	cv::meanStdDev(nimg, mean, var);

	double v = pow((double)var(0), 2);

	double* p = (double*)nimg.data;


	int col = nimg.cols;
	int row = nimg.rows;
	int size = col*row;

	if (!nimg.isContinuous()){
		for (int i = 0; i< size; i++)
		{
			if (p[i] >mean(0))
				p[i] = (double)(dmean + sqrt(dvar* pow((p[i] - mean(0)), 2) / v));
			else
				p[i] = (double)(dmean - sqrt(dvar* pow((p[i] - mean(0)), 2) / v));
		}
	}
	else {
		for (int i = 0; i<nimg.rows; i++)
			for (int j = 0; j<nimg.cols; j++)
			{
				if (nimg.at<double>(i, j)> mean(0))
					nimg.at<double>(i, j) = (double)(dmean + sqrt(dvar* pow((nimg.at<double>(i, j) - mean(0)), 2) / v));
				else
					nimg.at<double>(i, j) = (double)(dmean - sqrt(dvar* pow((nimg.at<double>(i, j) - mean(0)), 2) / v));
			}
	}

	return nimg;
}

//---------------------------------------------------------------------------------
//**************************   IMAGE BINARIZATION *********************************
//---------------------------------------------------------------------------------
cv::Mat Image_Binarisation(cv::Mat img, double threshold){
	/*
	Return a binary Image ( CV_8UC1 )
	*/
	cv::Mat _img;
	cv::Mat bimg = cv::Mat(img.size(), CV_8UC1);
	int size = img.rows* img.cols;

	double* p = (double*)img.data;
	uchar* t = (uchar*)bimg.data;

	if (img.isContinuous() && bimg.isContinuous()){
		for (int k = 0; k< size; k++){
			if (p[k] >= threshold) t[k] = 1;
			else t[k] = 0;
		}
	}
	else {
		for (int i = 0; i< img.rows; i++)
			for (int j = 0; j <img.cols; j++)
				if (img.at<double>(i, j)> threshold)
					bimg.at<uchar>(i, j) = 1;
				else
					bimg.at<uchar>(i, j) = 0;
	}
	return bimg;
}

//---------------------------------------------------------------------------------
//***********************   IMAGE SEGMENTATION  ***********************************
//---------------------------------------------------------------------------------

//------------------Small blocks remover ------------------------------------------

void removeSmallBlobs(cv::Mat& im, double size)
{
	/*
	* Replacement for Matlab's bwareaopen() that removes from a binary image all
	* connected components that have fewer pixels than size
	* Input image must be 8 bits, 1 channel, black and white (objects)
	* with values 0 and 255 respectively
	*/

	// Only accept CV_8UC1
	if (im.channels() != 1 || im.type() != CV_8U)
		return;
	// Find all contours
	std::vector<std::vector<cv::Point> > contours;

	cv::findContours(im.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
	{
		// Calculate contour area
		double area = cv::contourArea(contours[i]);

		// Remove small objects by drawing the contour with black color
		if (area > 0 && area <= size)
			cv::drawContours(im, contours, i, CV_RGB(0, 0, 0), -1);
	}
}

//------------------OTSU Thresholding for gray level Image----------------------------

double getThreshVal_Otsu_8u(const cv::Mat& _src)
{
	/*
	%Otsu, A Threshold Selection Method from Gray-Level Histogram, IEEE Trans.
	%on Systems, Man and Cybernetics, 1979
	%OpenCV Implementation ( ..\thresh.cpp)
	*/
	cv::Size size = _src.size();
	if (_src.isContinuous())
	{
		size.width *= size.height;
		size.height = 1;
	}
	const int N = 256;
	int i, j, h[N] = { 0 };
	for (i = 0; i < size.height; i++)
	{
		const uchar* src = _src.data + _src.step*i;
		j = 0;
#if CV_ENABLE_UNROLLED
		for (; j <= size.width - 4; j += 4)
		{
			int v0 = src[j], v1 = src[j + 1];
			h[v0]++; h[v1]++;
			v0 = src[j + 2]; v1 = src[j + 3];
			h[v0]++; h[v1]++;
		}
#endif
		for (; j < size.width; j++)
			h[src[j]]++;
	}

	double mu = 0, scale = 1. / (size.width*size.height);
	for (i = 0; i < N; i++)
		mu += i*(double)h[i];

	mu *= scale;
	double mu1 = 0, q1 = 0;
	double max_sigma = 0, max_val = 0;

	for (i = 0; i < N; i++)
	{
		double p_i, q2, mu2, sigma;

		p_i = h[i] * scale;
		mu1 *= q1;
		q1 += p_i;
		q2 = 1. - q1;

		if (std::min(q1, q2) < FLT_EPSILON || std::max(q1, q2) > 1. - FLT_EPSILON)
			continue;

		mu1 = (mu1 + i*p_i) / q1;
		mu2 = (mu - q1*mu1) / q2;
		sigma = q1*q2*(mu1 - mu2)*(mu1 - mu2);
		if (sigma > max_sigma)
		{
			max_sigma = sigma;
			max_val = i;
		}
	}

	return max_val;
}

//---------------------------Print Segmentation -------------------------------
cv::Mat Print_Segmentation(cv::Mat img){

	cv::Mat msk, _img2, _img, _img2u;
	img.convertTo(_img, CV_64FC1, 1.0 / 255.0);

	const int ITR = 4;

	cv::Mat _struct_elt_5(5, 5, CV_64FC1, cv::Scalar::all(1));

	for (int i = 0; i<ITR; i++){

		cv::erode(_img, _img, _struct_elt_5, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, 1);

		cv::pow(_img, 2, _img2);
		_img2.convertTo(_img2u, CV_8U, 255.0 / 1.0);

		cv::threshold(_img2u, msk, 1, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

		if (i == 2)
		{
			cv::Mat _msk = msk.clone();
			int _area = (int)(0.1*img.cols*img.rows);



			removeSmallBlobs(msk, _area);


			msk = 1 - msk;

			cv::bitwise_and(_msk, msk, msk);
			cv::Scalar _sum = cv::sum(_img);
			_img.setTo(_sum(0) / (_img.cols*_img.rows), msk); // a revoir


		}


	}
	int sizea, sizeb = 0;
	cv::Mat _mskt = msk.clone();

	_mskt = 255 - _mskt;
	Real_Mat_size(_mskt, sizea, sizeb);

	//cout << "nombre col image : " << img.cols << " | nombre colonne real: " << sizea << endl;

	//cout << "nombre rows image : " << img.rows << " | nombre rows real: " << sizeb << endl;

	int _sarea = (int)(0.15*sizea*sizeb);
	removeSmallBlobs(msk, _sarea);


	cv::Mat _struct_elt_7(7, 7, CV_8U, cv::Scalar::all(1));
	cv::erode(msk, msk, _struct_elt_7, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, 1);

	int holes = cv::countNonZero(msk) / 100;

	msk = 1 - msk;
	removeSmallBlobs(msk, holes); // c'est a definir
	msk = 1 - msk;

	//      cv::floodFill(msk, cv::Point(0,img.cols-1), 255);
	for (int i = 0; i<msk.rows; i++){
		msk.at<uchar>(i, 0) = 0;
		msk.at<uchar>(i, _img.cols - 1) = 0;
	}
	for (int j = 0; j<msk.cols; j++){
		msk.at<uchar>(0, j) = 0;
		msk.at<uchar>(_img.rows - 1, j) = 0;
	}


	return msk;

}


//---------------------------------------------------------------------------------
//**************************   ORIENTATION   **************************************
//---------------------------------------------------------------------------------

//----------------  Sobel Gradient Image ------------------------------------------
cv::Mat Sobel_Gradient_Image(cv::Mat img){
	cv::Mat  GX, GY, GXY;
	cv::Point P(-1, -1);
	cv::Vec3d   X(1, 2, 1);
	cv::Vec3d   Y(1, 0, -1);

	cv::sepFilter2D(img, GX, img.depth(), Y, X, P, 0, cv::BORDER_CONSTANT);
	cv::sepFilter2D(img, GY, img.depth(), X, Y, P, 0, cv::BORDER_CONSTANT);

	vector<cv::Mat> m;
	m.push_back(GX);  m.push_back(GY);
	cv::merge(m, GXY);

	return GXY;
}
//----------------------------------------------------------------------------------

//------------------- Gadient Orientation Base -------------------------------------
cv::Mat Orientation_Image(cv::Mat GMat, int bloc_size){
	cv::Mat BG;
	double* p = (double*)GMat.data;

	int size = GMat.cols*GMat.rows*GMat.channels();
	double x, y;
	if (GMat.isContinuous()) {
		int i = 0;

		while (i<size - 1){
			x = p[i]; y = p[i + 1];
			p[i] = x*y * 2;
			p[i + 1] = (x*x) - (y*y);
			i = i + 2;
		}
	}

	else {
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		for (int i = 0; i<GMat.rows; i++)
			for (int j = 0; j<GMat.cols; j++){
				x = GMat.at<cv::Vec2d>(i, j)[0];
				y = GMat.at<cv::Vec2d>(i, j)[1];
				GMat.at<cv::Vec2d>(i, j)[0] = x*y * 2;
				GMat.at<cv::Vec2d>(i, j)[1] = (x*x) - (y*y);

			}
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++
	int ncol = GMat.cols / bloc_size;
	int nrow = GMat.rows / bloc_size;
	cv::Mat oimg(nrow, ncol, CV_64FC1);
	double* t = (double*)oimg.data;
	cv::Scalar teta;

	for (int i = 0; i< nrow; i++)
	{
		for (int j = 0; j <ncol; j++)
		{
			BG = GMat(cv::Range(bloc_size*i, bloc_size*(i + 1)), cv::Range(bloc_size*j, bloc_size*(j + 1)));
			teta = cv::sum(BG);
			*t = (double) 0.5* ((double)atan2(teta(0), teta(1))) + PI_2;
			t++;

		}
	}
	return oimg;
}
//------------------------------------------------------------------------------------------

//-----------------------------Smooth Orientation ------------------------------------------
void  Smooth_Orientation(cv::Mat &oimg){
	cv::Mat teta(oimg.rows, oimg.cols, CV_64FC2);
	cv::Mat Blur_teta;
	int size = oimg.rows*oimg.cols;

	double* p = (double*)oimg.data;

	if (oimg.isContinuous() && teta.isContinuous()){
		double* t = (double*)teta.data;
		for (int i = 0; i<size; i++)
		{
			t[2 * i] = cos(p[i] * 2);
			t[2 * i + 1] = sin(p[i] * 2);
		}
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	else{
		for (int i = 0; i<oimg.rows; i++){
			for (int j = 0; j<oimg.cols; j++)
			{
				teta.at<cv::Vec2d>(i, j)[0] = (double)cos(2 * oimg.at<double>(i, j));
				teta.at<cv::Vec2d>(i, j)[1] = (double)sin(2 * oimg.at<double>(i, j));
			}
		}
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	cv::GaussianBlur(teta, Blur_teta, cv::Size(5, 5), 0.5, 0.5, cv::BORDER_CONSTANT);

	if ((Blur_teta.isContinuous()) && (oimg.isContinuous()))
	{
		double* t = (double*)Blur_teta.data;

		for (int i = 0; i<size; i++){
			p[i] = 0.5* atan2(t[2 * i + 1], t[2 * i]);
		}
	}
	else {
		double _or;
		for (int i = 0; i<Blur_teta.rows; i++)
			for (int j = 0; j<Blur_teta.cols; j++){
				_or = 0.5* atan2(Blur_teta.at<cv::Vec2d>(i, j)[1], Blur_teta.at<cv::Vec2d>(i, j)[0]);
				if (_or < 0) _or = _or + PI;
				oimg.at<double>(i, j) = _or;

			}
	}
	return;
}
//----------------------------------------------------------------------------------

//--------------------  Gadient Orientation Image ----------------------------------
cv::Mat  /*Image_processing::*/Gradient_Orientation_Image(cv::Mat img, int bloc_size){
	/*-------------------------------------------------------------------------------
	The orientation image represents an intrinsic property of the fingerprint images
	and defines invariant coordinates for ridges and furrows in a local neighborhood
	--------------------------------------------------------------------------------*/

	cv::Mat  GM = Sobel_Gradient_Image(img);
	cv::Mat  oimg = Orientation_Image(GM, bloc_size);

	Smooth_Orientation(oimg);
	return oimg;
}
//---------------------------------------------------------------------------------

//-------------------------Drawing Orientation ------------------------------------
cv::Mat Draw_Orientation(cv::Mat img, cv::Mat Omat, int blc_size){
	cv::Mat dimg = img.clone();

	int col = Omat.cols;
	int row = Omat.rows;
	double mi_blc_size = blc_size / 2 + 0.5;

	cv::Point2d a, b;
	for (int i = 0; i <= row; i++){
		cv::line(dimg, cv::Point(0, i*blc_size), cv::Point(col*blc_size, i*blc_size), CV_RGB(255, 0, 0), 1);
		for (int j = 0; j<col; j++)
			cv::line(dimg, cv::Point(j*blc_size, 0), cv::Point(j*blc_size, row*blc_size), CV_RGB(255, 0, 0), 1);
	}

	for (int i = 0; i<row; i++){
		for (int j = 0; j<col; j++){
			double theta = 0 - (Omat.at<double>(i, j));
			if (theta > PI_2) theta = theta - PI;
			if (theta <= -PI_2)theta = theta + PI;
			if ((theta >= 0) && (theta <= PI_4)){

				a.x = blc_size*j;
				a.y = blc_size*i + mi_blc_size + (mi_blc_size*tan(theta));
				b.x = blc_size*(j + 1);
				b.y = blc_size*i + mi_blc_size - (mi_blc_size*tan(theta));

				cv::line(dimg, a, b, CV_RGB(255, 0, 0), 1);
			}
			else {
				if ((theta>PI_4) && (theta <= PI_2)){
					a.x = blc_size*j + mi_blc_size + (mi_blc_size*tan(PI_2 - theta));
					a.y = blc_size*i;
					b.x = blc_size*j + mi_blc_size - (mi_blc_size*tan(PI_2 - theta));
					b.y = blc_size*(i + 1);
					cv::line(dimg, a, b, CV_RGB(0, 0, 255), 1);
				}

				else{
					if ((theta <0) && (theta >= -PI_4)){

						a.x = blc_size*j;
						a.y = blc_size*i + mi_blc_size - (mi_blc_size*tan(-theta));
						b.x = blc_size*(j + 1);
						b.y = blc_size*i + mi_blc_size + (mi_blc_size*tan(-theta));
						cv::line(dimg, a, b, CV_RGB(0, 255, 0), 1);
					}
					else {
						if ((theta <-PI_4) && (theta >= -PI_2)){
							a.x = blc_size*j + mi_blc_size - (mi_blc_size*tan(PI_2 + theta));
							a.y = blc_size*i;
							b.x = blc_size*j + mi_blc_size + (mi_blc_size*tan(PI_2 + theta));
							b.y = blc_size*(i + 1);
							cv::line(dimg, a, b, CV_RGB(255, 0, 255), 1);
						}

						else{
							a.x = blc_size*j + mi_blc_size;
							a.y = blc_size*i + mi_blc_size;
							b.x = blc_size*j + mi_blc_size + 1;
							b.y = blc_size*i + mi_blc_size + 1;
							cv::line(dimg, a, b, CV_RGB(255, 0, 255), 2);
						}

					}
				}
			}
		}
	}
	return dimg;
}
//---------------------------------------------------------------------------------



//---------------------------------------------------------------------------------
//******************************   FREQUENCY **************************************
//---------------------------------------------------------------------------------

cv::Mat  Frequency_image(cv::Mat img, cv::Mat oimg, int blc_size){
	int nrow = oimg.rows;
	int ncol = oimg.cols;

	int tri_blc_size = blc_size * 3;
	cv::Mat fimg(nrow, ncol, CV_64FC1);
	cv::Mat img2, pad_img;
	img.convertTo(img2, CV_32FC1);

	cv::Mat _to_rotate_bloc, _rotated, _rotation_matrix, _cropped, foimg;
	cv::Size fbloc_size(blc_size * 2, blc_size);
	cv::Point2d _center;
	double _or;
	cv::Mat _frq, _frq0, _dft, _dft_mag;
	cv::Scalar _mfrq;
	int _frequency;

	cv::copyMakeBorder(img2, pad_img, blc_size, blc_size, blc_size, blc_size, cv::BORDER_CONSTANT, cv::Scalar::all(255));
	_center.y = tri_blc_size / 2;
	_center.x = tri_blc_size / 2;

	double t = (double)cv::getTickCount();
	for (int i = 0; i< nrow; i++)
	{
		for (int j = 0; j <ncol; j++)
		{
			_or = oimg.at<double>(i, j) * 180 / PI;

			_to_rotate_bloc = pad_img(cv::Range(blc_size*i, blc_size*i + tri_blc_size), cv::Range(blc_size*j, blc_size*j + tri_blc_size));

			_rotation_matrix = cv::getRotationMatrix2D(_center, 90 + _or, 1);
			cv::warpAffine(_to_rotate_bloc, _rotated, _rotation_matrix, _to_rotate_bloc.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, 255);

			cv::getRectSubPix(_rotated, fbloc_size, _center, _cropped);

			cv::reduce(_cropped, _frq, 0, CV_REDUCE_SUM);  //the X-Signature

			_mfrq = cv::mean(_frq);
			cv::subtract(_frq, _mfrq(0), _frq0);

			//  ++++++ Distance between peaks calculation-+++++++++++++++++++++++++++++++++++++++++++++++++++
			cv::Mat padded;
			cv::copyMakeBorder(_frq0, padded, 0, 0, 0, blc_size * 6, cv::BORDER_CONSTANT, cv::Scalar::all(0));
			cv::Mat _dfts[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };

			cv::merge(_dfts, 2, _dft);
			cv::dft(_dft, _dft, cv::DFT_COMPLEX_OUTPUT);
			cv::split(_dft, _dfts);
			cv::magnitude(_dfts[0], _dfts[1], _dft_mag);

			_frequency = Max_indice_f(_dft_mag);
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// _frequency= blc_size*8-_frequency;

			fimg.at<double>(i, j) = (double)(((double)blc_size * 8) / (double)_frequency);
		}
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//cout << "Base frequency Times passed in seconds: " << t << endl;



	//++++++++++++++++++++++++++++ Interpolate the Frequency ++++++++++++++++++++++++
	t = (double)cv::getTickCount();
	cv::Mat pad_fimg, _bloc;
	cv::Mat _nfimg(nrow, ncol, CV_64FC1);

	bool done = false;
	cv::Mat gauss_kernel_1D = cv::getGaussianKernel(7, 2);
	cv::Mat gauss_kernel = gauss_kernel_1D* gauss_kernel_1D.t();

	int _itter = 0;

	while ((!done) && (_itter <1)){
		cv::copyMakeBorder(fimg, pad_fimg, 3, 3, 3, 3, cv::BORDER_CONSTANT, cv::Scalar::all(0));

		for (int i = 0; i< nrow; i++){
			for (int j = 0; j< ncol; j++){
				if ((fimg.at<double>(i, j) >= 3) && (fimg.at<double>(i, j) <= 25))
				{
					_nfimg.at<double>(i, j) = fimg.at<double>(i, j);
				}
				else{

					double sum_gauss_bloc = 0;
					double sum_gauss = 0;
					for (int _i = 0; _i<7; _i++){
						for (int _j = 0; _j<7; _j++)
						{
							if ((pad_fimg.at<double>(i + _i, j + _j) >= 3) && (pad_fimg.at<double>(i + _i, j + _j) <= 25)){
								sum_gauss_bloc = sum_gauss_bloc + (double)(pad_fimg.at<double>(i + _i, j + _j)* gauss_kernel.at<double>(_i, _j));
								sum_gauss = sum_gauss + gauss_kernel.at<double>(_i, _j);

							}
						}
					}

					_nfimg.at<double>(i, j) = (double)sum_gauss_bloc / sum_gauss;
				}
			}
		}

		done = cv::checkRange(_nfimg, true, NULL, 3, 25);
		fimg = _nfimg;
		_itter++;
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//cout << "Iterpolation Times passed in seconds: " << t << endl;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++   Low pass filter ++++++++++++++++++++++++++++++++++++++

	cv::GaussianBlur(fimg, fimg, cv::Size(7, 7), 2, 2, cv::BORDER_REPLICATE);

	return fimg;
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//**************************   IMAGE SEGMENTATION *********************************
//---------------------------------------------------------------------------------
cv::Mat Image_Frequency_Segmentation(cv::Mat img, cv::Mat oimg, int blc_size){
	cv::Mat simg = cv::Mat(img.size(), CV_8UC1);

	int nrow = oimg.rows;
	int ncol = oimg.cols;

	int tri_blc_size = blc_size * 3;
	cv::Mat img2, pad_img;
	img.convertTo(img2, CV_32FC1);

	cv::Mat _to_rotate_bloc, _rotated, _rotation_matrix, _cropped, foimg;
	cv::Size fbloc_size(blc_size * 2, blc_size);
	cv::Point2d _center;
	double _or;
	cv::Mat _frq, _frq0, _dft, _dft_mag;
	cv::Scalar _mfrq;
	int _frequency, _wave;

	cv::copyMakeBorder(img2, pad_img, blc_size, blc_size, blc_size, blc_size, cv::BORDER_CONSTANT, cv::Scalar::all(255));
	_center.y = tri_blc_size / 2;
	_center.x = tri_blc_size / 2;


	for (int i = 0; i< nrow; i++)
	{
		for (int j = 0; j <ncol; j++)
		{
			_or = oimg.at<double>(i, j) * 180 / PI;

			_to_rotate_bloc = pad_img(cv::Range(blc_size*i, blc_size*i + tri_blc_size), cv::Range(blc_size*j, blc_size*j + tri_blc_size));

			_rotation_matrix = cv::getRotationMatrix2D(_center, 90 + _or, 1);
			cv::warpAffine(_to_rotate_bloc, _rotated, _rotation_matrix, _to_rotate_bloc.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, 255);

			cv::getRectSubPix(_rotated, fbloc_size, _center, _cropped);

			cv::reduce(_cropped, _frq, 0, CV_REDUCE_SUM);  //the X-Signature

			_mfrq = cv::mean(_frq);
			cv::subtract(_frq, _mfrq(0), _frq0);

			//  ++++++ Distance between peaks calculation-+++++++++++++++++++++++++++++++++++++++++++++++++++
			cv::Mat padded;
			cv::copyMakeBorder(_frq0, padded, 0, 0, 0, blc_size * 6, cv::BORDER_CONSTANT, cv::Scalar::all(0));
			cv::Mat _dfts[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };

			cv::merge(_dfts, 2, _dft);
			cv::dft(_dft, _dft, cv::DFT_COMPLEX_OUTPUT);
			cv::split(_dft, _dfts);
			cv::magnitude(_dfts[0], _dfts[1], _dft_mag);

			_frequency = Max_indice_f(_dft_mag);
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			// _frequency= blc_size*8-_frequency;

			_wave = (double)(((double)blc_size * 8) / (double)_frequency);
			if ((_wave >= 3) && (_wave <= 25)) simg.at<uchar>(i, j) = 1;
			else simg.at<uchar>(i, j) = 0;

		}
	}


	return simg;
}
//---------------------------------------------------------------------------------
//**************************   GABOR FILTER  **************************************
//---------------------------------------------------------------------------------

cv::Mat Do_Gabor_Filter(cv::Mat img, int bloc_size, cv::Mat oimg, cv::Mat fimg, cv::Mat msk_img, int filter_size, double sigma_x, double sigma_y){
	cv::Mat gabor_img(img.rows, img.cols, CV_64FC1);
	cv::Mat pad_img, gabor_kernel, bloc_gabor, bloc;

	int nrow = oimg.rows;
	int ncol = oimg.cols;
	int mi_bloc_size = bloc_size / 2;
	int dbl_bloc_size = bloc_size * 2;

	cv::Size fsize(filter_size, filter_size);
	double _theta, _frequency, _gamma;
	_gamma = sigma_x / sigma_y;


	cv::copyMakeBorder(img, pad_img, mi_bloc_size, mi_bloc_size, mi_bloc_size, mi_bloc_size, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	for (int i = 0; i< nrow; i++)
	{
		for (int j = 0; j <ncol; j++)
		{
			_frequency = fimg.at<double>(i, j);   //wavelength of the sinusoidal factor
			// if (msk_img.at<uchar>(i,j)==1)

			if ((_frequency >= 3) && (_frequency <= 25))
			{
				_theta = oimg.at<double>(i, j) + PI_2;  //orthogonal direction
				bloc = pad_img(cv::Range(bloc_size*i, bloc_size*i + dbl_bloc_size), cv::Range(bloc_size*j, bloc_size*j + dbl_bloc_size));
				// bloc is just a pointer
				gabor_kernel = cv::getGaborKernel(fsize, sigma_x, _theta, _frequency, _gamma, 0);

				cv::filter2D(bloc, bloc_gabor, bloc.depth(), gabor_kernel);

				for (int _i = 0; _i<bloc_size; _i++){
					for (int _j = 0; _j< bloc_size; _j++){
						double _cel = bloc_gabor.at<double>(_i + mi_bloc_size, _j + mi_bloc_size);
						gabor_img.at<double>(i*bloc_size + _i, j*bloc_size + _j) = _cel;
					}
				}
			}
			else{
				for (int _i = 0; _i<bloc_size; _i++)
					for (int _j = 0; _j< bloc_size; _j++)
						gabor_img.at<double>(i*bloc_size + _i, j*bloc_size + _j) = 255;

			}
		}
	}

	return gabor_img;
}

//---------------------------------------------------------------------------------
//**************************   IMAGE ENHANCEMENT **********************************
//---------------------------------------------------------------------------------
cv::Mat /*Image_processing::*/Image_Enhancement(cv::Mat img, int bloc_size, double sigma_x, double sigma_y, int gabor_filter_size){
	cv::Mat double_img, img2;
	//int border_top=bloc_size-(img.rows%bloc_size);
	//int border_left=bloc_size-(img.cols%bloc_size);
	int nrows = img.rows - (img.rows%bloc_size);
	int ncols = img.cols - (img.cols%bloc_size);
	//int _rows=img.rows;
	//int _cols=img.cols;
	//cv::copyMakeBorder(img,img,0,border_top,0,border_left , cv::BORDER_CONSTANT, cv::Scalar::all(0));
	// top bottom right left
	//cv::imshow("img",img);

	//cout <<img.rows <<"  "<< img.cols <<endl;
	img = img(cv::Range(0, nrows), cv::Range(0, ncols));

	//cout <<img.rows <<"  "<< img.cols <<endl;
	img.convertTo(double_img, CV_64FC1);

	//double t = (double)cv::getTickCount();
	cv::Mat nimg = Normalize_Image(double_img, 0, 100); // mean 0 variance 100
	//t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
	//cout << "Normalisation Times passed in seconds: " << t << endl;

	//t = (double)cv::getTickCount();

	cv::Mat oimg = Gradient_Orientation_Image(nimg, bloc_size);
	//t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
	//cout << "Orientation Times passed in seconds: " << t << endl;
	//   Save_Ort_Mat_File(oimg,"hi");

	//t = (double)cv::getTickCount();
	cv::Mat fimg = Frequency_image(nimg, oimg, bloc_size);
	//t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
	//cout << "Frequency Times passed in seconds: " << t << endl;

	//t = (double)cv::getTickCount();
	cv::Mat msk = Image_Frequency_Segmentation(nimg, oimg, bloc_size);
	cv::Mat enhanced_image = Do_Gabor_Filter(nimg, bloc_size, oimg, fimg, msk, gabor_filter_size, sigma_x, sigma_y);
	//t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
	//cout << "Enhancement Times passed in seconds: " << t << endl;

	//    enhanced_image=enhanced_image(cv::Range(0,_rows),cv::Range(0,_cols));
	nimg.~Mat();
	oimg.~Mat();
	fimg.~Mat();
	msk.~Mat();
	return enhanced_image;
}

//---------------------------------------------------------------------------------
//**************************   IMAGE THINNING *************************************
//---------------------------------------------------------------------------------
cv::Mat Image_Thinnig(cv::Mat img, bool black){
	/*If black==true black line are thinned nor white
	Algorithm In  :: Zhang, T. Y., & Suen, C. Y. (1984).A fast parallel algorithm for
	thinning digital patterns. Communications of the ACM, 27(3), 236-239.ISO 690 */

	cv::Mat timg = cv::Mat(img.size(), CV_8UC1);
	bool done = false;
	if (black){
		timg = 1 - img;
	}
	else
	{
		timg = img.clone();
	}

	while (!done){
		cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
		for (int i = 1; i< img.rows - 1; i++){
			for (int j = 1; j< img.cols - 1; j++){
				if ((uchar)timg.at<uchar>(i, j) == (uchar)1){
					//cout << i << j << endl;
					if ((uchar)timg.at<uchar>(i + 1, j) == (uchar)0)  //P6
					{

						bool p2 = (timg.at<uchar>(i - 1, j) == 1);
						bool p3 = (timg.at<uchar>(i - 1, j + 1) == 1);
						bool p4 = (timg.at<uchar>(i, j + 1) == 1);
						bool p5 = (timg.at<uchar>(i + 1, j + 1) == 1);
						bool p7 = (timg.at<uchar>(i + 1, j - 1) == 1);
						bool p8 = (timg.at<uchar>(i, j - 1) == 1);
						bool p9 = (timg.at<uchar>(i - 1, j - 1) == 1);

						int A = (!p2&& p3) + (!p3 && p4) + (!p4 && p5) +
							(p7 || p8) + (!p8 && p9) + (!p9  && p2);
						if (A == 1) {
							int B = p2 + p3 + p4 + p5 + p7 + p8 + p9;
							if ((B >= 2) && (B <= 6)) mask.at<uchar>(i, j) = (uchar)1;
						}
					}
					else{
						if ((uchar)timg.at<uchar>(i, j + 1) == (uchar)0) //P4=0 && P6=1
						{
							bool p2 = (timg.at<uchar>(i - 1, j) == 1);
							bool p3 = (timg.at<uchar>(i - 1, j + 1) == 1);
							bool p5 = (timg.at<uchar>(i + 1, j + 1) == 1);
							bool p7 = (timg.at<uchar>(i + 1, j - 1) == 1);
							bool p8 = (timg.at<uchar>(i, j - 1) == 1);
							bool p9 = (timg.at<uchar>(i - 1, j - 1) == 1);
							int A = (!p2&& p3) + 1 + (!p7 && p8) + (!p8 && p9) + (!p9  && p2);
							if (A == 1) {
								int B = p2 + p3 + p5 + p7 + p8 + p9 + 1;
								if ((B >= 2) && (B <= 6)) mask.at<uchar>(i, j) = (uchar)1;
							}
						}
						else { //P4=1 && P6=1

							if (((uchar)timg.at<uchar>(i, j - 1) + (uchar)timg.at<uchar>(i - 1, j)) == (uchar)0) // P8=0 P2=0
							{
								bool p5 = (timg.at<uchar>(i + 1, j + 1) == 1);
								bool p9 = (timg.at<uchar>(i - 1, j - 1) == 1);
								int A = 1 + (!p5) + (p9);

								if (A == 1)
									mask.at<uchar>(i, j) = 1;
							}

						}
					}
				}
			}
		}
		if (cv::countNonZero(mask) == 0) break;

		else timg = timg - mask;
		mask = cv::Mat::zeros(img.size(), CV_8UC1);

		for (int i = 1; i< img.rows - 1; i++){
			for (int j = 1; j< img.cols - 1; j++){
				if ((uchar)timg.at<uchar>(i, j) == 1){
					if ((uchar)timg.at<uchar>(i - 1, j) == 0)  //P2
					{


						bool p3 = (timg.at<uchar>(i - 1, j + 1) == 1);
						bool p4 = (timg.at<uchar>(i, j + 1) == 1);
						bool p5 = (timg.at<uchar>(i + 1, j + 1) == 1);
						bool p6 = (timg.at<uchar>(i + 1, j) == 1);
						bool p7 = (timg.at<uchar>(i + 1, j - 1) == 1);
						bool p8 = (timg.at<uchar>(i, j - 1) == 1);
						bool p9 = (timg.at<uchar>(i - 1, j - 1) == 1);
						int A = (p3 || p4) + (!p4 && p5) + (!p5 && p6) +
							(!p6 && p7) + (!p7&& p8) + (!p8 && p9);
						if (A == 1) {
							int B = p6 + p3 + p4 + p5 + p7 + p8 + p9;
							if ((B >= 2) && (B <= 6)) mask.at<uchar>(i, j) = 1;
						}
					}
					else{
						if ((uchar)timg.at<uchar>(i, j - 1) == 0) //P2=1 && P8=0
						{
							bool p3 = (timg.at<uchar>(i - 1, j + 1) == 1);
							bool p4 = (timg.at<uchar>(i, j + 1) == 1);
							bool p5 = (timg.at<uchar>(i + 1, j + 1) == 1);
							bool p6 = (timg.at<uchar>(i + 1, j) == 1);
							bool p7 = (timg.at<uchar>(i + 1, j - 1) == 1);
							bool p9 = (timg.at<uchar>(i - 1, j - 1) == 1);
							int A = (!p3 && p4) + (!p4 && p5) + (!p5 && p6) +
								(!p6 && p7) + 1;
							if (A == 1) {
								int B = 1 + p3 + p4 + p5 + p6 + p7 + p9;
								if ((B >= 2) && (B <= 6)) mask.at<uchar>(i, j) = 1;
							}
						}
						else { //P2=1 && P8=1

							if (((uchar)timg.at<uchar>(i, j + 1) + (uchar)timg.at<uchar>(i + 1, j)) == (uchar)0) // P6=0 P4=0
							{
								bool p5 = (timg.at<uchar>(i + 1, j + 1) == 1);
								bool p9 = (timg.at<uchar>(i - 1, j - 1) == 1);

								int A = (p5)+1 + (!p9);
								if (A == 1)
									mask.at<uchar>(i, j) = 1;
							}
						}
					}
				}
			}

		}
		if (cv::countNonZero(mask) == 0)done = true;
		else timg = timg - mask;
	}
	if (black) timg = 1 - timg;

	return timg;
}

//---------------------------------------------------------------------------------
//**************************   MINUTIA EXTRACTION  ********************************
//---------------------------------------------------------------------------------
std::vector<Minutiae> Minutia_Extraction(cv::Mat img, cv::Mat oimg, int bloc_size){
	/*
	* img : thinned image
	*/

	std::vector<Minutiae> Minutia_vec;
	uchar _cross_number;
	uchar _dir_number;
	double _or, _dir;
	Minutiae *_m;


	for (int i = 1; i< img.rows - 1; i++)
		for (int j = 1; j< img.cols - 1; j++){
			if (img.at<uchar>(i, j) == 0){
				_dir_number = (img.at<uchar>(i + 1, j) != img.at<uchar>(i + 1, j - 1)) +
					(img.at<uchar>(i + 1, j - 1) != img.at<uchar>(i, j - 1)) +
					(img.at<uchar>(i, j - 1) != img.at<uchar>(i - 1, j - 1)) +
					(img.at<uchar>(i - 1, j - 1) != img.at<uchar>(i - 1, j));
				_cross_number = _dir_number +
					(img.at<uchar>(i - 1, j) != img.at<uchar>(i - 1, j + 1)) +
					(img.at<uchar>(i - 1, j + 1) != img.at<uchar>(i, j + 1)) +
					(img.at<uchar>(i, j + 1) != img.at<uchar>(i + 1, j + 1)) +
					(img.at<uchar>(i + 1, j + 1) != img.at<uchar>(i + 1, j));



				_cross_number = _cross_number / 2;
				switch (_cross_number){
				case 1:  _or = (double)oimg.at<double>(i / bloc_size, j / bloc_size);
					if (_dir_number == 2)  _dir = _or + PI;
					else {
						if (_dir_number == 1) {
							if (((img.at<uchar>(i + 1, j) == 0) && (_or<0)) || ((img.at<uchar>(i - 1, j) == 0) && (_or>0)))
								_dir = _or + PI;
							else  _dir = _or;
						}
						else _dir = _or;
					}

					_m = new Minutiae(j, i, _or, _dir, Minutiae::RIDGEENDING);
					Minutia_vec.push_back(*_m);
					break;
				case 3:  _or = (double)oimg.at<double>(i / bloc_size, j / bloc_size);
					_m = new Minutiae(j, i, _or, _or, Minutiae::BIFURCATION);
					Minutia_vec.push_back(*_m);
					break;
				default: break;
				}


			}
		}
	//cout << Minutia_vec.size()<< endl;
	return Minutia_vec;
}

void Image_processing::Draw_minutiae_Features(bool col, cv::Mat &img, std::vector<Minutiae> minutiae){

	cv::Point2d a, b;
	//cv::Scalar color(255,255,0);
	cv::Scalar color1(255, 0, 0);
	cv::Scalar color2(0, 0, 255);

	for (int i = 0; i< minutiae.size(); i++){
		Minutiae m = minutiae[i];

		cv::Point2d _center(m.pt.x, m.pt.y);
		if (col){
			cv::circle(img, _center, 3, color2, 1);
		}
		else{
			cv::circle(img, _center, 3, color1, 1);
		}
		double theta = 0 - m.getDir();
		if (theta > PI) theta = theta - 2 * PI;
		if (theta <= -PI)theta = theta + 2 * PI;
		if ((theta >= 0) && (theta <= PI_2)){

			a.x = m.pt.x;
			a.y = m.pt.y;
			b.x = m.pt.x + (8 * cos(theta));
			b.y = m.pt.y - (8 * sin(theta));

			//      cv::line(img,a,b,color,2);
		}
		else {
			if ((theta>PI_2) && (theta <= PI)){
				a.x = m.pt.x;
				a.y = m.pt.y;
				b.x = m.pt.x - (8 * cos(PI - theta));
				b.y = m.pt.y - (8 * sin(PI - theta));

				//  cv::line(img,a,b,color,2);
			}

			else{
				if ((theta <0) && (theta >= -PI_2)){

					a.x = m.pt.x;
					a.y = m.pt.y;
					b.x = m.pt.x + (8 * cos(-theta));
					b.y = m.pt.y + (8 * sin(-theta));
					// cv::line(img,a,b,color,2);
				}
				else {
					if ((theta <-PI_2) && (theta >= -PI)){
						a.x = m.pt.x;
						a.y = m.pt.y;
						b.x = m.pt.x - (8 * cos(PI + theta));
						b.y = m.pt.y + (8 * sin(PI + theta));
						//cv::line(img,a,b,color,2);
					}

					else{
						//cout << m.get_or() << "  error " << endl;
						a.x = m.pt.x;
						a.y = m.pt.y;
						b.x = m.pt.x + 8;
						b.y = m.pt.y + (8 * tan(theta));
						//   cv::line(img,a,b,CV_RGB(255,0,255),2);
					}
				}
			}
		}
		if (col){
			cv::line(img, a, b, color2, 1);
		}
		else{
			cv::line(img, a, b, color1, 1);
		}

	}
}

void Minutiae_Validation(cv::Mat img, std::vector<Minutiae> &minutiae, cv::Mat msk, double distanceThreshBetweenMinutiaes){

	//+++++++++++ elimination of bordary minutie +++++++++++++++++
	//  ofstream myfile;
	//  myfile.open ("Minutiae.txt");

	//  myfile << "minutia size before " << minutiae.size() << "\n";

	cv::Mat _struct_elt(9, 9, CV_64FC1, cv::Scalar::all(1));
	cv::erode(msk, msk, _struct_elt, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, 1);
	cv::erode(msk, msk, _struct_elt, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, 1);


	std::vector<Minutiae>::iterator it = minutiae.begin();

	while (it != minutiae.end())
	{
		Minutiae& _m = *it;
		if (msk.at<uchar>(_m.pt.y, _m.pt.x) == 0) {
			//  myfile << "x: " << _m.pt.x << " | y:  " << _m.pt.y << "\n";
			it = minutiae.erase(it);
		}
		else it++;
	}



	//minutiae.Save_to(myfile);
	//myfile << "minutia size after " << minutiae.size() << endl;

	//+++++++++++++++++++++++++ elimination of close minutie ++++++++++++++++++++++++++

	it = minutiae.begin();
	std::vector<Minutiae>::iterator jt;
	Minutiae _m, _n;
	std::vector<int> inf_dist;
	for (int i = 0; i< minutiae.size(); i++)   {
		Minutiae _m = minutiae[i];
		for (int j = i + 1; j< minutiae.size(); j++){
			Minutiae _n = minutiae[j];
			if ((_m.euclideanDistance(_n)< distanceThreshBetweenMinutiaes) && (_m.getType() == _n.getType())){
				if (std::find(inf_dist.begin(), inf_dist.end(), i) == inf_dist.end())
				{// not contain
					inf_dist.push_back(i);
				}
				if (std::find(inf_dist.begin(), inf_dist.end(), j) == inf_dist.end())
				{// not contain
					inf_dist.push_back(j);
				}
			}
		}
	}

	std::sort(inf_dist.begin(), inf_dist.end());
	//&&(_m.isEnd()==_n.isEnd())

	for (int i = 0; i<inf_dist.size(); i++){
		int _indice = inf_dist[inf_dist.size() - 1];
		inf_dist.pop_back();
		minutiae.erase(minutiae.begin() + _indice);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//myfile << "minutia size after " << minutiae.size() << endl;
	//myfile << endl<<"-------------------------------------------------------"<< endl;
	//minutiae.Save_to(myfile);

	//myfile.close();

}

std::vector<Minutiae> Image_processing::Final_Minutiae_Set_Extraction(cv::Mat img, double distanceThreshBetweenMinutiaes){

	//cv::namedWindow("image Originale");
	//cv::imshow("image Originale",img);
	// +++++++++++++++++ Enhancement  ++++++++++++++
	cv::Mat imge = Image_Enhancement(img, 16, 4, 4, 11);

	// cv::namedWindow("image Enhanced");
	// cv::imshow("image Enhanced",imge);

	// +++++++++++++++++ Binarization ++++++++++++++
	cv::Mat imgb = Image_Binarisation(imge, 0.48);

	//cv::namedWindow("image Binarized");
	//cv::Mat _imgb;
	//imgb.convertTo(_imgb,CV_64FC1);
	//cv::imshow("image Binarized",_imgb);

	// +++++++++++++++++ Segmentation ++++++++++++++
	cv::Mat  msk = Print_Segmentation(img);
	cv::Mat  _msk;
	//    msk.convertTo(_msk,CV_64F);
	//    cv::namedWindow("image Segmented");
	//    cv::imshow("image Segmented",_msk);

	msk = 1 - msk;
	cv::bitwise_or(imgb, msk, imgb);
	// +++++++++++++++++ Thining +++++++++++++++++++
	cv::Mat imgt = Image_Thinnig(imgb, true);
	//    cv::namedWindow("image Thinned");
	//    cv::imshow("image Thinned",imgt);

	cv::Mat _imgt;
	imgt.convertTo(_imgt, CV_8UC3, 255);
	//    cv::namedWindow("image Thinned");
	//    cv::imshow("image Thinned",_imgt);

	// ++++++++++++++++++Minutiae Extraction++++++++
	cv::Mat oimg = Gradient_Orientation_Image(imge, 16);
	std::vector<Minutiae> minutiae = Minutia_Extraction(imgt, oimg, 16);

	//    cv::Mat _imgt2=_imgt.clone();
	//    Draw_minutiae_Features(1,_imgt2,minutiae);
	//    cv::namedWindow("Minutiae");
	//    cv::imshow("Minutiae",_imgt2);
	//    cv::imwrite("C:/Users/Nabil/Desktop/Mme/Test/3.bmp", _imgt2);

	msk = 1 - msk;
	Minutiae_Validation(img, minutiae, msk, distanceThreshBetweenMinutiaes);

	//    cv::Mat _imgt3=_imgt.clone();
	//    Draw_minutiae_Features(1,_imgt3,minutiae);
	//    cv::namedWindow("MinutiaeV");
	//    cv::imshow("MinutiaeV",_imgt3);
	// 	  cv::imwrite("C:/Users/Nabil/Desktop/Mme/Test/3V.bmp", _imgt3);

	return minutiae;
}


cv::Mat Image_processing::Image_Transform(cv::Mat img, double tx, double ty, double d_or, double scale){
	// cv::Mat transformed=img.clone();CV_8UC1;
	cv::Mat transformed = cv::Mat(img.size(), CV_8UC1);
	double rad_or = (-(d_or* PI)) / 180;
	// cout << cos (rad_or);
	int _i, _j;
	// +++++++++ Transformation Application on Query ++++++++++

	//  cout << img.channels() <<endl;
	ofstream myfile;
	myfile.open("test1");
	int compt = 0;

	for (int i = 0; i< transformed.rows; i++){

		for (int j = 0; j< transformed.cols; j++){

			_i = (int)(scale*(cos(rad_or) * (i - tx) - sin(rad_or) * (j - ty)));
			_j = (int)(scale*(cos(rad_or) * (j - ty) + sin(rad_or) * (i - tx)));
			if (_i>0 && _j>0 && _i<img.rows && _j<img.cols){
				compt++;
				//if ((int)img.at<int>(_x,_y)!=-1)
				myfile << setprecision(8) << i << " | " << j << " :  " << _i << " | " << _j << "  |   : " << img.at<uchar>(_i, _j) << "\n";

				transformed.at<uchar>(i, j) = img.at<uchar>(_i, _j);
			}

			else {
				transformed.at<uchar>(i, j) = 255;
			}
		}
	}
	//cout << compt << endl;
	myfile.close();

	return transformed;



}


cv::Mat Image_processing::thinning(cv::Mat input, cv::Mat &enhancedImage, cv::Mat &segmentedImage){
	cv::Mat _enhancedImage;
	cv::Mat binarizedImage, _binarizedImage;
	cv::Mat  _segmentedImage;
	cv::Mat thinnedImage, _thinnedImage;
	try{
		// Run algorithms
		enhancedImage = Image_Enhancement(input, 16, 4, 4, 11);
		binarizedImage = Image_Binarisation(enhancedImage, 0.48);
		segmentedImage = Print_Segmentation(input);
	}
	catch (cv::Exception e){
		std::cout << std::endl << e.msg << std::endl;
		getchar();
		return thinnedImage;
	}
	//+++++++++++++++++++ Enhancement +++++++++++++++++++++++++++++++
	//enhancedImage.convertTo(_enhancedImage, CV_8UC3, 255);
	//imshow("image Enhanced", _enhancedImage);
	//+++++++++++++++++++ Binarization ++++++++++++++++++++++++++++++
	//binarizedImage.convertTo(_binarizedImage, CV_8UC3, 255);
	//imshow("image Binarized", _binarizedImage);
	//+++++++++++++++++++ Segmentation ++++++++++++++++++++++++++++++
	//segmentedImage.convertTo(_segmentedImage, CV_8UC3, 255);
	//imshow("image Segmented", _segmentedImage);

	segmentedImage = 1 - segmentedImage;
	try{ bitwise_or(binarizedImage, segmentedImage, binarizedImage); }
	catch (cv::Exception e){ binarizedImage.copyTo(segmentedImage, binarizedImage); }
	//+++++++++++++++++++ Thining+++++++++++++++++++++++++++++++++++
	thinnedImage = Image_Thinnig(binarizedImage, true);
	//thinnedImage.convertTo(_thinnedImage, CV_8UC3, 255);
	//imshow("image Thinned", _thinnedImage);

	return thinnedImage;
}

std::vector<Minutiae> Image_processing::extracting(cv::Mat input, cv::Mat enhancedImage, cv::Mat &segmentedImage, cv::Mat thinnedImage, double distanceThreshBetweenMinutiaes){
	// ++++++++++++++++++Minutiae Extraction++++++++
	cv::Mat oimg = Gradient_Orientation_Image(enhancedImage, 16);
	std::vector<Minutiae> minutiae = Minutia_Extraction(thinnedImage, oimg, 16);
	segmentedImage = 1 - segmentedImage;
	Minutiae_Validation(input, minutiae, segmentedImage, distanceThreshBetweenMinutiaes);

	return minutiae;
}