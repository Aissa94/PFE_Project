#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //outputImagesPath = "/Users/elma/Desktop/FeaturePointsComparisonOutputImages";
    //outputImagesPath = QInputDialog::getText(this, "Output Images Path");

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_firstImgBtn_pressed()
{
    QString str = QFileDialog::getOpenFileName();
    if (!str.trimmed().isEmpty())
        ui->firstImgText->setText(str);
}

void MainWindow::on_secondImgBtn_pressed()
{
    QString str = QFileDialog::getOpenFileName();
    if (!str.trimmed().isEmpty())
        ui->secondImgText->setText(str);
}

void MainWindow::on_pushButton_pressed()
{
	int selectedAlgorithm = ui->tabWidget->currentIndex();
    switch(selectedAlgorithm)
    {
    case 0:
        // SIFT
        runSIFT();
        break;

    case 1:
        // SURF
        runSURF();
        break;

    case 2:
        // BRIEF
        runBRIEF();
        break;

    case 3:
        // ORB
        runORB();
        break;

	case 4:
		// BRISK
		runBRISK();
		break;
	}

}

void MainWindow::runSIFT()
{
    //QMessageBox msg; msg.setText("SIFT"); msg.exec();

    //    SIFT( int nfeatures=0, int nOctaveLayers=3,
    //              double contrastThreshold=0.04, double edgeThreshold=10,
    //              double sigma=1.6);

    //ui->logPlainText->appendPlainText("\nStarting SIFT object detection!");
    ui->logPlainText->appendHtml("<b>Staring SIFT object detection!</b>");


    // Read Parameters ...
    int nfeatures = ui->siftNumFeatText->text().toInt();
    int nOctaveLayers = ui->siftNumOctText->text().toInt();
    double contrastThreshold = ui->siftContThreshText->text().toDouble();
    double edgeThreshold = ui->siftEdgeThreshText->text().toDouble();
    double sigma = ui->siftSigmaText->text().toDouble();

    // Create SIFT Objects ...
    cv::SIFT siftDetector(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

    // Read Images ...
    cv::Mat firstImg = cv::imread( ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );
    cv::Mat secondImg = cv::imread( ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );

    if((firstImg.cols < 100) && (firstImg.rows < 100))
    {
        cv::resize(firstImg, firstImg, cv::Size(), 200/firstImg.rows, 200/firstImg.cols);
    }

    if((secondImg.cols < 100) && (secondImg.rows < 100))
    {
        cv::resize(secondImg, secondImg, cv::Size(), 200/secondImg.rows, 200/secondImg.cols);
    }

    // Check if the Images are loaded correctly ...
    if(firstImg.empty() || secondImg.empty())
    {
        ui->logPlainText->appendPlainText("Error while trying to read one of the input files!");
        return;
    }

    // Keypoints Vectors for the First & Second Image ...
    std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;

    // Detecting Keypoints ...
    siftDetector.detect(firstImg, firstImgKeypoints);

    ui->logPlainText->appendPlainText("Found " + QString::number(firstImgKeypoints.size()) + " key points in the first image!");

    siftDetector.detect(secondImg, secondImgKeypoints);

    ui->logPlainText->appendPlainText("Found " + QString::number(secondImgKeypoints.size()) + " key points in the second image!");

    // Descriptors for the First & Second Image ...
    cv::Mat firstImgDescriptor, secondImgDescriptor;

    // Computing the descriptors
    siftDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
    siftDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

    // Find the matching points
    cv::DescriptorMatcher *matcher;
    if(ui->siftBruteForceCheck->isChecked())
    {
        matcher = new cv::BFMatcher(cv::NORM_L1);
    }
    else
    {
        matcher = new cv::FlannBasedMatcher();
    }
    std::vector< cv::DMatch > firstMatches, secondMatches;
    matcher->match( firstImgDescriptor, secondImgDescriptor, firstMatches );
    matcher->match( secondImgDescriptor, firstImgDescriptor, secondMatches );

    delete matcher;

    int bestMatchesCount = 0;
    std::vector< cv::DMatch > bestMatches;

    for ( uint i=0; i<firstMatches.size(); i++ )
    {
        cv::Point matchedPt1 = firstImgKeypoints[i].pt;
        cv::Point matchedPt2 = secondImgKeypoints[firstMatches[i].trainIdx].pt;

        bool foundInReverse = false;

        for( uint j=0; j<secondMatches.size(); j++)
        {
            cv::Point tmpSecImgKeyPnt = secondImgKeypoints[j].pt;
            cv::Point tmpFrstImgKeyPntTrn = firstImgKeypoints[secondMatches[j].trainIdx].pt;
            if((tmpSecImgKeyPnt == matchedPt2) && ( tmpFrstImgKeyPntTrn == matchedPt1))
            {
                foundInReverse = true;
                break;
            }
        }

        if(foundInReverse)
        {
            bestMatches.push_back(firstMatches[i]);
            bestMatchesCount++;
        }

    }

    ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount));


    double minKeypoints = firstImgKeypoints.size() <= secondImgKeypoints.size() ?
                firstImgKeypoints.size()
              :
                secondImgKeypoints.size();

    ui->logPlainText->appendPlainText("Probability = " + QString::number((bestMatchesCount/minKeypoints)*100));

    cv::Mat bestImgMatches;
    cv::drawMatches( firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
                     bestMatches, bestImgMatches, cv::Scalar(0,255,0), cv::Scalar(0,255,0),
                     std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    cv::imwrite(QString(qApp->applicationDirPath() + "/output.png").toStdString(), bestImgMatches);
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/output.png"));

}

