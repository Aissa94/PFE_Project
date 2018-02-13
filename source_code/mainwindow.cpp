#include "mainwindow.h"
#include "ui_mainwindow.h"

// Images Declaration
	cv::Mat firstImg, secondImg;
	cv::Mat firstImgDescriptorShow, secondImgDescriptorShow;

// Vectors Declaration
	// Keypoints Vectors for the First & Second Image ...
	std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;
	// Descriptors for the First & Second Image ...
	cv::Mat firstImgDescriptor, secondImgDescriptor;
	// Matches for the First & Second Image ...
	std::vector<cv::DMatch> firstMatches, secondMatches, bestMatches;
	cv::Mat bestImgMatches;

// Operators Declaration
	cv::FeatureDetector * ptrDetector;
	cv::DescriptorExtractor * ptrDescriptor;
	cv::DescriptorMatcher * ptrMatcher;

// Times
	double detectionTime, descriptionTime, firstMatchingTime, secondMatchingTime, bestMatchingTime;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	// cusomizing ToolTips :
	qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white;}");
	//qApp->setStyleSheet("QToolTip { visibility: visible; width: 120px; background-color: #555; color: #fff; text-align: center; border-radius: 6px; padding: 5px 0; position: absolute; z-index: 1; bottom: 125 %; left: 50% ;margin-left: -60px; opacity: 1; transition: opacity 0.3s;}");
    //outputImagesPath = "/Users/elma/Desktop/FeaturePointsComparisonOutputImages";
    //outputImagesPath = QInputDialog::getText(this, "Output Images Path");
    ui->descriptorFreakSelectedPairsText->setPlaceholderText("Ex: 1 2 11 22 154 256...");
    ui->segmentationMethod1Param1Label->setToolTip("test");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runSIFT()
{
    //QMessageBox msg; msg.setText("SIFT"); msg.exec();

    //    SIFT( int nfeatures=0, int nOctaveLayers=3,
    //              double contrastThreshold=0.04, double edgeThreshold=10,
    //              double sigma=1.6);

    //ui->logPlainText->appendPlainText("Starting SIFT object detection!");
	ui->logPlainText->appendHtml("<b>Starting SIFT object detection!</b>");


    // Read Parameters ...
    int nfeatures = ui->siftNumFeatText->text().toInt();
    int nOctaveLayers = ui->siftNumOctText->text().toInt();
    double contrastThreshold = ui->siftContThreshText->text().toDouble();
    double edgeThreshold = ui->siftEdgeThreshText->text().toDouble();
    double sigma = ui->siftSigmaText->text().toDouble();

    // Create SIFT Objects ...
    cv::SIFT siftDetector(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

    // Detecting Keypoints ...
    siftDetector.detect(firstImg, firstImgKeypoints);
    siftDetector.detect(secondImg, secondImgKeypoints);

	if (noKeyPoints("first", firstImgKeypoints) || noKeyPoints("second", secondImgKeypoints)) return;

    // Computing the descriptors
	siftDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
    siftDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

    // Find the matching points
    if(ui->siftBruteForceCheck->isChecked())
    {
        ptrMatcher = new cv::BFMatcher(cv::NORM_L1);
    }
    else
    {
        ptrMatcher = new cv::FlannBasedMatcher();
    }
    ptrMatcher->match( firstImgDescriptor, secondImgDescriptor, firstMatches );
    ptrMatcher->match( secondImgDescriptor, firstImgDescriptor, secondMatches );

	// Drowing best matches
	calculateBestMatches();
}

void MainWindow::runSURF()
{
    //QMessageBox msg; msg.setText("SURF"); msg.exec();

    //    SURF(double hessianThreshold=100,
    //                      int nOctaves=4, int nOctaveLayers=2,
    //                      bool extended=true, bool upright=false);

	ui->logPlainText->appendHtml("<b>Starting SURF object detection!</b>");

    // Read Parameters ...
    double hessainThreshold = ui->surfHessianThreshText->text().toDouble();
    int nOctaves = ui->surfNumOctavesText->text().toInt();
    int nOctaveLayers = ui->surfNumOctLayersText->text().toInt();
	bool extended = ui->surfExtendedText->isChecked();
	bool upright = !ui->surfUprightText->isChecked();

    // Create SURF Objects ...
    cv::SURF surfDetector(hessainThreshold, nOctaves, nOctaveLayers, extended, upright);

    // Detecting Keypoints ...
    surfDetector.detect(firstImg, firstImgKeypoints);
	surfDetector.detect(secondImg, secondImgKeypoints);

    if (noKeyPoints("first", firstImgKeypoints) || noKeyPoints("second", secondImgKeypoints)) return;

    // Computing the descriptors
    surfDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
    surfDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

    // Find the matching points
    if(ui->siftBruteForceCheck->isChecked())
    {
        ptrMatcher = new cv::BFMatcher(cv::NORM_L1);
    }
    else
    {
        ptrMatcher = new cv::FlannBasedMatcher();
    }

    ptrMatcher->match( firstImgDescriptor, secondImgDescriptor, firstMatches );
    ptrMatcher->match( secondImgDescriptor, firstImgDescriptor, secondMatches );

	// !!!!!!!!!!!!!!!!!!!!!!!!!hena yebda le pblm de diffirence entre hada w ta3 custom !!! !!! !!! !!
	calculateBestMatches();
}

void MainWindow::runORB()
{
    //QMessageBox msg; msg.setText("ORB"); msg.exec();

    //    ORB(int nfeatures = 500, float scaleFactor = 1.2f, int nlevels = 8, int edgeThreshold = 31,
    //                     int firstLevel = 0, int WTA_K=2, int scoreType=HARRIS_SCORE, int patchSize=31 );

    //ui->logPlainText->appendPlainText("Starting ORB object detection!");
	ui->logPlainText->appendHtml("<b>Starting ORB object detection!</b>");

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

    // Detecting Keypoints ...
    orbDetector.detect(firstImg, firstImgKeypoints);
	orbDetector.detect(secondImg, secondImgKeypoints);

	if (noKeyPoints("first", firstImgKeypoints) || noKeyPoints("second", secondImgKeypoints)) return;

    // Computing the descriptors
    orbDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
    orbDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

    // Find the matching points
    ptrMatcher = new cv::BFMatcher(cv::NORM_HAMMING);
	ptrMatcher->match( firstImgDescriptor, secondImgDescriptor, firstMatches );
    ptrMatcher->match( secondImgDescriptor, firstImgDescriptor, secondMatches );

	calculateBestMatches();
}

void MainWindow::runBRISK()
{
	//QMessageBox msg; msg.setText("BRISK"); msg.exec();

	//    BRISK(int thresh = 30, int octaves = 3,float patternScale = 1.0f);

	//ui->logPlainText->appendPlainText("Starting BRISK object detection!");
	ui->logPlainText->appendHtml("<b>Starting BRISK object detection!</b>");

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
	// Detecting Keypoints ...
	briskDetector.detect(firstImg, firstImgKeypoints);
	briskDetector.detect(secondImg, secondImgKeypoints);

	if (noKeyPoints("first", firstImgKeypoints) || noKeyPoints("second", secondImgKeypoints)) return;

	// Computing the descriptors
	briskDetector.compute(firstImg, firstImgKeypoints, firstImgDescriptor);
	briskDetector.compute(secondImg, secondImgKeypoints, secondImgDescriptor);

	// Find the matching points
	ptrMatcher = new cv::BFMatcher(cv::NORM_HAMMING);
	ptrMatcher->match(firstImgDescriptor, secondImgDescriptor, firstMatches);
	ptrMatcher->match(secondImgDescriptor, firstImgDescriptor, secondMatches);

	calculateBestMatches();
}

void MainWindow::runCustom()
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
			ui->detectorFastNonmaxSuppressionCheck->isChecked());
	/*case 2:
		// FASTX
		cv::FASTX(ui->detectorFastThresholdText->text().toInt(),
			ui->detectorFastNonmaxSuppressionCheck->isChecked(),
			ui->detectorFastTypeText->currentIndex());*/
		break;
	case 2:
		// SIFT
		ptrDetector = new cv::SiftFeatureDetector(ui->detectorSiftNfeaturesText->text().toInt(),
			ui->detectorSiftNOctaveLayersText->text().toInt(),
			ui->detectorSiftContrastThresholdText->text().toDouble(),
			ui->detectorSiftEdgeThresholdText->text().toDouble(),
			ui->detectorSiftSigmaText->text().toDouble());
		break;
	case 3:
		//SURF
		// we didn't need the Extended and Upright params because it is related to the SURF descriptor
		ptrDetector = new cv::SurfFeatureDetector(ui->detectorSurfHessianThresholdText->text().toDouble(),
			ui->detectorSurfNOctavesText->text().toInt(),
			ui->detectorSurfNLayersText->text().toInt(),
			true,
			false);
		break;
	case 4:
		//Dense
		ptrDetector = new cv::DenseFeatureDetector(ui->detectorDenseInitFeatureScaleText->text().toFloat(),
			ui->detectorDenseFeatureScaleLevelsText->text().toInt(),
			ui->detectorDenseFeatureScaleMulText->text().toFloat(),
			ui->detectorDenseInitXyStepText->text().toInt(),
			ui->detectorDenseInitImgBoundText->text().toInt(),
			ui->detectorDenseInitXyStepText->text().toInt()>0,
			ui->detectorDenseInitImgBoundText->text().toInt()>0);
		break;
	//....
	default:
		return;
		break;
	}

	// Write the parameters
	writeToFile("detector_" + detectorName, ptrDetector);
	// fs in WRITE mode automatically released
	try{
		// Detecting Keypoints ...
		detectionTime = (double)cv::getTickCount();
		ptrDetector->detect(firstImg, firstImgKeypoints);
		ptrDetector->detect(secondImg, secondImgKeypoints);
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
	}catch (...){
		ui->logPlainText->appendHtml("<b style='color:red'>Please select the right "+QString::fromStdString(detectorName)+" detector parameters, or use the defaults!.</b>");
		return;
	}
	if (noKeyPoints("first", firstImgKeypoints) || noKeyPoints("second", secondImgKeypoints)) return;
	ui->logPlainText->appendPlainText("detection time: " + QString::number(detectionTime) + " (s)");

	// Customising Descriptor...
	switch (descriptorIndex)
	{
	case 0:
	{
		// trait the descriptorFreakSelectedPairsIndexes
		std::string descriptorFreakSelectedPairsText = ui->descriptorFreakSelectedPairsText->text().toStdString();
		std::stringstream stringStream(descriptorFreakSelectedPairsText);
		// get ints from a text
		int number;
		std::vector<int> descriptorFreakSelectedPairsIndexes;
		while (stringStream >> number){
			descriptorFreakSelectedPairsIndexes.push_back(number);
		}
		// FREAK	
		ptrDescriptor = new cv::FREAK(ui->descriptorFreakOrientationNormalizedCheck->isChecked(),
			ui->descriptorFreakScaleNormalizedCheck->isChecked(),
			ui->descriptorFreakPatternScaleText->text().toFloat(),
			ui->descriptorFreakNOctavesText->text().toFloat(),
			descriptorFreakSelectedPairsIndexes);
		/* Select Pairs
		std::vector<std::vector<cv::KeyPoint>> test = { firstImgKeypoints, secondImgKeypoints };
		descriptorFreakSelectedPairsIndexes = ((cv::FREAK)ptrDescriptor).selectPairs({ firstImg, secondImg }, test, 0.699999999999, true);*/
	}
		break;
	case 1:
		// BRIEF
		ptrDescriptor = new cv::BriefDescriptorExtractor(ui->descriptorBriefLengthText->text().toInt());
		break;
	case 2:
		// SIFT
		ptrDescriptor = new cv::SiftDescriptorExtractor();
		break;
	case 3:
		//SURF
		// we just need the Extended and Upright params because others are related to the SURF detector
		ptrDescriptor = new cv::SurfDescriptorExtractor(100, 4, 3, ui->descriptorSurfExtended->isChecked(), !ui->detectorSurfUprightText->isChecked());
		break;
	//....
	default:
		break;
	}

	// Write the parameters
	writeToFile("descriptor_" + descriptorName, ptrDescriptor);

    if (ui->opponentColor->isChecked())
		//OpponentColor
		ptrDescriptor = new cv::OpponentColorDescriptorExtractor(ptrDescriptor);
	// Write the parameters
	writeToFile("descriptorOppCol_" + descriptorName, ptrDescriptor);

	try{
		// Aissa !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! affichinna firstImgKeypoints avant et après pour voir c'est compute va les changer ou pas!!!!
		descriptionTime = (double)cv::getTickCount();
		ptrDescriptor->compute(firstImg, firstImgKeypoints, firstImgDescriptor);
		ptrDescriptor->compute(secondImg, secondImgKeypoints, secondImgDescriptor);
		descriptionTime = ((double)cv::getTickCount() - descriptionTime) / cv::getTickFrequency();
		ui->logPlainText->appendPlainText("description time: " + QString::number(descriptionTime) + " (s)");
	}
	catch (...){
		if(descriptorName == "FREAK")ui->logPlainText->appendHtml("<b style='color:red'>Please select the right pair indexes within the FREAK descriptor, or just leave it!.</b><br>(For more details read Section(4.2) in: <i>A. Alahi, R. Ortiz, and P. Vandergheynst. FREAK: Fast Retina Keypoint. In IEEE Conference on Computer Vision and Pattern Recognition, 2012.</i>)");
		else ui->logPlainText->appendHtml("<b style='color:red'>Please select the right " + QString::fromStdString(descriptorName) + " descriptor parameters, or use the defaults!.</b>");
		return;
	}

	// Customising Matcher...
	int kBestMatches = 1;
	// std::vector<std::vector< cv::DMatch >> firstSetMatches(), secondSetMatches();
	switch (matcherIndex)
	{
	case 0:
	{
		// BruteForce
		kBestMatches = ui->matcherBruteForceKBestText->text().toInt();
		int norm = getNormByText(ui->matcherBruteForceNormTypeText->currentText().toStdString());
		ptrMatcher = new cv::BFMatcher(norm, ui->matcherBruteForceCrossCheckText->isChecked());
	}
		break;
	case 1:
	{
		// FlannBased
		// using default values
		ptrMatcher = new cv::FlannBasedMatcher(getFlannBasedIndexParamsType(), new cv::flann::SearchParams(32));
	}
		break;
	// ...
	default:
		break;
	}

	// Write the parameters
	writeToFile("matcher_" + matcherName, ptrMatcher);

	// Find the matching points
	try{
		/*if (kBestMatches > 1){
		// first and second set of the k best matches
		firstMatchingTime = (double)cv::getTickCount();
		ptrMatcher->knnMatch(firstImgDescriptor, secondImgDescriptor, firstSetMatches, kBestMatches, cv::Mat(), false);
		firstMatchingTime = ((double)cv::getTickCount() - firstMatchingTime) / cv::getTickFrequency();

		secondMatchingTime = (double)cv::getTickCount();
		ptrMatcher->knnMatch(secondImgDescriptor, firstImgDescriptor, secondSetMatches, kBestMatches, cv::Mat(), false);
		secondMatchingTime = ((double)cv::getTickCount() - secondMatchingTime) / cv::getTickFrequency();
		}
		else*/ {
			// first and reverse set of the best matches (simple match)
			firstMatchingTime = (double)cv::getTickCount();
			ptrMatcher->match(firstImgDescriptor, secondImgDescriptor, firstMatches);
			firstMatchingTime = ((double)cv::getTickCount() - firstMatchingTime) / cv::getTickFrequency();

			secondMatchingTime = (double)cv::getTickCount();
			ptrMatcher->match(secondImgDescriptor, firstImgDescriptor, secondMatches);
			secondMatchingTime = ((double)cv::getTickCount() - secondMatchingTime) / cv::getTickFrequency();
		}
	}
	catch (...){
		// For example Flann-Based doesn't work with Brief desctiptor extractor
		// And also, some descriptors must be used with specific NORM_s
		ui->logPlainText->appendHtml("<b style='color:red'>Cannot match descriptors because of an incompatible combination!, try another one.</b>");
		return;
	}
	
	bestMatchingTime = std::min(firstMatchingTime, secondMatchingTime);
	ui->logPlainText->appendPlainText("matching time: " + QString::number(bestMatchingTime) + " (s)");
	ui->logPlainText->appendPlainText("Total time: " + QString::number(detectionTime + descriptionTime + bestMatchingTime) + " (s)");

	calculateBestMatches();
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
	if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex()==4)){
		// Custom && OpponentColor
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
	}
	else{
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
	}

	// Check if the Images are loaded correctly ...
	if (firstImg.empty() || secondImg.empty())
	{
		ui->logPlainText->appendHtml("<b style='color:red'>Error while trying to read one of the input files!</b>");
		return;
	}
	if ((firstImg.cols < 100) && (firstImg.rows < 100))
	{
		cv::resize(firstImg, firstImg, cv::Size(), 200 / firstImg.rows, 200 / firstImg.cols);
	}

	if ((secondImg.cols < 100) && (secondImg.rows < 100))
	{
		cv::resize(secondImg, secondImg, cv::Size(), 200 / secondImg.rows, 200 / secondImg.cols);
	}

	// Rest parameters :
	resetParams();

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

	default:
		// custom
		runCustom();
		break;
	}
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

