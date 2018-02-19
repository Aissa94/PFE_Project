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
	// Matches for the Direct & Invers matching ...
	std::vector<cv::DMatch> directMatches, inverseMatches, bestMatches;
	// Maches for knn > 1
	std::vector<std::vector< cv::DMatch >> knnMatches;
	cv::Mat bestImgMatches;

// Operators Declaration
	cv::FeatureDetector * ptrDetector;
	cv::DescriptorExtractor * ptrDescriptor;
	cv::DescriptorMatcher * ptrMatcher;

// Times
	double detectionTime, descriptionTime, directMatchingTime, inverseMatchingTime, bestMatchingTime, conversionTime;
// Others
	int kBestMatches;
	int cpt;

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
    ptrMatcher->match( firstImgDescriptor, secondImgDescriptor, directMatches );
    ptrMatcher->match( secondImgDescriptor, firstImgDescriptor, inverseMatches );

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

    ptrMatcher->match( firstImgDescriptor, secondImgDescriptor, directMatches );
    ptrMatcher->match( secondImgDescriptor, firstImgDescriptor, inverseMatches );

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
	ptrMatcher->match( firstImgDescriptor, secondImgDescriptor, directMatches );
    ptrMatcher->match( secondImgDescriptor, firstImgDescriptor, inverseMatches );

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
	ptrMatcher->match(firstImgDescriptor, secondImgDescriptor, directMatches);
	ptrMatcher->match(secondImgDescriptor, firstImgDescriptor, inverseMatches);

	calculateBestMatches();
}