void MainWindow::runSURF()
{
    //QMessageBox msg; msg.setText("SURF"); msg.exec();

    //    SURF(double hessianThreshold=100,
    //                      int nOctaves=4, int nOctaveLayers=2,
    //                      bool extended=true, bool upright=false);

    //ui->logPlainText->appendPlainText("\nStarting SURF object detection!");
    ui->logPlainText->appendHtml("<b>Staring SURF object detection!</b>");

    // Read Parameters ...
    double hessainThreshold = ui->surfHessianThreshText->text().toDouble();
    int nOctaves = ui->surfNumOctavesText->text().toInt();
    int nOctaveLayers = ui->surfNumOctLayersText->text().toInt();
    bool extended = false;
    if(ui->surfExtendedText->text().trimmed().toLower() == "yes")
        extended = true;
    bool upright = false;
    if(ui->surfUprightText->text().trimmed().toLower() == "yes")
        upright = true;

    // Create SURF Objects ...
    cv::SURF surfDetector(hessainThreshold, nOctaves, nOctaveLayers, extended, upright);

    // Read Images ...
    cv::Mat firstImg = cv::imread( ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );
    cv::Mat secondImg = cv::imread( ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );

    if((firstImg.cols < 100) && (firstImg.rows < 100))
    {
        cv::resize(firstImg, firstImg, cv::Size(), 200/firstImg.rows, 200/firstImg.cols);
    }

    if((secondImg.cols < 100) && (secondImg.rows < 100))
    {
        cv::resize(secondImg, secondImg, cv::Size(), 200/secondImg.rows, 200/secondImg.cols);
    }

    // Check if the Images are loaded correctly ...
    if(firstImg.empty() || secondImg.empty())
    {
        ui->logPlainText->appendPlainText("Error while trying to read one of the input files!");
        return;
    }

    // Keypoints Vectors for the First & Second Image ...
    std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;

    // Detecting Keypoints ...
    surfDetector.detect(firstImg, firstImgKeypoints);

    ui->logPlainText->appendPlainText("Found " + QString::number(firstImgKeypoints.size()) + " key points in the first image!");

    surfDetector.detect(secondImg, secondImgKeypoints);

    ui->logPlainText->appendPlainText("Found " + QString::number(secondImgKeypoints.size()) + " key points in the second image!");

    // Descriptors for the First & Second Image ...
    cv::Mat firstImgDescriptor, secondImgDescriptor;

    // Computing the descriptors
    surfDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
    surfDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

    // Find the matching points
    cv::DescriptorMatcher *matcher;
    if(ui->siftBruteForceCheck->isChecked())
    {
        matcher = new cv::BFMatcher(cv::NORM_L1);
    }
    else
    {
        matcher = new cv::FlannBasedMatcher();
    }

    std::vector< cv::DMatch > firstMatches, secondMatches;
    matcher->match( firstImgDescriptor, secondImgDescriptor, firstMatches );
    matcher->match( secondImgDescriptor, firstImgDescriptor, secondMatches );

    delete matcher;

    int bestMatchesCount = 0;
    std::vector< cv::DMatch > bestMatches;

    for ( uint i=0; i<firstMatches.size(); i++ )
    {
        cv::Point matchedPt1 = firstImgKeypoints[i].pt;
        cv::Point matchedPt2 = secondImgKeypoints[firstMatches[i].trainIdx].pt;

        bool foundInReverse = false;

        for( uint j=0; j<secondMatches.size(); j++)
        {
            cv::Point tmpSecImgKeyPnt = secondImgKeypoints[j].pt;
            cv::Point tmpFrstImgKeyPntTrn = firstImgKeypoints[secondMatches[j].trainIdx].pt;
            if((tmpSecImgKeyPnt == matchedPt2) && ( tmpFrstImgKeyPntTrn == matchedPt1))
            {
                foundInReverse = true;
                break;
            }
        }

        if(foundInReverse)
        {
            bestMatches.push_back(firstMatches[i]);
            bestMatchesCount++;
        }

    }

    ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount));

    double minKeypoints = firstImgKeypoints.size() <= secondImgKeypoints.size() ?
                firstImgKeypoints.size()
              :
                secondImgKeypoints.size();

    ui->logPlainText->appendPlainText("Probability = " + QString::number((bestMatchesCount/minKeypoints)*100));

    cv::Mat bestImgMatches;
    cv::drawMatches( firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
                     bestMatches, bestImgMatches, cv::Scalar(0,255,0), cv::Scalar(0,255,0),
                     std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    cv::imwrite(QString(qApp->applicationDirPath() + "/output.png").toStdString(), bestImgMatches);
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/output.png"));

}