int MainWindow::getNormByText(std::string norm){
	if (norm == "NORM_INF") return cv::NORM_INF;
	else if (norm == "NORM_L1") return cv::NORM_L1;
	else if (norm == "NORM_L2") return cv::NORM_L2;
	else if (norm == "NORM_L2SQR") return cv::NORM_L2SQR;
	else if (norm == "NORM_HAMMING") return cv::NORM_HAMMING;
	else if (norm == "NORM_HAMMING2") return cv::NORM_HAMMING2;
	else if (norm == "NORM_TYPE_MASK") return cv::NORM_TYPE_MASK;
	else if (norm == "NORM_RELATIVE") return cv::NORM_RELATIVE;
	else if (norm == "NORM_MINMAX") return cv::NORM_MINMAX;
	else return cv::NORM_L2;
}

cv::Ptr<cv::flann::IndexParams> MainWindow::getFlannBasedIndexParamsType(){
	// Return the default constructur, for the select type
	if (ui->matcherFlannBasedLinearIndexParams->isChecked()) return new cv::flann::LinearIndexParams();
	else if (ui->matcherFlannBasedKDTreeIndexParams->isChecked()) return new cv::flann::KDTreeIndexParams();
	else if (ui->matcherFlannBasedKMeansIndexParams->isChecked()) return new cv::flann::KMeansIndexParams();
	else if (ui->matcherFlannBasedCompositeIndexParams->isChecked()) return new cv::flann::CompositeIndexParams();
	else if (ui->matcherFlannBasedLshIndexParams->isChecked()) return new cv::flann::LshIndexParams(20,15,2);
	else if (ui->matcherFlannBasedAutotunedIndexParams->isChecked()) return new cv::flann::AutotunedIndexParams();
	else return new cv::flann::KDTreeIndexParams();
}

