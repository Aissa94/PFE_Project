#include "mainwindow.h"
#include "ui_mainwindow.h"

// Images Declaration
	cv::Mat firstImg;
	cv::Mat secondImg;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //outputImagesPath = "/Users/elma/Desktop/FeaturePointsComparisonOutputImages";
    //outputImagesPath = QInputDialog::getText(this, "Output Images Path");
	ui->descriptorFreakSelectedPairsText->setPlaceholderText("Ex: 1 2 11 22 154 256...");
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
	// Read Images ...
	firstImg = cv::imread(ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
	secondImg = cv::imread(ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

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

	// Launch the algorithm
	switch (ui->allMethodsTabs->currentIndex())
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
        // ORB
        runORB();
        break;

	case 3:
		// BRISK
		runBRISK();
		break;

    case 4:
        // BRIEF
        runBRIEF();
        break;

	default:
		// custom
		newRunCustom();
		break;
	}
}

void MainWindow::runSIFT()
{
    //QMessageBox msg; msg.setText("SIFT"); msg.exec();

    //    SIFT( int nfeatures=0, int nOctaveLayers=3,
    //              double contrastThreshold=0.04, double edgeThreshold=10,
    //              double sigma=1.6);

    //ui->logPlainText->appendPlainText("Starting SIFT object detection!");
    ui->logPlainText->appendPlainText("Starting SIFT object detection!");


    // Read Parameters ...
    int nfeatures = ui->siftNumFeatText->text().toInt();
    int nOctaveLayers = ui->siftNumOctText->text().toInt();
    double contrastThreshold = ui->siftContThreshText->text().toDouble();
    double edgeThreshold = ui->siftEdgeThreshText->text().toDouble();
    double sigma = ui->siftSigmaText->text().toDouble();

    // Create SIFT Objects ...
    cv::SIFT siftDetector(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

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

    ui->logPlainText->appendPlainText("Starting SURF object detection!");

    // Read Parameters ...
    double hessainThreshold = ui->surfHessianThreshText->text().toDouble();
    int nOctaves = ui->surfNumOctavesText->text().toInt();
    int nOctaveLayers = ui->surfNumOctLayersText->text().toInt();
	bool extended = ui->surfExtendedText->isChecked();
	bool upright = ui->surfUprightText->isChecked();

    // Create SURF Objects ...
    cv::SURF surfDetector(hessainThreshold, nOctaves, nOctaveLayers, extended, upright);

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

    //ui->logPlainText->appendPlainText("Starting BRIEF object detection!");
    ui->logPlainText->appendPlainText("Starting BRIEF object detection!");

    // Read Parameters ...
    int descLen = ui->briefDescLenText->text().toInt();


    // Create needed objects ...
    cv::FeatureDetector *fDetector;

	switch (ui->briefTabs->currentIndex())
    {
	case 0: // SIFT features used for BRIEF detection ...
		{
			double brfContrastThreshold = ui->briefSiftContThreshText->text().toDouble();
			int brfEdgeThreshold = ui->briefSiftEdgeThreshText->text().toInt();
			int brfNumberFeatures = ui->briefSiftNumFeatText->text().toInt();
			int brfOctaveLayers = ui->briefSiftNumOctText->text().toInt();
			double brfSigma = ui->briefSiftSigmaText->text().toDouble();
			fDetector = new cv::SIFT(brfContrastThreshold, brfEdgeThreshold, brfNumberFeatures, brfOctaveLayers, brfSigma);
			break;
		}
	case 1: // SURF features used for BRIEF detection ...
		{
			double brfHessainThreshold = ui->briefSurfHessianThreshText->text().toDouble();
			int brfNOctaves = ui->briefSurfNumOctavesText->text().toInt();
			int brfNOctaveLayers = ui->briefSurfNumOctLayersText->text().toInt();
			bool brfExtended = ui->briefSurfExtendedText->isChecked();
			bool brfUpright = ui->briefSurfUprightText->isChecked();
			fDetector = new cv::SURF(brfHessainThreshold, brfNOctaves, brfNOctaveLayers, brfExtended, brfUpright);
			break;
		}
    }

	cv::BriefDescriptorExtractor briefExtractor(descLen);

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

    //ui->logPlainText->appendPlainText("Starting ORB object detection!");
    ui->logPlainText->appendPlainText("Starting ORB object detection!");

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

	//ui->logPlainText->appendPlainText("Starting BRISK object detection!");
	ui->logPlainText->appendPlainText("Starting BRISK object detection!");

	QStandardItemModel *model = new QStandardItemModel(2, 5, this); //2 Rows and 3 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Coordinate X1")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Coordinate Y1")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Coordinate X2")));
	model->setHorizontalHeaderItem(3, new QStandardItem(QString("Coordinate Y2")));
	model->setHorizontalHeaderItem(4, new QStandardItem(QString("ERR")));

	// Read Parameters ...
	float patternScale = ui->briskPatternScaleText->text().toFloat();
	int octaves = ui->briskOctavesText->text().toInt();
	int thresh = ui->briskThreshText->text().toInt();


	// Create BRISK Object ...
	cv::BRISK briskDetector(thresh, octaves, patternScale);

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

	// Computing the descriptors
	briskDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
	briskDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

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
	QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/output.png"));

}

void MainWindow::newRunCustom()
{
	// Get choices
	int detectorIndex = ui->detectorTabs->currentIndex();
	int descriptorIndex = ui->descriptorTabs->currentIndex();
	int matcherIndex = ui->matcherTabs->currentIndex();
	std::string detectorName = ui->detectorTabs->tabText(ui->detectorTabs->currentIndex()).toStdString();
	std::string descriptorName = ui->descriptorTabs->tabText(ui->descriptorTabs->currentIndex()).toStdString();
	std::string matcherName = ui->matcherTabs->tabText(ui->matcherTabs->currentIndex()).toStdString();

	ui->logPlainText->appendHtml(QString::fromStdString("<b>Starting (" + detectorName + ", " + descriptorName + ", " + matcherName + ") object detection!</b>"));

	// Customising Detector...
	//cv::FeatureDetector *ptrDetector;
	cv::FeatureDetector * ptrDetector = nullptr;// = cv::FeatureDetector::create(detectorName);
	switch (detectorIndex)
	{
	case 0:{
		// STAR
		ptrDetector = new cv::StarFeatureDetector(ui->detectorStarMaxSizeText->text().toInt(),
			ui->detectorStarResponseThresholdText->text().toInt(),
			ui->detectorStarThresholdProjectedText->text().toInt(),
			ui->detectorStarThresholdBinarizedText->text().toInt(),
			ui->detectorStarSuppressNonmaxSizeText->text().toInt());
	}
		break;
	case 1:
		// FAST
		ptrDetector = new cv::FastFeatureDetector(ui->detectorFastThresholdText->text().toInt(),
			ui->detectorFastNonmaxSuppressionCheck->isChecked()/*,
			ui->detectorFastTypeText->currentIndex()*/);
	/*case 2:
		// FASTX
		cv::FASTX(ui->detectorFastThresholdText->text().toInt(),
			ui->detectorFastNonmaxSuppressionCheck->isChecked(),
			ui->detectorFastTypeText->currentIndex());*/
		break;
	case 2:
		// SIFT
		ptrDetector = new cv::SiftDescriptorExtractor(ui->detectorSiftNfeaturesText->text().toInt(),
			ui->detectorSiftNOctaveLayersText->text().toInt(),
			ui->detectorSiftContrastThresholdText->text().toFloat(),
			ui->detectorSiftEdgeThresholdText->text().toFloat(),
			ui->detectorSiftSigmaText->text().toFloat());
		break;
	case 3:
		//SURF
		ptrDetector = new cv::SurfDescriptorExtractor(ui->detectorSurfHessianThresholdText->text().toFloat(),
			ui->detectorSurfNOctavesText->text().toInt(),
			ui->detectorSurfNOctavesText->text().toInt(),
			ui->detectorSurfExtendedText->isChecked(),
			ui->detectorSurfUprightText->isChecked());
		break;
	//....
	default:
		return;
		break;
	}

	// Open the file in WRITE mode
	cv::FileStorage fsDetector("params/detector_" + detectorName + "_params.yaml", cv::FileStorage::WRITE);
	// Write the parameters
	ptrDetector->write(fsDetector);
	// fs in WRITE mode automatically released

	// Keypoints Vectors for the First & Second Image ...
	std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;

	// Detecting Keypoints ...
	ptrDetector->detect(firstImg, firstImgKeypoints);
	ptrDetector->detect(secondImg, secondImgKeypoints);

	if (noKeyPoints("first", firstImgKeypoints) || noKeyPoints("second", secondImgKeypoints)) return;

	// Customising Descriptor...
	cv::DescriptorExtractor * ptrDescriptor = nullptr;
	switch (descriptorIndex)
	{
	case 0:
	{
		// trait the descriptorFreakSelectedPairsNumbers
		std::string descriptorFreakSelectedPairsText = ui->descriptorFreakSelectedPairsText->text().toStdString();
		std::stringstream stringStream(descriptorFreakSelectedPairsText);
		// get ints from a text
		int number;
		std::vector<int> descriptorFreakSelectedPairsNumbers;
		while (stringStream >> number){
			descriptorFreakSelectedPairsNumbers.push_back(number);
		}
		for each (int var in descriptorFreakSelectedPairsNumbers)
		{
			ui->logPlainText->appendPlainText(QString::number(var));
		}
		// FREAK
		ptrDescriptor = new cv::FREAK(ui->descriptorFreakOrientationNormalizedCheck->isChecked(),
			ui->descriptorFreakScaleNormalizedCheck->isChecked(),
			ui->descriptorFreakPatternScaleText->text().toFloat(),
			ui->descriptorFreakNOctavesText->text().toFloat());
	}
		break;
	case 1:
		// BRIEF
		ptrDescriptor = new cv::BriefDescriptorExtractor(ui->descriptorBriefLengthLabel->text().toInt());
		break;
	case 2:
		break;
	//....
	default:
		break;
	}

	// Open the file in WRITE mode
	cv::FileStorage fsDescriptor("params/descriptor_" + descriptorName + "_params.yaml", cv::FileStorage::WRITE);
	// Write the parameters
	ptrDescriptor->write(fsDescriptor);
	// fs in WRITE mode automatically released

	// Descriptors for the First & Second Image ...
	cv::Mat firstImgDescriptor, secondImgDescriptor;

	// Computing the descriptors
	ptrDescriptor->compute(firstImg, firstImgKeypoints, firstImgDescriptor);
	ptrDescriptor->compute(secondImg, secondImgKeypoints, secondImgDescriptor);

	// Find the matching points
	cv::BFMatcher matcher(cv::NORM_HAMMING);
	std::vector< cv::DMatch > firstMatches, secondMatches;

	//QMessageBox msg; msg.setText("Here"); msg.exec();

	matcher.match(firstImgDescriptor, secondImgDescriptor, firstMatches);
	matcher.match(secondImgDescriptor, firstImgDescriptor, secondMatches);

	int bestMatchesCount = 0;
	std::vector< cv::DMatch > bestMatches;

	QStandardItemModel *model = new QStandardItemModel(2, 5, this); //2 Rows and 3 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Coordinate X1")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Coordinate Y1")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Coordinate X2")));
	model->setHorizontalHeaderItem(3, new QStandardItem(QString("Coordinate Y2")));
	model->setHorizontalHeaderItem(4, new QStandardItem(QString("ERR")));

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
	QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/output.png"));

}

void MainWindow::runCustom()
{
	// Get choices
	std::string detectorName = ui->detectorTabs->tabText(ui->detectorTabs->currentIndex()).toStdString();
	std::string descriptorName = ui->descriptorTabs->tabText(ui->descriptorTabs->currentIndex()).toStdString();
	std::string matcherName = ui->matcherTabs->tabText(ui->matcherTabs->currentIndex()).toStdString();

	ui->logPlainText->appendHtml(QString::fromStdString("<b>Starting ("+detectorName+", "+descriptorName+", "+matcherName+") object detection!</b>"));

	// Customising Detector...
	cv::Ptr<cv::FeatureDetector> ptrDetector = cv::FeatureDetector::create(detectorName);

	// Try to read from file
	cv::FileStorage fsDetector("params/detector_"+detectorName+"_params.yaml", cv::FileStorage::READ);
	if (fsDetector.isOpened())
	{
		// Read the parameters
		//ptrDetector->set("octaves", std::stoi(fsDetector["octaves"])); //we don't really need it
		int i = 0;
		i = i + 1;
	}
	else
	{
		int i = 0;
		i = i + 1;
		// Close the file in READ mode
		fsDetector.release();
		i = i + 2;
		// Add detector's parameters
		//ptrDetector->set("thres", 30);
		//ptrDetector->set("octaves", 3);
		//ptrDetector->set("patternScale", 1);

		// Open the file in WRITE mode
		cv::FileStorage fsDetector("params/detector_" + detectorName + "_params.yaml", cv::FileStorage::WRITE);
		// Write the parameters
		ptrDetector->write(fsDetector);
		// fs in WRITE mode automatically released
	}
	// fs in READ mode automatically released

	// Keypoints Vectors for the First & Second Image ...
	std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;

	// Detecting Keypoints ...
	ptrDetector->detect(firstImg, firstImgKeypoints);
	ptrDetector->detect(secondImg, secondImgKeypoints);

	ui->logPlainText->appendPlainText("Found " + QString::number(firstImgKeypoints.size()) + " key points in the first image!");
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

	// Customising Descriptor...
	cv::Ptr<cv::DescriptorExtractor> ptrDescriptor = cv::DescriptorExtractor::create(descriptorName);
	
	// Try to read from file
	cv::FileStorage fsDescriptor("params/descriptor_" + descriptorName + "_params.yaml", cv::FileStorage::READ);
	if (fsDescriptor.isOpened())
	{
		// Read the parameters
		//ptrDetector->set("octaves", std::stoi(fsDescriptor["octaves"])); //we don't really need it
		int i;
		i = 0;
		i = 2;
	}
	else
	{
		// Close the file in READ mode
		fsDescriptor.release();

		// Add descriptor's parameters
		//ptrDescriptor->set("thres", 30);
		//ptrDescriptor->set("octaves", 3);
		//ptrDetector->set("patternScale", 1);

		// Open the file in WRITE mode
		cv::FileStorage fsDescriptor("params/descriptor_" + descriptorName + "_params.yaml", cv::FileStorage::WRITE);
		// Write the parameters
		ptrDescriptor->write(fsDescriptor);
		// fs in WRITE mode automatically released
	}
	// fs in READ mode automatically released

	// Descriptors for the First & Second Image ...
	cv::Mat firstImgDescriptor, secondImgDescriptor;

	// Computing the descriptors
	ptrDescriptor->compute(firstImg, firstImgKeypoints, firstImgDescriptor);
	ptrDescriptor->compute(secondImg, secondImgKeypoints, secondImgDescriptor);

	// Find the matching points
	cv::BFMatcher matcher(cv::NORM_HAMMING);
	std::vector< cv::DMatch > firstMatches, secondMatches;

	//QMessageBox msg; msg.setText("Here"); msg.exec();

	matcher.match(firstImgDescriptor, secondImgDescriptor, firstMatches);
	matcher.match(secondImgDescriptor, firstImgDescriptor, secondMatches);

	int bestMatchesCount = 0;
	std::vector< cv::DMatch > bestMatches;

	QStandardItemModel *model = new QStandardItemModel(2, 5, this); //2 Rows and 3 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Coordinate X1")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Coordinate Y1")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Coordinate X2")));
	model->setHorizontalHeaderItem(3, new QStandardItem(QString("Coordinate Y2")));
	model->setHorizontalHeaderItem(4, new QStandardItem(QString("ERR")));

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
	QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/output.png"));

}

void MainWindow::on_actionDestroy_All_Windows_triggered()
{
    cv::destroyAllWindows();
}

void MainWindow::on_actionRun_triggered()
{
	on_pushButton_pressed();
}

void MainWindow::on_actionClear_Log_triggered()
{
    ui->logPlainText->clear();
}

void MainWindow::on_actionSave_Log_File_As_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Log File"), "palmprint_registration_log_file",
		tr("Txt File (*.txt);;All Files (*)"));
	if (fileName.isEmpty())
		return;
	else {
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(this, tr("Unable to open file"),
				file.errorString());
			return;
		}
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_4_5);
		// We must fix it !
		out << ui->logPlainText->toPlainText();// .toStdString().c_str();
	}
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

bool MainWindow::noKeyPoints(std::string rank, std::vector<cv::KeyPoint> imgKeypoints)
{
	ui->logPlainText->appendPlainText("Found " + QString::number(imgKeypoints.size()) + " key points in the " + QString::fromStdString(rank) + " image!");

	if (imgKeypoints.size() <= 0)
	{
		ui->logPlainText->appendPlainText("Point matching can not be done because no key points detected in the " + QString::fromStdString(rank) + " image!");
		return true;
	}
	return false;
}
