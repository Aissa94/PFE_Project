#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtWidgets>

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Additional libraries
// http://www.codelooker.com/id/217/1100103.html
#include "lib/binarisation/AdaptiveLocalThreshold.h"
#include "lib/optimisation/Ideka.h"
#include "lib/thinning/ZhangSuen.h"
#include "lib/thinning/GuoHall.h"
#include "lib/extraction/CrossingNumber.h"
#include "lib/extraction/Filter.h"

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
	void calculateBestMatches();
	void writeToFile(std::string fileName, cv::Algorithm * algoToWrite);
	bool noKeyPoints(std::string rank, std::vector<cv::KeyPoint> imgKeypoints);
	int  getNormByText(std::string norm);
	cv::Ptr<cv::flann::IndexParams> getFlannBasedIndexParamsType();
	cv::Mat skeletonization(cv::Mat img);
	void harrisCorners(cv::Mat thinnedImage, std::vector<cv::KeyPoint> &keypoints, float threshold = 125.0);
	void clusteringIntoKClusters(std::vector<cv::Mat> features_vector, int k);
	void visualizingMinutiae(cv::Mat img, std::vector<Minutiae> minutiae, std::string stepName = "3-minutiae");
	void calculateMinutiaeMagnitudeAngle(std::vector<Minutiae> minutiaes, std::vector<float> &magnitudes, std::vector<float> &angles);
	std::vector<cv::KeyPoint> getKeypointsFromMinutiae(std::vector<Minutiae> minutiaes);
};

#endif // MAINWINDOW_H