void MainWindow::writeToFile(std::string fileName, cv::Algorithm * algoToWrite){
// Open a file and write parameters of an algorithm
	// Open the file in WRITE mode
	cv::FileStorage fs("params/" + fileName + "_params.yaml", cv::FileStorage::WRITE);
	// Write the parameters
	algoToWrite->write(fs);
	// fs in WRITE mode automatically released
}

void MainWindow::calculateBestMatches(){
    
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

	// Calculate the number of best matches between two sets of matches
	int bestMatchesCount = 0;
	QStandardItemModel *model = new QStandardItemModel(2, 5, this); //2 Rows and 3 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Coordinate X1")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Coordinate Y1")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Coordinate X2")));
	model->setHorizontalHeaderItem(3, new QStandardItem(QString("Coordinate Y2")));
	model->setHorizontalHeaderItem(4, new QStandardItem(QString("Distance")));
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

void MainWindow::resetParams()
{
	try{
		firstImgKeypoints.clear(); secondImgKeypoints.clear();
		firstImgDescriptor.release(); secondImgDescriptor.release();
		firstMatches.clear(); secondMatches.clear(); bestMatches.clear();
		bestImgMatches.release();
		delete ptrDetector;
		delete ptrDescriptor;
		delete ptrMatcher;
	}
	catch (...){
		ui->logPlainText->appendHtml("<b style='color:yellow'>Enable to free some structures!</b>");
	}
}