void MainWindow::runCustom()
{
	// Get choices
	int segmentationIndex = ui->segmentationTabs->currentIndex();
	int detectorIndex = ui->detectorTabs->currentIndex();
	int descriptorIndex = ui->descriptorTabs->currentIndex();
	int matcherIndex = ui->matcherTabs->currentIndex();
	std::string segmentationName = ui->segmentationTabs->tabText(ui->segmentationTabs->currentIndex()).toStdString();
	std::string detectorName = ui->detectorTabs->tabText(ui->detectorTabs->currentIndex()).toStdString();
	std::string descriptorName = ui->descriptorTabs->tabText(ui->descriptorTabs->currentIndex()).toStdString();
	std::string matcherName = ui->matcherTabs->tabText(ui->matcherTabs->currentIndex()).toStdString();

	ui->logPlainText->appendHtml(QString::fromStdString("<b>Starting (" + segmentationName +", "+ detectorName + ", " + descriptorName + ", " + matcherName + ") object detection!</b>"));
	if (segmentationIndex != 0){
		// First load the image to process in grayscale and transform it to a binary image using thresholding:
		// Binarization
		// The Otsu thresholding will automatically choose the best generic threshold for the image to obtain a good contrast between foreground and background information.
		// If you have only a single capturing device, then playing around with a fixed threshold value could result in a better image for that specific setup
		cv::threshold(firstImg, firstImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); //127, 255, cv::THRESH_BINARY);
		cv::threshold(secondImg, secondImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); //127, 255, cv::THRESH_BINARY);
		cv::imwrite(QString(qApp->applicationDirPath() + "/1-binarization.png").toStdString(), firstImg);
	}
	// Customising Segmentor...	
	switch (segmentationIndex)
	{
	case 1:
		// Thinning
		thinning(firstImg);
		thinning(secondImg);
		cv::imwrite(QString(qApp->applicationDirPath() + "/2-thinning-1.png").toStdString(), firstImg);
		cpt = 1;
		break;
	case 2:
		// Skeletonization
		// This will create more unique and stronger interest points
		firstImg = skeletonization(firstImg);
		secondImg = skeletonization(secondImg);
		cv::imwrite(QString(qApp->applicationDirPath() + "/2-skeltonization-2.png").toStdString(), firstImg);
		cpt = 2;
		break;
		//....
	}

	// Customising Detector...	
	switch (detectorIndex)
	{
	case 0:{
		// Minutiae-detection
		std::vector<Minutiae> firstMinutiae, secondMinutiae;
		
		detectionTime = (double)cv::getTickCount();
		crossingNumber::getMinutiae(firstImg, firstMinutiae, ui->detectorMinutiaeBorderText->text().toInt());
		crossingNumber::getMinutiae(secondImg, secondMinutiae, ui->detectorMinutiaeBorderText->text().toInt());
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
		
		//visualizingMinutiae(firstImg, firstMinutiae);

		//Minutiae-filtering
		// slow with the second segmentation
		Filter::filterMinutiae(firstMinutiae);
		Filter::filterMinutiae(secondMinutiae);
		
		//visualizingMinutiae(secondImg, secondMinutiae, "4-filtering");

		// images must be segmented if not Minutiae will be ampty
		try{
			// Transforming Minutiae to KeyPoints
			firstImgKeypoints = getKeypointsFromMinutiae(firstMinutiae);
			secondImgKeypoints = getKeypointsFromMinutiae(secondMinutiae);
			// Add conversion time from minutiae to keypoints
			detectionTime += conversionTime;
		}
		catch (...){
			ui->logPlainText->appendHtml("<i style='color:red'>Before detecting Minutiaen you must select a segmentation method !</i>");
			return;
		}

	}
		   break;
	case 1:{
		// Harris-Corners
		detectionTime = (double)cv::getTickCount();
		harrisCorners(firstImg, firstImgKeypoints, ui->detectorHarrisThresholdText->text().toFloat());
		harrisCorners(secondImg, secondImgKeypoints, ui->detectorHarrisThresholdText->text().toFloat());
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
	}
		   break;
	case 2:{
		// STAR
		ptrDetector = new cv::StarFeatureDetector(ui->detectorStarMaxSizeText->text().toInt(),
			ui->detectorStarResponseThresholdText->text().toInt(),
			ui->detectorStarThresholdProjectedText->text().toInt(),
			ui->detectorStarThresholdBinarizedText->text().toInt(),
			ui->detectorStarSuppressNonmaxSizeText->text().toInt());
	}
		break;
	case 3:
		// FAST
		ptrDetector = new cv::FastFeatureDetector(ui->detectorFastThresholdText->text().toInt(),
			ui->detectorFastNonmaxSuppressionCheck->isChecked());
		break;
	case 4:
		// SIFT
		ptrDetector = new cv::SiftFeatureDetector(ui->detectorSiftNfeaturesText->text().toInt(),
			ui->detectorSiftNOctaveLayersText->text().toInt(),
			ui->detectorSiftContrastThresholdText->text().toDouble(),
			ui->detectorSiftEdgeThresholdText->text().toDouble(),
			ui->detectorSiftSigmaText->text().toDouble());
		break;
	case 5:
		//SURF
		// we didn't need the Extended and Upright params because it is related to the SURF descriptor
		ptrDetector = new cv::SurfFeatureDetector(ui->detectorSurfHessianThresholdText->text().toDouble(),
			ui->detectorSurfNOctavesText->text().toInt(),
			ui->detectorSurfNLayersText->text().toInt(),
			true,
			false);
		break;
	case 6:
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
		ui->logPlainText->appendHtml("<i style='color:yellow'>No detector selected.</i>");
		return;
		break;
	}

	if (detectorIndex > 1){
		// Write the parameters
		writeToFile("detector_" + detectorName, ptrDetector);
		// fs in WRITE mode automatically released
		try{
			// Detecting Keypoints ...
			if (detectorIndex == 1 && ui->detectorFastXCheck->isChecked()){
				// FASTX
				detectionTime = (double)cv::getTickCount();
				cv::FASTX(firstImg, firstImgKeypoints, ui->detectorFastThresholdText->text().toInt(),
					ui->detectorFastNonmaxSuppressionCheck->isChecked(),
					ui->detectorFastTypeText->currentIndex());
				cv::FASTX(secondImg, secondImgKeypoints, ui->detectorFastThresholdText->text().toInt(),
					ui->detectorFastNonmaxSuppressionCheck->isChecked(),
					ui->detectorFastTypeText->currentIndex());
				detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
			}
			else {
				// Others
				detectionTime = (double)cv::getTickCount();
				ptrDetector->detect(firstImg, firstImgKeypoints);
				ptrDetector->detect(secondImg, secondImgKeypoints);
				detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
			}
		}
		catch (...){
			ui->logPlainText->appendHtml("<b style='color:red'>Please select the right " + QString::fromStdString(detectorName) + " detector parameters, or use the defaults!.</b>");
			return;
		}
	}
	if (noKeyPoints("first", firstImgKeypoints) || noKeyPoints("second", secondImgKeypoints)) return;
	ui->logPlainText->appendPlainText("detection time: " + QString::number(detectionTime) + " (s)");

	if (true){ //Klustering
		/*std::vector<cv::Mat> matrix = { firstImgKeypoints, secondImgKeypoints };
		clusteringIntoKClusters(matrix, 1000);*/
	}

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
		ui->logPlainText->appendHtml("<i style='color:yellow'>No descriptor selected.</i>");
		return;
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
	kBestMatches = (ui->matcherKBestText->text().toInt() < 1) ? 1 : ui->matcherKBestText->text().toInt();
	switch (matcherIndex)
	{
	case 0:
	{
		// BruteForce
		int norm = getNormByText(ui->matcherBruteForceNormTypeText->currentText().toStdString());
		ptrMatcher = new cv::BFMatcher(norm, ui->matcherBruteForceCrossCheckText->isChecked());
	}
		break;
	case 1:
	{
		// FlannBased
		// using default values
		ptrMatcher = new cv::FlannBasedMatcher(getFlannBasedIndexParamsType(),
			new cv::flann::SearchParams(ui->matcherFlannBasedSearchParamsText->text().toInt()));
	}
		break;
	// ...
	default:
		ui->logPlainText->appendHtml("<i style='color:yellow'>No matcher selected.</i>");
		return;
		break;
	}

	// Write the parameters
	writeToFile("matcher_" + matcherName, ptrMatcher);

	// Find the matching points
	try{
		if (kBestMatches < 2 || ui->matcherInlierInversMatches->isChecked()){
			// direct and reverse set of the best matches (simple match)
			directMatchingTime = (double)cv::getTickCount();
			ptrMatcher->match(firstImgDescriptor, secondImgDescriptor, directMatches);
			directMatchingTime = ((double)cv::getTickCount() - directMatchingTime) / cv::getTickFrequency();

			inverseMatchingTime = (double)cv::getTickCount();
			ptrMatcher->match(secondImgDescriptor, firstImgDescriptor, inverseMatches);
			inverseMatchingTime = ((double)cv::getTickCount() - inverseMatchingTime) / cv::getTickFrequency();

			bestMatchingTime = std::min(directMatchingTime, inverseMatchingTime);
		}
		else { // ui->matcherInlierLoweRatio->isChecked()
			// only direct set of the k best matches
			directMatchingTime = (double)cv::getTickCount();
			ptrMatcher->knnMatch(firstImgDescriptor, secondImgDescriptor, knnMatches, kBestMatches, cv::Mat(), false);
			directMatchingTime = ((double)cv::getTickCount() - directMatchingTime) / cv::getTickFrequency();

			bestMatchingTime = directMatchingTime;
		}
	}
	catch (...){
		// For example Flann-Based doesn't work with Brief desctiptor extractor
		// And also, some descriptors must be used with specific NORM_s
		ui->logPlainText->appendHtml("<b style='color:red'>Cannot match descriptors because of an incompatible combination!, try another one.</b>");
		return;
	}
	
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
	if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 4)){
		// Custom && OpponentColor
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
	}
	else{
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), cv::IMREAD_GRAYSCALE);//or CV_LOAD_IMAGE_GRAYSCALE
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), cv::IMREAD_GRAYSCALE); //or CV_LOAD_IMAGE_GRAYSCALE
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
	ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

	cv::drawKeypoints(secondImg, secondImgKeypoints, secondImgDescriptorShow, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
	QGraphicsScene *imgKeypoints2 = new QGraphicsScene();
	QImage imgKey2((const uchar *)secondImgDescriptorShow.data, secondImgDescriptorShow.cols, secondImgDescriptorShow.rows, secondImgDescriptorShow.step, QImage::Format_RGB888);
	QPixmap piximg2 = QPixmap::fromImage(imgKey2);
	imgKeypoints2->addPixmap(piximg2);
	ui->graphicsView_3->setScene(imgKeypoints2);
	ui->graphicsView_3->fitInView(imgKeypoints2->sceneRect(), Qt::KeepAspectRatio);
	ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

	// Calculate the number of best matches between two sets of matches
	int bestMatchesCount = 0;
	float sumDistances = 0;
	QStandardItemModel *model = new QStandardItemModel(2, 5, this); //2 Rows and 3 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Coordinate X1")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Coordinate Y1")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Coordinate X2")));
	model->setHorizontalHeaderItem(3, new QStandardItem(QString("Coordinate Y2")));
	model->setHorizontalHeaderItem(4, new QStandardItem(QString("Distance")));

	/*if (use_ransac == false)
		compute_inliers_homography(matches_surf, inliers_surf, H, MAX_H_ERROR);
	else
		compute_inliers_ransac(matches_surf, inliers_surf, MAX_H_ERROR, false);*/

	if (kBestMatches < 2 || ui->matcherInlierInversMatches->isChecked()) {
		// in reverse matching test
		for (uint i = 0; i < directMatches.size(); i++)
		{
			cv::Point matchedPt1 = firstImgKeypoints[i].pt;
			cv::Point matchedPt2 = secondImgKeypoints[directMatches[i].trainIdx].pt;

			bool foundInReverse = false;

			for (uint j = 0; j < inverseMatches.size(); j++)
			{
				cv::Point tmpSecImgKeyPnt = secondImgKeypoints[j].pt;
				cv::Point tmpFrstImgKeyPntTrn = firstImgKeypoints[inverseMatches[j].trainIdx].pt;
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
				QStandardItem *dist = new QStandardItem(QString::number(directMatches[i].distance));
				model->setItem(bestMatchesCount, 4, dist);
				ui->tableView->setModel(model);

				sumDistances += directMatches[i].distance;
				bestMatches.push_back(directMatches[i]);
				bestMatchesCount++;
			}
		}
		ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount) + "/" + QString::number(std::min(directMatches.size(), inverseMatches.size())));
	} else if (kBestMatches == 2 && ui->matcherInlierLoweRatio->isChecked()){
		// knn = 2
		// Lowe's ratio test = 0.7
		for each (std::vector<cv::DMatch> match in knnMatches)
		{
			if (match[0].distance < 0.7*match[1].distance){
				bestMatches.push_back(match[0]);
				bestMatchesCount++;
			}
		}
		ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount) + "/" + QString::number(knnMatches.size()));
	} else {// knn > 2
		for each (std::vector<cv::DMatch> match in knnMatches)
		{
			bestMatches.push_back(match[0]);
			bestMatchesCount++;
		}
		ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount) + "/" + QString::number(knnMatches.size()));
	}
	double minKeypoints = firstImgKeypoints.size() <= secondImgKeypoints.size() ?
		firstImgKeypoints.size()
		:
		secondImgKeypoints.size();

	ui->logPlainText->appendPlainText("Sum of distances = " + QString::number(sumDistances));
	ui->logPlainText->appendPlainText("Probability = " + QString::number((bestMatchesCount / minKeypoints) * 100) + "%");
	cv::drawMatches(firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
		bestMatches, bestImgMatches, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 0),
		std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	cv::imwrite(QString(qApp->applicationDirPath() + "/output.png").toStdString(), bestImgMatches);
	
	//-- Draw only "good" matches
	QGraphicsScene *scene = new QGraphicsScene();
	QImage dest((const uchar *)bestImgMatches.data, bestImgMatches.cols, bestImgMatches.rows, bestImgMatches.step, QImage::Format_RGB888);
	dest.bits();
	QPixmap pixmap = QPixmap::fromImage(dest);
	scene->addPixmap(pixmap);
	ui->graphicsView->setScene(scene);
	ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
}

