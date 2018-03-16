#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include <iostream>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Additional libraries
#include "include_files.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
	virtual void wheelEvent(QWheelEvent* event);
    
private slots:
	void on_firstImgBtn_pressed();

    void on_secondImgBtn_pressed();

	void on_pushButton_pressed();

    void on_actionDestroy_All_Windows_triggered();

    void on_actionRun_triggered();

    void on_actionClear_Log_triggered();
    
	void on_actionSave_Log_File_As_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionAbout_Me_triggered();

private:
    Ui::MainWindow *ui;

    QString outputImagesPath;

	void resetParams();
    void runSIFT();
    void runSURF();
    void runORB();
	void runBRISK();
	void runCustom();
	void runCustom_old();
	bool readFirstImage();
	bool readSecondImage();
	bool readSetOfImages();
	bool createTestFolder();
	void writeToFile(std::string fileName, cv::Algorithm * algoToWrite);
	bool noKeyPoints(std::string rank, std::vector<cv::KeyPoint> imgKeypoints);
	int  getNormByText(std::string norm);
	cv::Ptr<cv::flann::IndexParams> getFlannBasedIndexParamsType();
	cv::Mat skeletonization(cv::Mat img);
	void harrisCorners(cv::Mat thinnedImage, std::vector<cv::KeyPoint> &keypoints, float threshold = 125.0);
	double clusteringIntoKClusters(std::vector<cv::Mat> features_vector, int k); 
	template <typename T>
	void writeKeyPoints(cv::Mat img, std::vector<T> keyPoints, int first_second, std::string fileName = "", int squareSize = 5);
	void displayImage(cv::Mat imageMat, int first_second);
	void displayFeature(cv::Mat featureMat, int first_second);
	QImage matToQImage(const cv::Mat& mat);

	void customisingBinarization(int segmentationIndex);
	void customisingSegmentor(int segmentationIndex);
	void customisingDetector(int detectorIndex, std::string detectorName);
	void customisingDescriptor(int descriptorIndex, std::string descriptorName);
	void customisingMatcher(int matcherIndex, std::string matcherName);
	void matching();
	void calculateBestMatches();

	int fileCounter(std::string dir, std::string prefix, std::string suffix, std::string extension);
	bool FileExistenceCheck(const std::string& name);
};

#endif // MAINWINDOW_H