void MainWindow::runBRIEF()
{
    //QMessageBox msg; msg.setText("BRIEF"); msg.exec();

    //ui->logPlainText->appendPlainText("\nStarting BRIEF object detection!");
    ui->logPlainText->appendHtml("<b>Staring BRIEF object detection!</b>");

    // Read Parameters ...
    int descLen = ui->briefDescLenText->text().toInt();


    // Create needed objects ...
    cv::FeatureDetector *fDetector;

    switch(ui->tabWidget_3->currentIndex())
    {
	case 0: // SIFT features used for BRIEF detection ...
		{
			double brfContrastThreshold = ui->briefSiftContThreshText->text().toDouble();
			int brfEdgeThreshold = ui->briefSiftEdgeThreshText->text().toInt();
			int brfNumberFeatures = ui->briefSiftNumFeatText->text().toInt();
			int brfOctaveLayers = ui->briefSurfNumOctLayersText->text().toInt();
			double brfSigma = ui->briefSiftSigmaText->text().toDouble();
			fDetector = new cv::SIFT(brfContrastThreshold, brfEdgeThreshold, brfNumberFeatures, brfOctaveLayers, brfSigma);
			break;
		}
	case 1: // SURF features used for BRIEF detection ...
		{
			double brfHessainThreshold = ui->briefSurfHessianThreshText->text().toDouble();
			int brfNOctaves = ui->briefSurfNumOctavesText->text().toInt();
			int brfNOctaveLayers = ui->briefSurfNumOctLayersText->text().toInt();
			bool brfExtended = false;
			if (ui->briefSurfExtendedText->text().trimmed().toLower() == "yes")
				brfExtended = true;
			bool brfUpright = false;
			if (ui->briefSurfUprightText->text().trimmed().toLower() == "yes")
				brfUpright = true;
			fDetector = new cv::SURF(brfHessainThreshold, brfNOctaves, brfNOctaveLayers, brfExtended, brfUpright);
			break;
		}
    }

    cv::BriefDescriptorExtractor briefExtractor(descLen);

    // Read Images ...
    cv::Mat firstImg = cv::imread( ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );
    cv::Mat secondImg = cv::imread( ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );

    if((firstImg.cols < 100) && (firstImg.rows < 100))
    {
        cv::resize(firstImg, firstImg, cv::Size(), 200/firstImg.rows, 200/firstImg.cols);
    }

    if((secondImg.cols < 100) && (secondImg.rows < 100))
    {
        cv::resize(secondImg, secondImg, cv::Size(), 200/secondImg.rows, 200/secondImg.cols);
    }



    // Check if the Images are loaded correctly ...
    if(firstImg.empty() || secondImg.empty())
    {
        ui->logPlainText->appendPlainText("Error while trying to read one of the input files!");
        return;
    }

    // Keypoints Vectors for the First & Second Image ...
    std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;

    // Detecting Keypoints ...
    fDetector->detect(firstImg, firstImgKeypoints);

    ui->logPlainText->appendPlainText("Found " + QString::number(firstImgKeypoints.size()) + " key points in the first image!");

    fDetector->detect(secondImg, secondImgKeypoints);

    delete fDetector;

    ui->logPlainText->appendPlainText("Found " + QString::number(secondImgKeypoints.size()) + " key points in the second image!");

    if(firstImgKeypoints.size() <= 0)
    {
        ui->logPlainText->appendPlainText("Point matching can not be done because no key points detected in the first image!");
        return;
    }

    if(secondImgKeypoints.size() <= 0)
    {
        ui->logPlainText->appendPlainText("Point matching can not be done because no key points detected in the second image!");
        return;
    }


    // Descriptors for the First & Second Image ...
    cv::Mat firstImgDescriptor, secondImgDescriptor;

    // Computing the descriptors
    briefExtractor.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
    briefExtractor.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

    // Find the matching points
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector< cv::DMatch > firstMatches, secondMatches;

    //QMessageBox msg; msg.setText("Here"); msg.exec();

    matcher.match( firstImgDescriptor, secondImgDescriptor, firstMatches );
    matcher.match( secondImgDescriptor, firstImgDescriptor, secondMatches );

    int bestMatchesCount = 0;
    std::vector< cv::DMatch > bestMatches;

    for ( uint i=0; i<firstMatches.size(); i++ )
    {
        cv::Point matchedPt1 = firstImgKeypoints[i].pt;
        cv::Point matchedPt2 = secondImgKeypoints[firstMatches[i].trainIdx].pt;

        bool foundInReverse = false;

        for( uint j=0; j<secondMatches.size(); j++)
        {
            cv::Point tmpSecImgKeyPnt = secondImgKeypoints[j].pt;
            cv::Point tmpFrstImgKeyPntTrn = firstImgKeypoints[secondMatches[j].trainIdx].pt;
            if((tmpSecImgKeyPnt == matchedPt2) && ( tmpFrstImgKeyPntTrn == matchedPt1))
            {
                foundInReverse = true;
                break;
            }
        }

        if(foundInReverse)
        {
            bestMatches.push_back(firstMatches[i]);
            bestMatchesCount++;
        }

    }

    ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount));

    double minKeypoints = firstImgKeypoints.size() <= secondImgKeypoints.size() ?
                firstImgKeypoints.size()
              :
                secondImgKeypoints.size();

    ui->logPlainText->appendPlainText("Probability = " + QString::number((bestMatchesCount/minKeypoints)*100));

    cv::Mat bestImgMatches;
    cv::drawMatches( firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
                     bestMatches, bestImgMatches, cv::Scalar(0,255,0), cv::Scalar(0,255,0),
                     std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //    if((bestImgMatches.rows < 480) && (bestImgMatches.cols < 640))
    //        cv::resize(bestImgMatches, bestImgMatches, cv::Size(), 2, 2);

    // RANSAC
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    for( uint i = 0; i < bestMatches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( firstImgKeypoints[ bestMatches[i].queryIdx ].pt );
        scene.push_back( secondImgKeypoints[ bestMatches[i].trainIdx ].pt );
    }

    cv::Mat H = cv::findHomography( obj, scene, CV_LMEDS );

    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<cv::Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0);
    obj_corners[1] = cvPoint( firstImg.cols, 0 );
    obj_corners[2] = cvPoint( firstImg.cols, firstImg.rows );
    obj_corners[3] = cvPoint( 0, firstImg.rows );
    std::vector<cv::Point2f> scene_corners(4);

    cv::perspectiveTransform( obj_corners, scene_corners, H);

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
	cv::Point2f offset((float)firstImg.cols, 0);
	cv::line(bestImgMatches, scene_corners[0] + offset, scene_corners[1] + offset, cv::Scalar(255, 0, 0), 4);
	cv::line(bestImgMatches, scene_corners[1] + offset, scene_corners[2] + offset, cv::Scalar(255, 0, 0), 4);
	cv::line(bestImgMatches, scene_corners[2] + offset, scene_corners[3] + offset, cv::Scalar(255, 0, 0), 4);
	cv::line(bestImgMatches, scene_corners[3] + offset, scene_corners[0] + offset, cv::Scalar(255, 0, 0), 4);

    // End of RANSAC

    cv::imwrite(QString(qApp->applicationDirPath() + "/output.png").toStdString(), bestImgMatches);
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/output.png"));

}