void MainWindow::wheelEvent(QWheelEvent *event){

	ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	ui->graphicsView_2->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	ui->graphicsView_3->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	// Scale the view / do the zoom
	double scaleFactor = 1.15;
	if (event->delta() > 0) {
		// Zoom in
		ui->graphicsView->scale(scaleFactor, scaleFactor);
		ui->graphicsView_2->scale(scaleFactor, scaleFactor);
		ui->graphicsView_3->scale(scaleFactor, scaleFactor);
	}
	else {
		// Zooming out
		ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
		ui->graphicsView_2->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
		ui->graphicsView_3->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
	}
}

void MainWindow::resetParams()
{
	try{
		firstImgKeypoints.clear(); secondImgKeypoints.clear();
		firstImgDescriptor.release(); secondImgDescriptor.release();
		directMatches.clear(); inverseMatches.clear(); bestMatches.clear();
		knnMatches.clear();
		bestImgMatches.release();
		//delete ptrDetector;
		//delete ptrDescriptor;
		//delete ptrMatcher;
	}
	catch (...){
		ui->logPlainText->appendHtml("<b style='color:yellow'>Enable to free some structures!</b>");
	}
}

cv::Mat MainWindow::skeletonization(cv::Mat img){
	// Image to store the skeleton and also a temporary image in order to store intermediate 
	cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0)); //The skeleton image is filled with black at the beginning.
	//cv::Mat temp(img.size(), CV_8UC1);
	cv::Mat temp;
	cv::Mat eroded;
	//  structuring element we will use for our morphological operations
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3)); //here we use a 3x3 cross-shaped structure element (i.e. we use 4-connexity).

	bool done;
	do
	{
		cv::erode(img, eroded, element);
		cv::dilate(eroded, temp, element); // temp = open(img)
		cv::subtract(img, temp, temp);
		cv::bitwise_or(skel, temp, skel);
		eroded.copyTo(img);

		done = (cv::countNonZero(img) == 0);
	} while (!done);

	return skel;
}

