#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtWidgets>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
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

    void runSIFT();
    void runSURF();
    void runBRIEF();
    void runORB();
	void runBRISK();
	void runCustom();
	void runCustom_old();
	bool noKeyPoints(std::string rank, std::vector<cv::KeyPoint> imgKeypoints);
	int getNormByText(std::string norm);
	void writeToFile(std::string fileName, cv::Algorithm * algoToWrite);
};

#endif // MAINWINDOW_H
