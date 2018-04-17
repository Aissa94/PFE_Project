#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "include_files.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
	Ui::MainWindow *ui;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
	virtual void wheelEvent(QWheelEvent* event);
    
private slots:
	void on_firstImgBtn_pressed();

    void on_secondImgBtn_pressed();

	void on_refreshBddImageNames_pressed();

	void on_pushButton_pressed();

    void on_actionDestroy_All_Windows_triggered();

    void on_actionRun_triggered();

    void on_actionClear_Log_triggered();
    
	void on_actionSave_Log_File_As_triggered();

    void on_actionAbout_Qt_triggered();

	void on_actionAbout_Me_triggered();

	void on_refreshRankkGraph_pressed();

	void on_refreshEerGraph_pressed();

	void displayMatches(int bestImgIndex = 0); 
	
	void showRankkToolTip(QMouseEvent *event);

	void showEerToolTip(QVector<double>xFMRsplined, QVector<double>yFMRsplined, QVector<double>xFNMRsplined, QVector<double>yFNMRsplined, QMouseEvent *event);


private:
    QString outputImagesPath;

	void resetParams();
    void runSIFT();
    void runSURF();
    void runORB();
	void runBRISK();
	void runCustom();
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
	double kMeans(std::vector<cv::Mat> features_vector, int k);
	void clustering();
	float testInReverse(std::vector<cv::DMatch> directMatches, std::vector<cv::DMatch> inverseMatches, std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints, float limitDistance, std::vector<cv::DMatch> &bestMatches, std::vector<cv::DMatch> &badMatches);
	float testOfLowe(std::vector<std::vector<cv::DMatch>> twoMatches, float lowesRatio, float limitDistance, std::vector<cv::DMatch> &bestMatches, std::vector<cv::DMatch> &badMatches);
	template <typename T>
	void writeKeyPoints(cv::Mat img, std::vector<T> keyPoints, int first_second, std::string fileName = "", int squareSize = 5);
	void writeMatches(int imgIndex = 0);
	void displayImage(cv::Mat imageMat, int first_second);
	void displayFeature(cv::Mat featureMat, int first_second);

	QImage matToQImage(const cv::Mat& mat);
	QString getCurrentTime();

	void customisingBinarization(int segmentationIndex);
	void customisingSegmentor(int segmentationIndex);
	void customisingDetector(int detectorIndex, std::string detectorName);
	void customisingDescriptor(int descriptorIndex, std::string descriptorName);
	void customisingMatcher(int matcherIndex, std::string matcherName);
	bool matching();
	void outlierElimination();
	void maskMatchesByTrainImgIdx(const std::vector<cv::DMatch> matches, int trainImgIdx, std::vector<char>& mask);
	int computeRankK(float scoreThreshold);
	cv::Mat maskMatchesByMinutiaeNature(std::vector<Minutiae> firstImgKeypoints, std::vector<Minutiae> secondImgKeypoints);
	void showDecision();

	int fileCounter(std::string dir, std::string prefix, std::string suffix, std::string extension);
	bool fileExistenceCheck(const std::string& name);
	void showError(std::string title, std::string text, std::string e_msg = "");

	void drowRankk(int maxRank);
	void drowEer(std::vector <std::tuple<float, int, int>> FMR_dataFromExcel, std::vector <std::tuple<float, int, int>> FNMR_dataFromExcel);
	void makePlot();
};

#endif // MAINWINDOW_H
