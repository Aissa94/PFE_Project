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
	void launchInCMD();

protected:
	virtual void wheelEvent(QWheelEvent* event);
    
private slots:
	void on_firstImgBtn_pressed();

    void on_secondImgBtn_pressed();

	//void on_inputBrowse_pressed();

	void on_refreshBddImageNames_pressed();

	void on_pushButton_pressed();

    void on_actionDestroy_All_Windows_triggered();

    void on_actionRun_triggered();

	void on_actionAdd_Command_triggered();

	void on_actionDelete_All_Commands_triggered();

    void on_actionClear_Log_triggered();

    void on_actionAbout_Qt_triggered();

	void on_actionAbout_Me_triggered();

	void on_refreshRankkGraph_pressed();

	void on_refreshEerGraph_pressed();

	void displayMatches(int bestImgIndex = 0); 
	
	void showRankkToolTip(QMouseEvent *event);

	void showEerToolTip(QMouseEvent *event);


private:
    QString outputImagesPath;

	void resetParams();
	void runSIFT(int &excelColumn, int testType);
	void runSURF(int &excelColumn, int testType);
	void runORB(int &excelColumn, int testType);
	void runBRISK(int &excelColumn, int testType);
	void runDefault(int testType);
	void runCustom(int testType);
	bool readFirstImage();
	bool readSecondImage();
	bool readSetOfImages();
	//bool readInputFile();
	bool createTestFolder();
	bool noKeyPoints(std::string rank, std::vector<cv::KeyPoint> imgKeypoints);
	int  getNormByText(std::string norm);
	cv::Ptr<cv::flann::IndexParams> getFlannBasedIndexParamsType();
	QString getFlannBasedNameParamsType();
	cv::Mat skeletonization(cv::Mat img);
	void harrisCorners(cv::Mat thinnedImage, std::vector<cv::KeyPoint> &keypoints, float threshold = 125.0);
	double kMeans(std::vector<cv::Mat> features_vector, int k, int attempts);
	void clustering(int nbClusters = 40, int nbAttempts = 3);
	void maskMatchesByCluster(std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints);
	void maskMatchesByCluster(std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints, int imgIndx);
	float testInReverse(std::vector<cv::DMatch> directMatches, std::vector<cv::DMatch> inverseMatches, float limitDistance, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches);
	float testOfLowe(std::vector<std::vector<cv::DMatch>> twoMatches, float lowesRatio, float limitDistance, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches);
	float MainWindow::testRansac(std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints, float limitDistance, float confidence, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches, cv::Mat &fundamental);
	template <typename T>
	void writeKeyPoints(cv::Mat img, std::vector<T> keyPoints, int first_second, std::string fileName = "", int squareSize = 5);
	void writeMatches(int imgIndex = 0);
	void displayImage(cv::Mat imageMat, int first_second);
	void displayFeature(cv::Mat featureMat, int first_second);

	QImage matToQImage(const cv::Mat& mat);
	QString getCurrentTime();
	int segmentationNameToInt(const QString& value);
	int detectorNameToInt(const QString& value);
	int descriptorNameToInt(const QString& value);
	int matcherNameToInt(const QString& value);
	void OutliersEliminationToInt(const QString& value);
	void FlannBasedNameToIndex(const QString& value);
	void exportSuccess(int showMethod);
	void initializeTable();
	bool takeTest(int testType);
	void importExcelFile(int type);
	void setTableValue(QAxObject* sheet, int lineIndex, int columnIndex, const QString& value);
	QString GetTableValue(QAxObject* sheet, int rowIndex, int columnIndex);
	void exportTable(int rowsCount);
	void importTable(int identifierNumber);

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
	bool showDecision();

	int fileCounter(std::string dir, std::string prefix, std::string suffix, std::string extension);
	bool fileExistenceCheck(const std::string& name);
	void showError(std::string title, std::string text, std::string e_msg = "");

	void drowRankk(int maxRank);
	void drowEer(std::map<float, std::pair<int, int>> FMR_dataFromExcel, std::map<float, std::pair<int, int>> FNMR_dataFromExcel);
	void makePlot();
};

#endif // MAINWINDOW_H