// This code is part of the code supplied with the OpenCV Blueprints book. It was written by Steven Puttemans
// https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/chapter_6/source_code/fingerprint/fingerprint_process/fingerprint_process.cpp
// -------------->
void MainWindow::thinningIteration(cv::Mat& im, int iter){
	// Perform a single thinning iteration, which is repeated until the skeletization is finalized
	cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);
	for (int i = 1; i < im.rows - 1; i++)
	{
		for (int j = 1; j < im.cols - 1; j++)
		{
			uchar p2 = im.at<uchar>(i - 1, j);
			uchar p3 = im.at<uchar>(i - 1, j + 1);
			uchar p4 = im.at<uchar>(i, j + 1);
			uchar p5 = im.at<uchar>(i + 1, j + 1);
			uchar p6 = im.at<uchar>(i + 1, j);
			uchar p7 = im.at<uchar>(i + 1, j - 1);
			uchar p8 = im.at<uchar>(i, j - 1);
			uchar p9 = im.at<uchar>(i - 1, j - 1);

			int A = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
				(p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
				(p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
				(p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
			int B = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
			int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
			int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)

				marker.at<uchar>(i, j) = 1;
		}
	}
	im &= ~marker;
}

void MainWindow::thinning(cv::Mat& im){
	// Function for thinning any given binary image within the range of 0-255. If not you should first make sure that your image has this range preset and configured!
	// Enforce the range tob e in between 0 - 255
	im /= 255;

	cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
	cv::Mat diff;

	do {
		thinningIteration(im, 0);
		thinningIteration(im, 1);
		absdiff(im, prev, diff);
		im.copyTo(prev);
	} while (countNonZero(diff) > 0);

	im *= 255;
}

void MainWindow::harrisCorners(cv::Mat thinnedImage, std::vector<cv::KeyPoint> &keypoints, float threshold=125.0){
// detect the strong minutiae using Haris corner detection (retrieved from 'OpenCV 3 Blueprints' book)
	cv::Mat harris_corners, harris_normalised;
	harris_corners = cv::Mat::zeros(thinnedImage.size(), CV_32FC1);
	cornerHarris(thinnedImage, harris_corners, 2, 3, 0.04, cv::BORDER_DEFAULT);
	// get a map with all the available corner responses rescaled to the range of[0 255] and stored as float values
	normalize(harris_corners, harris_normalised, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

	// Select the strongest corners that we want
	// threshold = 125.0;
	cv::Mat rescaled;
	convertScaleAbs(harris_normalised, rescaled);
	cv::Mat harris_c(rescaled.rows, rescaled.cols, CV_8UC3);
	cv::Mat in[] = { rescaled, rescaled, rescaled };
	int from_to[] = { 0, 0, 1, 1, 2, 2 };
	mixChannels(in, 3, &harris_c, 1, from_to, 3);
	for (int x = 0; x<harris_normalised.cols; x++){
		for (int y = 0; y<harris_normalised.rows; y++){
			if ((int)harris_normalised.at<float>(y, x) > threshold){
				// Draw or store the keypoint location here, just like you decide. In our case we will store the location of the keypoint
				circle(harris_c, cv::Point(x, y), 5, cv::Scalar(0, 255, 0));
				circle(harris_c, cv::Point(x, y), 1, cv::Scalar(0, 0, 255));
				keypoints.push_back(cv::KeyPoint(x, y, 1));
			}
		}
	}
	//imshow("temp", harris_c); cv::waitKey(0);
}

void MainWindow::clusteringIntoKClusters(std::vector<cv::Mat> features_vector, int k){
// K : The number of clusters to split the samples in rawFeatureData (retrieved from 'OpenCV 3 Blueprints' book)
	cv::Mat rawFeatureData = cv::Mat::zeros(firstImgKeypoints.size() + secondImgKeypoints.size(), firstImgKeypoints[0].size, CV_32FC1);
	// We need to copy the data from the vector of key points features_vector to imageFeatureData:
	int cur_idx = 0;
	for (int i = 0; i < features_vector.size(); i++){
		features_vector[i].copyTo(rawFeatureData.rowRange(cur_idx, cur_idx + features_vector[i].rows));
		cur_idx += features_vector[i].rows;
	}
	cv::Mat labels, centers;
	double result;
	result = cv::kmeans(rawFeatureData, k, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 1.0),
		3, cv::KMEANS_PP_CENTERS, centers);
}

// <--------------

void MainWindow::visualizingMinutiae(cv::Mat img, std::vector<Minutiae> minutiae, std::string stepName){
	//Visualisation
	cv::Mat minutImg = img.clone();
	cvtColor(img, minutImg, CV_GRAY2RGB);
	for (std::vector<Minutiae>::size_type i = 0; i < minutiae.size(); i++){
		//add an transparent square at each minutiae-location
		int squareSize = 5;     //has to be uneven
		cv::Mat roi = minutImg(cv::Rect(minutiae[i].getLocX() - squareSize / 2, minutiae[i].getLocY() - squareSize / 2, squareSize, squareSize));
		double alpha = 0.3;
		if (minutiae[i].getType() == Minutiae::Type::RIDGEENDING){
			cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(255, 0, 0));    //blue square for ridgeending
			addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
		}
		else if (minutiae[i].getType() == Minutiae::Type::BIFURCATION){
			cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 0, 255));    //red square for bifurcation
			addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
		}

	}
	//namedWindow("Minutiea", cv::WINDOW_AUTOSIZE);     // Create a window for display.
	//imshow("Minutiea", minutImg);                 // Show our image inside it.
	cv::imwrite(QString(qApp->applicationDirPath() + "/" + QString::fromStdString(stepName) + "-" + QString::number(cpt) + ".png").toStdString(), minutImg);
}