void MainWindow::runORB()
{
    //QMessageBox msg; msg.setText("ORB"); msg.exec();

    //    ORB(int nfeatures = 500, float scaleFactor = 1.2f, int nlevels = 8, int edgeThreshold = 31,
    //                     int firstLevel = 0, int WTA_K=2, int scoreType=HARRIS_SCORE, int patchSize=31 );

    //ui->logPlainText->appendPlainText("\nStarting ORB object detection!");
    ui->logPlainText->appendHtml("<b>Staring ORB object detection!</b>");

    // Read Parameters ...
    int  nfeatures = ui->orbNumFeatText->text().toInt();
    float scaleFactor = ui->orbScaleFactText->text().toFloat();
    int nlevels = ui->orbNumLevelsText->text().toInt();
    int edgeThreshold = ui->orbEdgeThreshText->text().toInt();
    int firstLevel = ui->orbFirstLevText->text().toInt();
    int WTA_K = ui->orbWTAKText->text().toInt();
    int scoreType = cv::ORB::HARRIS_SCORE;
    if(ui->orbScoreHarrisRadioBtn->isChecked())
        scoreType = cv::ORB::HARRIS_SCORE;
    else if(ui->orbScoreFastRadioBtn->isChecked())
        scoreType = cv::ORB::FAST_SCORE;
    int patchSize = ui->orbPatchSizeText->text().toInt();


    // Create ORB Object ...
    cv::ORB orbDetector(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize);

    // Read Images ...
    cv::Mat firstImg = cv::imread( ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );
    cv::Mat secondImg = cv::imread( ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE );

    if((firstImg.cols < 100) && (firstImg.rows < 100))
    {
        cv::resize(firstImg, firstImg, cv::Size(), 200/firstImg.rows, 200/firstImg.cols);
    }

    if((secondImg.cols < 100) && (secondImg.rows < 100))
    {
        cv::resize(secondImg, secondImg, cv::Size(), 200/secondImg.rows, 200/secondImg.cols);
    }



    // Check if the Images are loaded correctly ...
    if(firstImg.empty() || secondImg.empty())
    {
        ui->logPlainText->appendPlainText("Error while trying to read one of the input files!");
        return;
    }

    // Keypoints Vectors for the First & Second Image ...
    std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;

    // Detecting Keypoints ...
    orbDetector.detect(firstImg, firstImgKeypoints);

    ui->logPlainText->appendPlainText("Found " + QString::number(firstImgKeypoints.size()) + " key points in the first image!");

    orbDetector.detect(secondImg, secondImgKeypoints);

    ui->logPlainText->appendPlainText("Found " + QString::number(secondImgKeypoints.size()) + " key points in the second image!");

    if(firstImgKeypoints.size() <= 0)
    {
        ui->logPlainText->appendPlainText("Point matching can not be done because no key points detected in the first image!");
        return;
    }

    if(secondImgKeypoints.size() <= 0)
    {
        ui->logPlainText->appendPlainText("Point matching can not be done because no key points detected in the second image!");
        return;
    }


    // Descriptors for the First & Second Image ...
    cv::Mat firstImgDescriptor, secondImgDescriptor;

    // Computing the descriptors
    orbDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
    orbDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

    // Find the matching points
    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector< cv::DMatch > firstMatches, secondMatches;

    //QMessageBox msg; msg.setText("Here"); msg.exec();

    matcher.match( firstImgDescriptor, secondImgDescriptor, firstMatches );
    matcher.match( secondImgDescriptor, firstImgDescriptor, secondMatches );

    int bestMatchesCount = 0;
    std::vector< cv::DMatch > bestMatches;

    for ( uint i=0; i<firstMatches.size(); i++ )
    {
        cv::Point matchedPt1 = firstImgKeypoints[i].pt;
        cv::Point matchedPt2 = secondImgKeypoints[firstMatches[i].trainIdx].pt;

        bool foundInReverse = false;

        for( uint j=0; j<secondMatches.size(); j++)
        {
            cv::Point tmpSecImgKeyPnt = secondImgKeypoints[j].pt;
            cv::Point tmpFrstImgKeyPntTrn = firstImgKeypoints[secondMatches[j].trainIdx].pt;
            if((tmpSecImgKeyPnt == matchedPt2) && ( tmpFrstImgKeyPntTrn == matchedPt1))
            {
                foundInReverse = true;
                break;
            }
        }

        if(foundInReverse)
        {
            bestMatches.push_back(firstMatches[i]);
            bestMatchesCount++;
        }

    }

    ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount));

    double minKeypoints = firstImgKeypoints.size() <= secondImgKeypoints.size() ?
                firstImgKeypoints.size()
              :
                secondImgKeypoints.size();

    ui->logPlainText->appendPlainText("Probability = " + QString::number((bestMatchesCount/minKeypoints)*100));

    cv::Mat bestImgMatches;
    cv::drawMatches( firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
                     bestMatches, bestImgMatches, cv::Scalar(0,255,0), cv::Scalar(0,255,0),
                     std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    cv::imwrite(QString(qApp->applicationDirPath() + "/output.png").toStdString(), bestImgMatches);
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/output.png"));

}