void MainWindow::calculateMinutiaeMagnitudeAngle(std::vector<Minutiae> minutiaes, std::vector<float> &magnitudes, std::vector<float> &angles){
// Calculates the magnitude and angle of minutiaes.
	// getting X and Y vecors of minutiaes
	std::vector<float> minutiaeXs, minutiaeYs;
	for each (Minutiae minutiae in minutiaes)
	{
		minutiaeXs.push_back(minutiae.getLocX());
		minutiaeYs.push_back(minutiae.getLocY());
	}
	// The angles are calculated with accuracy about 0.3 degrees.For the point(0, 0), the angle is set to 0.
	conversionTime = (double)cv::getTickCount();
	cv::cartToPolar(minutiaeXs, minutiaeYs, magnitudes, angles, true);
	conversionTime = ((double)cv::getTickCount() - conversionTime) / cv::getTickFrequency();
}

std::vector<cv::KeyPoint> MainWindow::getKeypointsFromMinutiae(std::vector<Minutiae> minutiaes){
	std::vector<cv::KeyPoint> keypoints;
	// Calculates the magnitude and angle of minutiaes.
	std::vector<float> magnitudes, angles;
	calculateMinutiaeMagnitudeAngle(minutiaes, magnitudes, angles);

	// Minutiae to KeyPoint
	int i = 0;
	for each (Minutiae minutiae in minutiaes)
	{
		keypoints.push_back(cv::KeyPoint(minutiae.getLocX(), minutiae.getLocY(), magnitudes[i], angles[i]));
		i++;
	}
	return keypoints;
}