void MainWindow::runBRISK()
{
	//QMessageBox msg; msg.setText("BRISK"); msg.exec();

	//    BRISK(int thresh = 30, int octaves = 3,float patternScale = 1.0f);

	//ui->logPlainText->appendPlainText("\nStarting BRISK object detection!");
	ui->logPlainText->appendHtml("<b>Staring BRISK object detection!</b>");

	QStandardItemModel *model = new QStandardItemModel(2, 5, this); //2 Rows and 3 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Coordinate X1")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Coordinate Y1")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Coordinate X2")));
	model->setHorizontalHeaderItem(3, new QStandardItem(QString("Coordinate Y2")));
	model->setHorizontalHeaderItem(4, new QStandardItem(QString("Distance")));

	// Read Parameters ...
	float patternScale = ui->briskPatternScaleText->text().toFloat();
	int octaves = ui->briskOctavesText->text().toInt();
	int thresh = ui->briskThreshText->text().toInt();


	// Create BRISK Object ...
	cv::BRISK briskDetector(thresh, octaves, patternScale);

	// Read Images ...
	cv::Mat firstImg = cv::imread(ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat secondImg = cv::imread(ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

	if ((firstImg.cols < 100) && (firstImg.rows < 100))
	{
		cv::resize(firstImg, firstImg, cv::Size(), 200 / firstImg.rows, 200 / firstImg.cols);
	}

	if ((secondImg.cols < 100) && (secondImg.rows < 100))
	{
		cv::resize(secondImg, secondImg, cv::Size(), 200 / secondImg.rows, 200 / secondImg.cols);
	}



	// Check if the Images are loaded correctly ...
	if (firstImg.empty() || secondImg.empty())
	{
		ui->logPlainText->appendPlainText("Error while trying to read one of the input files!");
		return;
	}

	// Keypoints Vectors for the First & Second Image ...
	std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;

	// Detecting Keypoints ...
	briskDetector.detect(firstImg, firstImgKeypoints);

	ui->logPlainText->appendPlainText("Found " + QString::number(firstImgKeypoints.size()) + " key points in the first image!");

	briskDetector.detect(secondImg, secondImgKeypoints);

	ui->logPlainText->appendPlainText("Found " + QString::number(secondImgKeypoints.size()) + " key points in the second image!");

	if (firstImgKeypoints.size() <= 0)
	{
		ui->logPlainText->appendPlainText("Point matching can not be done because no key points detected in the first image!");
		return;
	}

	if (secondImgKeypoints.size() <= 0)
	{
		ui->logPlainText->appendPlainText("Point matching can not be done because no key points detected in the second image!");
		return;
	}


	// Descriptors for the First & Second Image ...
	cv::Mat firstImgDescriptor, secondImgDescriptor;
	cv::Mat firstImgDescriptorShow, secondImgDescriptorShow;

	// Computing the descriptors
	briskDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
	briskDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

	cv::drawKeypoints(firstImg, firstImgKeypoints, firstImgDescriptorShow, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
	QGraphicsScene *imgKeypoints1 = new QGraphicsScene();
	QImage imgKey1((const uchar *)firstImgDescriptorShow.data, firstImgDescriptorShow.cols, firstImgDescriptorShow.rows, firstImgDescriptorShow.step, QImage::Format_RGB888);
	QPixmap piximg1 = QPixmap::fromImage(imgKey1);
	imgKeypoints1->addPixmap(piximg1);
	ui->graphicsView_2->setScene(imgKeypoints1);
	ui->graphicsView_2->fitInView(imgKeypoints1->sceneRect(), Qt::KeepAspectRatio);

	cv::drawKeypoints(secondImg, secondImgKeypoints, secondImgDescriptorShow, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
	QGraphicsScene *imgKeypoints2 = new QGraphicsScene();
	QImage imgKey2((const uchar *)secondImgDescriptorShow.data, secondImgDescriptorShow.cols, secondImgDescriptorShow.rows, secondImgDescriptorShow.step, QImage::Format_RGB888);
	QPixmap piximg2 = QPixmap::fromImage(imgKey2);
	imgKeypoints2->addPixmap(piximg2);
	ui->graphicsView_3->setScene(imgKeypoints2);
	ui->graphicsView_3->fitInView(imgKeypoints2->sceneRect(), Qt::KeepAspectRatio);
		
	// Find the matching points
	cv::BFMatcher matcher(cv::NORM_HAMMING);
	std::vector< cv::DMatch > firstMatches, secondMatches;

	//QMessageBox msg; msg.setText("Here"); msg.exec();

	matcher.match(firstImgDescriptor, secondImgDescriptor, firstMatches);
	matcher.match(secondImgDescriptor, firstImgDescriptor, secondMatches);


	int bestMatchesCount = 0;
	std::vector< cv::DMatch > bestMatches;

	for (uint i = 0; i<firstMatches.size(); i++)
	{
		cv::Point matchedPt1 = firstImgKeypoints[i].pt;
		cv::Point matchedPt2 = secondImgKeypoints[firstMatches[i].trainIdx].pt;

		bool foundInReverse = false;

		for (uint j = 0; j<secondMatches.size(); j++)
		{
			cv::Point tmpSecImgKeyPnt = secondImgKeypoints[j].pt;
			cv::Point tmpFrstImgKeyPntTrn = firstImgKeypoints[secondMatches[j].trainIdx].pt;
			if ((tmpSecImgKeyPnt == matchedPt2) && (tmpFrstImgKeyPntTrn == matchedPt1))
			{
				foundInReverse = true;
				break;
			}
		}

		if (foundInReverse)
		{
			QStandardItem *x1 = new QStandardItem(QString::number(matchedPt1.x));
			model->setItem(bestMatchesCount, 0, x1);
			QStandardItem *y1 = new QStandardItem(QString::number(matchedPt1.y));
			model->setItem(bestMatchesCount, 1, y1);
			QStandardItem *x2 = new QStandardItem(QString::number(matchedPt2.x));
			model->setItem(bestMatchesCount, 2, x2);
			QStandardItem *y2 = new QStandardItem(QString::number(matchedPt2.y));
			model->setItem(bestMatchesCount, 3, y2);
			QStandardItem *dist = new QStandardItem(QString::number(firstMatches[i].distance));
			model->setItem(bestMatchesCount, 4, dist);
			ui->tableView->setModel(model);
			bestMatches.push_back(firstMatches[i]);
			bestMatchesCount++;
		}

	}

	ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount));

	double minKeypoints = firstImgKeypoints.size() <= secondImgKeypoints.size() ?
		firstImgKeypoints.size()
		:
		secondImgKeypoints.size();

	ui->logPlainText->appendPlainText("Probability = " + QString::number((bestMatchesCount / minKeypoints) * 100));

	cv::Mat bestImgMatches;
	cv::drawMatches(firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
		bestMatches, bestImgMatches, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 0),
		std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	cv::imwrite(QString(qApp->applicationDirPath() + "/output.png").toStdString(), bestImgMatches);
	
	QGraphicsScene *scene = new QGraphicsScene();
	QImage dest((const uchar *)bestImgMatches.data, bestImgMatches.cols, bestImgMatches.rows, bestImgMatches.step, QImage::Format_RGB888);
	//dest.bits();
	QPixmap pixmap = QPixmap::fromImage(dest);
	scene->addPixmap(pixmap);
	ui->graphicsView->setScene(scene);
	ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::on_actionDestroy_All_Windows_triggered()
{
    cv::destroyAllWindows();
}

void MainWindow::on_actionRun_triggered()
{
    switch(ui->tabWidget->currentIndex())
    {
    case 0:
        // SIFT
        runSIFT();
        break;

    case 1:
        // SURF
        runSURF();
        break;

    case 2:
        // BRIEF
        runBRIEF();
        break;

    case 3:
        // ORB
        runORB();
        break;

	case 4:
		// BRISK
		runBRISK();
		break;
    }
}

void MainWindow::on_actionClear_Log_triggered()
{
    ui->logPlainText->clear();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionAbout_Me_triggered()
{
    QMessageBox::about(this,
                       "About Us",
                       "<b>OpenCV Feature Detectors Comparison</b>"
                       "<br><br>This program uses OpenCV and Qt, and is provided as is, for educational purposes such as benchmarking of algorithms.<br>"
                       "<br>You may contact me for the source code of this program at <a href='mailto:dn_ghouila@esi.dz'>dn_ghouila@esi.dz</a>"
                       "<br><br>Thanks"
                       "<br><br>GHOUILA Nabil & BELKAID Aïssa"
					   "<br><br><a href='mailto:dn_ghouila@esi.dz'>dn_ghouila@esi.dz</a>");
}
