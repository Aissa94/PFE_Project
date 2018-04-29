#include "mainwindow.h"
#include "ui_mainwindow.h"

// Images Declaration
	cv::Mat firstImg, secondImg;
	std::vector<std::pair<std::string, cv::Mat>> setImgs;

// Vectors Declaration
	// Segmentation parameters for First and Second Image ...
	cv::Mat firstEnhancedImage, firstSegmentedImage, secondEnhancedImage, secondSegmentedImage;
	std::vector<cv::Mat> setEnhancedImages, setSegmentedImages;
	// Keypoints Vectors for the First & Second Image ...
	std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;
	std::vector<std::vector<cv::KeyPoint>> setImgsKeypoints;
	// Descriptors for the First & Second Image ...
	cv::Mat firstImgDescriptor, secondImgDescriptor;
	std::vector<cv::Mat> setImgsDescriptors;
	// Clustering parameters ...
	cv::Mat labels, centers;
	
	// 1 to 1 ...
	// Matches for the Direct & Invers matching
	std::vector<cv::DMatch> directMatches, inverseMatches, goodMatches, badMatches;
	// Maches for knn = 2
	std::vector<std::vector<cv::DMatch>> twoMatches;
	
	// 1 to N ...
	// Matches for the Direct & Invers matching
	std::vector<std::vector<cv::DMatch>> directMatchesSet, inverseMatchesSet, goodMatchesSet, badMatchesSet;
	// Maches for knn = 2
	std::vector<std::vector<std::vector<cv::DMatch>>> twoMatchesSet;

	//Masks
	cv::Mat matchingMask;
	std::vector<cv::Mat> matchingMasks;
	
	//Excel data
	ExcelManager *excelReader, *excelRecover;
	
	QStandardItemModel *model;
	std::vector<std::pair<int, float>> rankkData;

// Operators Declaration
	cv::FeatureDetector * ptrDetector;
	cv::DescriptorExtractor * ptrDescriptor;
	cv::DescriptorMatcher * ptrMatcher;
	cv::Feature2D * ptrDefault;

// Times
	double /*segmentationTime = 0,*/ detectionTime = 0, descriptionTime = 0, clusteringTime = 0, matchingTime = 0;
// Others
	float sumDistances = 0;
	float score = 0;
	std::vector<float> sumDistancesSet;
	std::vector<float> scoreSet;
	int bestScoreIndex = -1;
	std::string directoryPath;
	bool oneToN;
	int bestMatchesCount;
	double minKeypoints;
	int cpt;
	const QString exportFile = QDir::toNativeSeparators(QDir::currentPath()) + "\\Tests\\palmprint_registration_log_file.xlsx";
	QString inputFile;
	int prev_cursor_position;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	// cusomizing ToolTips :
	//qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white;}");
    ui->descriptorFreakSelectedPairsText->setPlaceholderText("Ex: 1 2 11 22 154 256...");
	
	QPixmap pixmap1(":/start-img.png");
	QIcon ButtonIcon1(pixmap1);
	ui->pushButton->setIcon(ButtonIcon1);

	QPixmap pixmap2(":/refresh-img.png");
	QIcon ButtonIcon2(pixmap2);
	ui->refreshBddImageNames->setIcon(ButtonIcon2);

	if (ui->oneToN->isChecked()) on_refreshBddImageNames_pressed();

	// Control check boxes
	connect(ui->oneToN, &QCheckBox::toggled, [=](bool checked) {
		if (checked){
			if (ui->matcher1toNtype1->isChecked()) {
				ui->matcherInlierLoweRatio->setEnabled(false);
				ui->matcherInlierLoweRatioText->setEnabled(false);
				ui->matcherInlierInversMatches->setEnabled(false);
				ui->matcherInlierNoTest->setChecked(true);
			}
			if (ui->imageExistsInBdd->isChecked()){
				ui->bddImageNames->setEnabled(true);
				ui->refreshBddImageNames->setEnabled(true);
			}
			else {
				ui->bddImageNames->setEnabled(false);
				ui->refreshBddImageNames->setEnabled(false);
			}
		}
		else {
			ui->matcherInlierLoweRatio->setEnabled(true);
			ui->matcherInlierLoweRatioText->setEnabled(true);
			ui->matcherInlierInversMatches->setEnabled(true);
		}
		if (!ui->matcherInlierLoweRatio->isChecked()){
			ui->matcherBruteForceCrossCheckLabel->setDisabled(ui->matcherInlierLoweRatio->isChecked() || (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()));
			ui->matcherBruteForceCrossCheckText->setDisabled(ui->matcherInlierLoweRatio->isChecked() || (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()));
		}
	});
	connect(ui->matcherInlierLoweRatio, &QCheckBox::toggled, [=](bool checked) {
		if (checked){
			ui->matcherBruteForceCrossCheckLabel->setEnabled(false);
			ui->matcherBruteForceCrossCheckText->setEnabled(false);
		}
		else {
			ui->matcherBruteForceCrossCheckLabel->setEnabled(true);
			ui->matcherBruteForceCrossCheckText->setEnabled(true);
		}
	});
	connect(ui->matcher1toNtype1, &QCheckBox::toggled, [=](bool checked) {
		ui->matcherInlierNoTest->setChecked(true);
		if (checked){
			ui->matcherBruteForceCrossCheckLabel->setEnabled(false);
			ui->matcherBruteForceCrossCheckText->setEnabled(false);
		}
		else {
			ui->matcherBruteForceCrossCheckLabel->setEnabled(true);
			ui->matcherBruteForceCrossCheckText->setEnabled(true);
		}
	});

	this->setWindowState(Qt::WindowMaximized);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runSIFT(int &excelColumn)
{
    //    SIFT( int nfeatures=0, int nOctaveLayers=3,
    //              double contrastThreshold=0.04, double edgeThreshold=10,
    //              double sigma=1.6);

    // Read Parameters ...
    int nfeatures = ui->siftNumFeatText->text().toInt();
    int nOctaveLayers = ui->siftNumOctText->text().toInt();
    double contrastThreshold = ui->siftContThreshText->text().toDouble();
    double edgeThreshold = ui->siftEdgeThreshText->text().toDouble();
    double sigma = ui->siftSigmaText->text().toDouble();

    // Create SIFT Objects ...
	ptrDefault = new cv::SIFT(nfeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);

	// Matcher
	if (ui->siftBruteForceCheck->isChecked())
	{
		ptrMatcher = new cv::BFMatcher(cv::NORM_L1);
	}
	else
	{
		ptrMatcher = new cv::FlannBasedMatcher();
	}
	
	try{
		excelReader->SetCellValue(++excelColumn, 0, QString::number(contrastThreshold));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(edgeThreshold));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(nfeatures));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(nOctaveLayers));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(sigma));

		QString siftBruteForceCheck = ui->siftBruteForceCheck->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(++excelColumn, 0, siftBruteForceCheck);
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - SIFT", e.what());
	}
}

void MainWindow::runSURF(int &excelColumn)
{
    //    SURF(double hessianThreshold=100,
    //                      int nOctaves=4, int nOctaveLayers=2,
    //                      bool extended=true, bool upright=false);

	// Read Parameters ...
    double hessainThreshold = ui->surfHessianThreshText->text().toDouble();
    int nOctaves = ui->surfNumOctavesText->text().toInt();
    int nOctaveLayers = ui->surfNumOctLayersText->text().toInt();
	bool extended = ui->surfExtendedText->isChecked();
	bool upright = !ui->surfUprightText->isChecked();

	// Create SURF Objects ...
	ptrDefault = new cv::SURF(hessainThreshold, nOctaves, nOctaveLayers, extended, upright);
	// Matcher
	if(ui->surfBruteForceCheck->isChecked())
    {
        ptrMatcher = new cv::BFMatcher(cv::NORM_L1);
    }
    else
    {
        ptrMatcher = new cv::FlannBasedMatcher();
    }

	try
	{

		excelReader->SetCellValue(++excelColumn, 0, QString::number(hessainThreshold));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(nOctaves));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(nOctaveLayers));

		QString surfExtendedCheck = extended ? "TRUE" : "FALSE";
		excelReader->SetCellValue(++excelColumn, 0, surfExtendedCheck);

		QString surfUprightCheck = upright ? "FALSE" : "TRUE";
		excelReader->SetCellValue(++excelColumn, 0, surfUprightCheck);

		QString surfBruteForceCheck = ui->surfBruteForceCheck->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(++excelColumn, 0, surfBruteForceCheck);
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - SURF", e.what());
	}
}

void MainWindow::runORB(int &excelColumn)
{
    //    ORB(int nfeatures = 500, float scaleFactor = 1.2f, int nlevels = 8, int edgeThreshold = 31,
    //                     int firstLevel = 0, int WTA_K=2, int scoreType=HARRIS_SCORE, int patchSize=31 );

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
	ptrDefault = new cv::ORB(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize);
	// Matcher
    ptrMatcher = new cv::BFMatcher(cv::NORM_HAMMING);

	try
	{
		excelReader->SetCellValue(++excelColumn, 0, QString::number(nfeatures));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(scaleFactor));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(nlevels));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(edgeThreshold));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(firstLevel));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(WTA_K));

		QString orbScoreType = ui->orbScoreHarrisRadioBtn->isChecked() ? "Harris" : "FAST";
		excelReader->SetCellValue(++excelColumn, 0, orbScoreType);
		excelReader->SetCellValue(++excelColumn, 0, QString::number(patchSize));
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - ORB", e.what());
	}
}

void MainWindow::runBRISK(int &excelColumn)
{
	//    BRISK(int thresh = 30, int octaves = 3,float patternScale = 1.0f);

	// Read Parameters ...
	float patternScale = ui->briskPatternScaleText->text().toFloat();
	int octaves = ui->briskOctavesText->text().toInt();
	int thresh = ui->briskThreshText->text().toInt();

	// Create BRISK Object ...
	ptrDefault = new cv::BRISK(thresh, octaves, patternScale);
	// Matcher
	ptrMatcher = new cv::BFMatcher(cv::NORM_HAMMING);
	/*QModelIndex index = ui->viewTable->model()->index(0, 2);
	ui->logPlainText->appendHtml(ui->viewTable->model()->data(index).toString());*/
//	ui->logPlainText->appendHtml(ui->viewTable->model()->data(ui->viewTable->model()->index(5, 3)).toString());

	try
	{
		excelReader->SetCellValue(++excelColumn, 0, QString::number(patternScale));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(octaves));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(thresh));
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - BRISK", e.what());
	}
}

void MainWindow::runDefault()
{
	// Get choices
	int methodIndex = ui->defaultTabs->currentIndex();
	std::string methodName = ui->defaultTabs->tabText(ui->defaultTabs->currentIndex()).toStdString();

	ui->logPlainText->appendHtml(QString::fromStdString("<b>Starting (" + methodName + ") based identification</b> "));

	if (oneToN) {
		setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>()); 
		setImgsDescriptors = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
		goodMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
		badMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
		sumDistancesSet = std::vector<float>(setImgs.size());
		scoreSet = std::vector<float>(setImgs.size());
	}
	QString curtime = getCurrentTime();
	int excelColumn;
	try
	{
		excelReader = new ExcelManager(true, exportFile, methodIndex + 1);
		excelReader->SetCellValue(1, 0, QString::number(cpt));
		excelReader->SetCellValue(2, 0, getCurrentTime());
		excelReader->SetCellValue(3, 0, ui->firstImgText->text());
		excelReader->SetCellValue(4, 0, ui->secondImgText->text());

		QString one2nImage = ui->oneToN->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(5, 0, one2nImage);

		if (ui->oneToN->isChecked())
		{
			QString imageExistsInBdd = ui->imageExistsInBdd->isChecked() ? "TRUE" : "FALSE";
			excelReader->SetCellValue(6, 0, imageExistsInBdd);
			if (ui->imageExistsInBdd->isChecked()) excelReader->SetCellValue(7, 0, ui->bddImageNames->currentText());
		}
		excelColumn = 7;
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - Excel", e.what());
	}
	switch (methodIndex) {
		case 0:
			// SIFT
			runSIFT(excelColumn);
			break;

		case 1:
			// SURF
			runSURF(excelColumn);
			break;

		case 2:
			// ORB
			runORB(excelColumn);
			break;

		case 3:
		default:
			// BRISK
			runBRISK(excelColumn);
			break;
	}

	// Detecting Keypoints ...
	detectionTime = (double)cv::getTickCount();
	ptrDefault->detect(firstImg, firstImgKeypoints);
	if (oneToN){
		for (int i = 0; i < setImgs.size(); i++){
			ptrDefault->detect(setImgs[i].second, setImgsKeypoints[i]);
		}
	}
	else ptrDefault->detect(secondImg, secondImgKeypoints);
	detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();

	writeKeyPoints(firstImg, firstImgKeypoints, 1, "KeyPoints1");
	if (!oneToN) writeKeyPoints(secondImg, secondImgKeypoints, 2, "KeyPoints2");
	if (noKeyPoints("first", firstImgKeypoints) || (!oneToN && noKeyPoints("second", secondImgKeypoints))) return;
	if (oneToN) {
		ui->logPlainText->textCursor().movePosition(QTextCursor::End);
		prev_cursor_position = ui->logPlainText->textCursor().position();
	}
	ui->logPlainText->appendPlainText("Detection time: " + QString::number(detectionTime) + " (s)");

	// Computing the descriptors
	descriptionTime = (double)cv::getTickCount();
	ptrDefault->compute(firstImg, firstImgKeypoints, firstImgDescriptor);
	if (oneToN){
		for (int i = 0; i < setImgs.size(); i++){
			ptrDefault->compute(setImgs[i].second, setImgsKeypoints[i], setImgsDescriptors[i]);
		}
	}
	else ptrDefault->compute(secondImg, secondImgKeypoints, secondImgDescriptor);
	descriptionTime = ((double)cv::getTickCount() - descriptionTime) / cv::getTickFrequency();
	ui->logPlainText->appendPlainText("Description time: " + QString::number(descriptionTime) + " (s)");

	// Only the best direct match
	matchingTime = (double)cv::getTickCount();
	if (oneToN){
		for (int i = 0; i < setImgs.size(); i++){
			ptrMatcher->match(firstImgDescriptor, setImgsDescriptors[i], goodMatchesSet[i]);
		}
	}
	else {
		ptrMatcher->match(firstImgDescriptor, secondImgDescriptor, goodMatches);
	}
	matchingTime = ((double)cv::getTickCount() - matchingTime) / cv::getTickFrequency();
	ui->logPlainText->appendPlainText("Matching time: " + QString::number(matchingTime) + " (s)");

	ui->logPlainText->appendPlainText("Total time: " + QString::number(detectionTime + descriptionTime + clusteringTime + matchingTime) + " (s)");
	
	// Scores
	float bestScore = 0;
	if (oneToN){
		for (int i = 0; i < setImgs.size(); i++){
			for (int j = 0; j < goodMatchesSet[i].size(); j++){
				sumDistancesSet[i] += goodMatchesSet[i][j].distance;
			}
			if (goodMatchesSet[i].size() > 0){
				float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size()) * 100;
				float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
				scoreSet[i] = 1.0 / average * goodProbability;
				// update the best score index
				if (scoreSet[i] >= bestScore) {
					if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScore].size()){
						bestScoreIndex = i;
						bestScore = scoreSet[i];
					}
				}
			}
			else scoreSet[i] = 0.0;
		}
	}
	else { // 1 to 1
		for (int i = 0; i < goodMatches.size(); i++){
			sumDistances += goodMatches[i].distance;
		}
		if (goodMatches.size() > 0){
			float average = sumDistances / static_cast<float>(goodMatches.size());
			float goodProbability = static_cast<float>(goodMatches.size()) / static_cast<float>(goodMatches.size()) * 100;
			score = 1.0 / average * goodProbability;
		}
		else score = 0.0;
	}

	try{
		excelReader->SetCellValue(excelColumn + 1, 0, ui->decisionStageThresholdText->text());

		excelReader->SetCellValue(excelColumn + 2, 0, QString::number(firstImgKeypoints.size()));

		excelReader->SetCellValue(excelColumn + 4, 0, QString::number(detectionTime) + " (s)");
		excelReader->SetCellValue(excelColumn + 5, 0, QString::number(descriptionTime) + " (s)");
		excelReader->SetCellValue(excelColumn + 6, 0, QString::number(matchingTime) + " (s)");
		excelReader->SetCellValue(excelColumn + 7, 0, QString::number(detectionTime + descriptionTime + matchingTime) + " (s)");

		if (!ui->oneToN->isChecked()) {
			excelReader->SetCellValue(excelColumn + 3, 0, QString::number(secondImgKeypoints.size()));
			excelReader->SetCellValue(excelColumn + 8, 0, QString::number(goodMatches.size()));
			excelReader->SetCellValue(excelColumn + 9, 0, QString::number(badMatches.size()));
			excelReader->SetCellValue(excelColumn + 10, 0, QString::number(sumDistances / static_cast<float>(goodMatches.size())));
			excelReader->SetCellValue(excelColumn + 11, 0, QString::number(score));
		}
		else {
			if (bestScoreIndex > -1)
			{
				excelReader->SetCellValue(excelColumn + 3, 0, QString::number(setImgsKeypoints[bestScoreIndex].size()));
				excelReader->SetCellValue(excelColumn + 8, 0, QString::number(goodMatchesSet[bestScoreIndex].size()));
				excelReader->SetCellValue(excelColumn + 9, 0, QString::number(badMatchesSet[bestScoreIndex].size()));
				excelReader->SetCellValue(excelColumn + 10, 0, QString::number(sumDistancesSet[bestScoreIndex] / static_cast<float>(goodMatchesSet[bestScoreIndex].size())));
				excelReader->SetCellValue(excelColumn + 11, 0, QString::number(scoreSet[bestScoreIndex]));
				excelReader->SetCellValue(excelColumn + 13, 0, QString::fromStdString(setImgs[bestScoreIndex].first));
			}
			if (ui->imageExistsInBdd->isChecked() && ui->imageExistsInBdd->isEnabled()) {
				if (ui->bddImageNames->currentIndex() > -1) excelReader->SetCellValue(excelColumn + 12, 0, QString::number(scoreSet[ui->bddImageNames->currentIndex()]));
				float scoreThreshold = ui->decisionStageThresholdText->text().toFloat();
				excelReader->SetCellValue(excelColumn + 14, 0, QString::number(computeRankK(scoreThreshold)));
			}
		}

		excelReader->~ExcelManager();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - Excel", e.what());
	}
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

	ui->logPlainText->appendHtml(QString::fromStdString("<b>Starting (" + segmentationName + ", " + detectorName + ", " + descriptorName + ", " + matcherName + ") based identification</b> "));
	
	// Binarization
	customisingBinarization(segmentationIndex);

	// Customising Segmentor...
	customisingSegmentor(segmentationIndex);

	// Customising Detector...	
	customisingDetector(detectorIndex, detectorName);

	// Customising Descriptor...
	customisingDescriptor(descriptorIndex, descriptorName);

	// Clustering descriptor
	if (false){
		clustering();
	}

	// Customising Matcher...
	customisingMatcher(matcherIndex, matcherName);

	// Find the matching points
	if (!matching()) return;

	ui->logPlainText->appendPlainText("Total time: " + QString::number(detectionTime + descriptionTime + clusteringTime + matchingTime) + " (s)");

	// Keep only best matching according to the selected test
	outlierElimination();

	QString curtime = getCurrentTime();

	try
	{

		excelReader = new ExcelManager(true, exportFile, 5);

		excelReader->SetCellValue(1, 1, QString::number(cpt));
		excelReader->SetCellValue(2, 1, curtime);
		excelReader->SetCellValue(3, 1, ui->firstImgText->text());
		excelReader->SetCellValue(4, 1, ui->secondImgText->text());
		QString one2nImage = ui->oneToN->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(5, 1, one2nImage);

		if (ui->oneToN->isChecked())
		{
			QString imageExistsInBdd = ui->imageExistsInBdd->isChecked() ? "TRUE" : "FALSE";
			excelReader->SetCellValue(6, 1, imageExistsInBdd);
			if (ui->imageExistsInBdd->isChecked()) excelReader->SetCellValue(7, 1, ui->bddImageNames->currentText());
			else excelReader->mergeRowsCells(7);
		}
		else
		{
			excelReader->mergeRowsCells(6);
			excelReader->mergeRowsCells(7);
		}

		excelReader->SetCellValue(8, 1, QString::fromStdString(segmentationName));

		excelReader->SetCellValue(9, 2, ui->segmentationThresholdLabel->text());
		excelReader->SetCellValueSecondRow(9, ui->segmentationThresholdText->text());

		switch (segmentationIndex)
		{
			case 1:
			{
				// Skeletonization of Morphological Skeleton
				excelReader->SetCellValue(10, 2, ui->segmentationMorphologicalSkeletonParam1Label->text());
				excelReader->SetCellValueSecondRow(10, ui->segmentationMorphologicalSkeletonParam1Text->text());
				excelReader->SetCellValue(11, 2, ui->segmentationMorphologicalSkeletonParam2Label->text());
				excelReader->SetCellValueSecondRow(11, ui->segmentationMorphologicalSkeletonParam2Text->text());
				excelReader->SetCellValue(12, 2, ui->segmentationMorphologicalSkeletonParam3Label->text());
				excelReader->SetCellValueSecondRow(12, ui->segmentationMorphologicalSkeletonParam3Text->text());
				break;
			}
			case 2:
			{
				// Thinning of Zhang-Suen
				excelReader->SetCellValue(10, 2, ui->segmentationZhangSuenParam1Label->text());
				excelReader->SetCellValueSecondRow(10, ui->segmentationZhangSuenParam1Text->text());
				excelReader->SetCellValue(11, 2, ui->segmentationZhangSuenParam2Label->text());
				excelReader->SetCellValueSecondRow(11, ui->segmentationZhangSuenParam2Text->text());
				excelReader->SetCellValue(12, 2, ui->segmentationZhangSuenParam3Label->text());
				excelReader->SetCellValueSecondRow(12, ui->segmentationZhangSuenParam3Text->text());
				break;
			}
			case 3:
			{
				// Thinning of Lin-Hong implemented by Mrs. Faiçal
				excelReader->SetCellValue(10, 2, ui->segmentationLinHongParam1Label->text());
				excelReader->SetCellValueSecondRow(10, ui->segmentationLinHongParam1Text->text());
				excelReader->SetCellValue(11, 2, ui->segmentationLinHongParam2Label->text());
				excelReader->SetCellValueSecondRow(11, ui->segmentationLinHongParam2Text->text());
				excelReader->SetCellValue(12, 2, ui->segmentationLinHongParam3Label->text());
				excelReader->SetCellValueSecondRow(12, ui->segmentationLinHongParam3Text->text());
				break;
			}
			case 4:
			{
				// Thinning of Guo-Hall
				excelReader->SetCellValue(10, 2, ui->segmentationGuoHallParam1Label->text());
				excelReader->SetCellValueSecondRow(10, ui->segmentationGuoHallParam1Text->text());
				excelReader->SetCellValue(11, 2, ui->segmentationGuoHallParam2Label->text());
				excelReader->SetCellValueSecondRow(11, ui->segmentationGuoHallParam2Text->text());
				excelReader->SetCellValue(12, 2, ui->segmentationGuoHallParam3Label->text());
				excelReader->SetCellValueSecondRow(12, ui->segmentationGuoHallParam3Text->text());
				break;
			}
			default:
			{
				excelReader->mergeCellsCustom(10, 12);
			}
		}

		excelReader->SetCellValue(13, 1, QString::fromStdString(detectorName));
		switch (detectorIndex){
			case 0:{
				// Minutiae-detection using Crossing Number By Dr. Faiçal
				excelReader->SetCellValue(14, 2, ui->detectorMinutiaeParam1Label->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorMinutiaeParam1Text->text());
				excelReader->SetCellValue(15, 2, ui->detectorMinutiaeParam2Label->text());
				excelReader->SetCellValueSecondRow(15, ui->detectorMinutiaeParam2Text->text());
				excelReader->mergeCellsCustom(16, 18);
				break;
			}
			case 1:{
				// Minutiae-detection using Crossing Number
				excelReader->SetCellValue(14, 2, ui->detectorCrossingNumberBorderLabel->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorCrossingNumberBorderText->text());
				excelReader->SetCellValue(15, 2, ui->detectorCrossingNumberParamLabel->text());
				excelReader->SetCellValueSecondRow(15, ui->detectorCrossingNumberParamText->text());
				excelReader->mergeCellsCustom(16, 18);
				break;
			}
			case 2:{
				// Harris-Corners
				excelReader->SetCellValue(14, 2, ui->detectorHarrisThresholdLabel->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorHarrisThresholdText->text());
				excelReader->SetCellValue(15, 2, ui->detectorHarrisParamLabel->text());
				excelReader->SetCellValueSecondRow(15, ui->detectorHarrisParamText->text());
				excelReader->mergeCellsCustom(16, 18);
				break;
			}
			case 3:{
				// STAR
				excelReader->SetCellValue(14, 2, ui->detectorStarMaxSizeLabel->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorStarMaxSizeText->text());
				excelReader->SetCellValue(15, 2, ui->detectorStarResponseThresholdLabel->text());
				excelReader->SetCellValueSecondRow(15, ui->detectorStarResponseThresholdText->text());
				excelReader->SetCellValue(16, 2, ui->detectorStarLineThresholdProjectedLabel->text());
				excelReader->SetCellValueSecondRow(16, ui->detectorStarThresholdProjectedText->text());
				excelReader->SetCellValue(17, 2, ui->detectorStarLineThresholdBinarizedLabel->text());
				excelReader->SetCellValueSecondRow(17, ui->detectorStarThresholdBinarizedText->text());
				excelReader->SetCellValue(18, 2, ui->detectorStarSuppressNonmaxSizeLabel->text());
				excelReader->SetCellValueSecondRow(18, ui->detectorStarSuppressNonmaxSizeText->text());
				break;
			}
			case 4: {
				// FAST
				QString detectorFastNonmaxSuppressionCheck = ui->detectorFastNonmaxSuppressionCheck->isChecked() ? "TRUE" : "FALSE";
				QString detectorFastXCheck = ui->detectorFastXCheck->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(14, 2, ui->detectorFastThresholdLabel->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorFastThresholdText->text());
				excelReader->SetCellValue(15, 2, ui->detectorFastNonmaxSuppressionLabel->text());
				excelReader->SetCellValueSecondRow(15, detectorFastNonmaxSuppressionCheck);
				excelReader->SetCellValue(16, 2, ui->detectorFastXLabel->text());
				excelReader->SetCellValueSecondRow(16, detectorFastXCheck);
				if (ui->detectorFastXCheck->isChecked())
				{
					excelReader->SetCellValue(17, 2, ui->detectorFastTypeLabel->text());
					excelReader->SetCellValueSecondRow(17, ui->detectorFastTypeText->currentText());
					excelReader->mergeRowsCells(18);
				}
				else excelReader->mergeCellsCustom(17, 18);
				break;
			}
			case 5: {
				// SIFT
				excelReader->SetCellValue(14, 2, ui->detectorSiftContrastThresholdLabel->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorSiftContrastThresholdText->text());
				excelReader->SetCellValue(15, 2, ui->detectorSiftEdgeThresholdLab->text());
				excelReader->SetCellValueSecondRow(15, ui->detectorSiftEdgeThresholdText->text());
				excelReader->SetCellValue(16, 2, ui->detectorSiftNfeaturesLabel->text());
				excelReader->SetCellValueSecondRow(16, ui->detectorSiftNfeaturesText->text());
				excelReader->SetCellValue(17, 2, ui->detectorSiftNOctaveLayersLabel->text());
				excelReader->SetCellValueSecondRow(17, ui->detectorSiftNOctaveLayersText->text());
				excelReader->SetCellValue(18, 2, ui->detectorSiftSigmaLabel->text());
				excelReader->SetCellValueSecondRow(18, ui->detectorSiftSigmaText->text());
				break;
			}
			case 6: {
				//SURF
				QString detectorSurfUprightText = ui->detectorSurfUprightText->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(14, 2, ui->detectorSurfHessianThresholdLabel->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorSurfHessianThresholdText->text());
				excelReader->SetCellValue(15, 2, ui->detectorSurfNOctavesLabel->text());
				excelReader->SetCellValueSecondRow(15, ui->detectorSurfNOctavesText->text());
				excelReader->SetCellValue(16, 2, ui->detectorSurfNLayersLabel->text());
				excelReader->SetCellValueSecondRow(16, ui->detectorSurfNLayersText->text());
				excelReader->SetCellValue(17, 2, ui->detectorSurfUprightLabel->text());
				excelReader->SetCellValueSecondRow(17, detectorSurfUprightText);
				excelReader->mergeRowsCells(18);
				break;
			}
			case 7: {
				//Dense
				excelReader->SetCellValue(14, 2, ui->detectorDenseInitFeatureScaleLabel->text());
				excelReader->SetCellValueSecondRow(14, ui->detectorDenseInitFeatureScaleText->text());
				excelReader->SetCellValue(15, 2, ui->detectorDenseFeatureScaleLevelsLabel->text());
				excelReader->SetCellValueSecondRow(15, ui->detectorDenseFeatureScaleLevelsText->text());
				excelReader->SetCellValue(16, 2, ui->detectorDenseFeatureScaleMulLabel->text());
				excelReader->SetCellValueSecondRow(16, ui->detectorDenseFeatureScaleMulText->text());
				excelReader->SetCellValue(17, 2, ui->detectorDenseInitXyStepLabel->text());
				excelReader->SetCellValueSecondRow(17, ui->detectorDenseInitXyStepText->text());
				excelReader->SetCellValue(18, 2, ui->detectorDenseInitImgBoundLabel->text());
				excelReader->SetCellValueSecondRow(18, ui->detectorDenseInitImgBoundText->text());
				break;
			}
		}

		excelReader->SetCellValue(19, 1, QString::fromStdString(descriptorName));
		switch (descriptorIndex)
		{
			case 0:
			{
				// FREAK
				QString descriptorFreakOrientationNormalizedCheck = ui->descriptorFreakOrientationNormalizedCheck->isChecked() ? "TRUE" : "FALSE";
				QString descriptorFreakScaleNormalizedCheck = ui->descriptorFreakScaleNormalizedCheck->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(20, 2, ui->descriptorFreakOrientationNormalizedLabel->text());
				excelReader->SetCellValueSecondRow(20, descriptorFreakOrientationNormalizedCheck);
				excelReader->SetCellValue(21, 2, ui->descriptorFreakScaleNormalizedLabel->text());
				excelReader->SetCellValueSecondRow(21, descriptorFreakScaleNormalizedCheck);
				excelReader->SetCellValue(22, 2, ui->descriptorFreakPatternScaleLabel->text());
				excelReader->SetCellValueSecondRow(22, ui->descriptorFreakPatternScaleText->text());
				excelReader->SetCellValue(23, 2, ui->descriptorFreakNOctavesLabel->text());
				excelReader->SetCellValueSecondRow(23, ui->descriptorFreakNOctavesText->text());
				excelReader->SetCellValue(24, 2, ui->descriptorFreakNelectedPairsLabel->text());
				excelReader->SetCellValueSecondRow(24, ui->descriptorFreakSelectedPairsText->text());
				break;
			}
			case 1:
			{
				// BRIEF
				excelReader->SetCellValue(20, 2, ui->descriptorBriefPATCH_SIZELabel->text());
				excelReader->SetCellValueSecondRow(20, ui->descriptorBriefPATCH_SIZEText->text());
				excelReader->SetCellValue(21, 2, ui->descriptorBriefKERNEL_SIZELabel->text());
				excelReader->SetCellValueSecondRow(21, ui->descriptorBriefKERNEL_SIZEText->text());
				excelReader->SetCellValue(22, 2, ui->descriptorBriefLengthLabel->text());
				excelReader->SetCellValueSecondRow(22, ui->descriptorBriefLengthText->text());
				excelReader->mergeCellsCustom(23, 24);
				break;
			}
			case 2:
			{
				// SIFT
				excelReader->SetCellValue(20, 2, ui->descriptorSiftLengthLabel->text());
				excelReader->SetCellValueSecondRow(20, ui->descriptorSiftLengthText->text());
				excelReader->mergeCellsCustom(21, 24);
				break;
			}
			case 3:
			{
				//SURF
				QString descriptorSurfExtended = ui->descriptorSurfExtended->isChecked() ? "128" : "64";

				excelReader->SetCellValue(20, 2, ui->descriptorSurfExtendedLabel->text());
				excelReader->SetCellValueSecondRow(20, descriptorSurfExtended);
				excelReader->mergeCellsCustom(21, 24);
				break;
			}
			default:
			{
				excelReader->SetCellValue(20, 2, ui->descriptorMethod2Param1Label->text());
				excelReader->SetCellValueSecondRow(20, ui->descriptorMethod2Param1Text->text());
				excelReader->SetCellValue(21, 2, ui->descriptorMethod2Param2Label->text());
				excelReader->SetCellValueSecondRow(21, ui->descriptorMethod2Param2Text->text());
				excelReader->SetCellValue(22, 2, ui->descriptorMethod2Param3Label->text());
				excelReader->SetCellValueSecondRow(22, ui->descriptorMethod2Param3Text->text());
				excelReader->SetCellValue(23, 2, ui->descriptorMethod2LengthLabel->text());
				excelReader->SetCellValueSecondRow(23, ui->descriptorMethod2LengthText->text());
				excelReader->mergeRowsCells(24);
				break;
			}
		}

		excelReader->SetCellValue(25, 1, QString::fromStdString(matcherName));

		QString matcher1toNtype = ui->matcher1toNtype1->isChecked() ? "Type 1" : "Type 2";
		excelReader->SetCellValue(26, 2, ui->matcher1toNtypeLabel->text());
		excelReader->SetCellValueSecondRow(26, matcher1toNtype);

		excelReader->SetCellValue(27, 2, ui->matcherKBestLabel->text());
		if (ui->matcherInlierNoTest->isChecked()) excelReader->SetCellValueSecondRow(27, ui->matcherInlierNoTest->text());
		else {
			if (ui->matcherInlierInversMatches->isChecked()) excelReader->SetCellValueSecondRow(27, ui->matcherInlierInversMatches->text());
			else excelReader->SetCellValueSecondRow(27, ui->matcherInlierLoweRatio->text() + ":" + ui->matcherInlierLoweRatioText->text());
		}
		if (ui->matcherInlierLimitDistance->isChecked()) {
			excelReader->SetCellValue(28, 2, ui->matcherInlierLimitDistance->text());
			excelReader->SetCellValueSecondRow(28, ui->matcherInlierLimitDistanceText->text());
		}
		else excelReader->mergeRowsCells(28);

		switch (matcherIndex)
		{
			case 0:
			{
				// BruteForce
				QString matcherBruteForceCrossCheckText = ui->matcherBruteForceCrossCheckText->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(29, 2, ui->matcherBruteForceNormTypeLabel->text());
				excelReader->SetCellValueSecondRow(29, ui->matcherBruteForceNormTypeText->currentText());
				excelReader->SetCellValue(30, 2, ui->matcherBruteForceCrossCheckLabel->text());
				excelReader->SetCellValueSecondRow(30, matcherBruteForceCrossCheckText);
				excelReader->mergeRowsCells(31);
				break;
			}
			case 1:
			{
				// FlannBased
				excelReader->SetCellValue(29, 2, ui->matcherFlannBasedIndexParamsLabel->text());
				excelReader->SetCellValueSecondRow(29, getFlannBasedNameParamsType());
				excelReader->SetCellValue(30, 2, ui->matcherFlannBasedSearchParamsLabel->text());
				excelReader->SetCellValueSecondRow(30, ui->matcherFlannBasedSearchParamsText->text());
				excelReader->mergeRowsCells(31);
				break;
			}
			default:
			{
				excelReader->SetCellValue(29, 2, ui->matcherMethodParam1Label->text());
				excelReader->SetCellValueSecondRow(29, ui->matcherMethodParam1Text->text());
				excelReader->SetCellValue(30, 2, ui->matcherMethodParam2Label->text());
				excelReader->SetCellValueSecondRow(30, ui->matcherMethodParam2Text->text());
				excelReader->SetCellValue(31, 2, ui->matcherMethodParam3Label->text());
				excelReader->SetCellValueSecondRow(31, ui->matcherMethodParam3Text->text());
				break;
			}
		}

		QString opponentColor = ui->opponentColor->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(32, 1, opponentColor); 
		
		excelReader->SetCellValue(33, 1, ui->decisionStageThresholdText->text());

		excelReader->SetCellValue(34, 1, QString::number(firstImgKeypoints.size()));

		excelReader->SetCellValue(36, 1, QString::number(detectionTime) + " (s)");
		excelReader->SetCellValue(37, 1, QString::number(descriptionTime) + " (s)");
		excelReader->SetCellValue(38, 1, QString::number(matchingTime) + " (s)");
		excelReader->SetCellValue(39, 1, QString::number(detectionTime + descriptionTime + matchingTime) + " (s)");

		if (!ui->oneToN->isChecked()) {
			excelReader->SetCellValue(35, 1, QString::number(secondImgKeypoints.size()));
			excelReader->SetCellValue(40, 1, QString::number(goodMatches.size()));
			excelReader->SetCellValue(41, 1, QString::number(badMatches.size()));
			excelReader->SetCellValue(42, 1, QString::number(sumDistances / static_cast<float>(goodMatches.size())));
			excelReader->SetCellValue(43, 1, QString::number(score));
			excelReader->mergeRowsCells(44);
			excelReader->mergeRowsCells(45);
			excelReader->mergeRowsCells(46);
		}
		else {
			if (bestScoreIndex > -1)
			{
				excelReader->SetCellValue(35, 1, QString::number(setImgsKeypoints[bestScoreIndex].size()));
				excelReader->SetCellValue(40, 1, QString::number(goodMatchesSet[bestScoreIndex].size()));
				excelReader->SetCellValue(41, 1, QString::number(badMatchesSet[bestScoreIndex].size()));
				excelReader->SetCellValue(42, 1, QString::number(sumDistancesSet[bestScoreIndex] / static_cast<float>(goodMatchesSet[bestScoreIndex].size())));
				excelReader->SetCellValue(43, 1, QString::number(scoreSet[bestScoreIndex]));
				excelReader->SetCellValue(45, 1, QString::fromStdString(setImgs[bestScoreIndex].first));
			}
			else
			{
				excelReader->mergeRowsCells(35);
				excelReader->mergeRowsCells(40);
				excelReader->mergeRowsCells(41);
				excelReader->mergeRowsCells(42);
				excelReader->mergeRowsCells(43);
				excelReader->mergeRowsCells(45);
			}
			if (ui->imageExistsInBdd->isChecked() && ui->imageExistsInBdd->isEnabled()) {
				if (ui->bddImageNames->currentIndex() > -1) excelReader->SetCellValue(44, 1, QString::number(scoreSet[ui->bddImageNames->currentIndex()]));
				else excelReader->mergeRowsCells(44);
				float scoreThreshold = ui->decisionStageThresholdText->text().toFloat();
				excelReader->SetCellValue(46, 1, QString::number(computeRankK(scoreThreshold)));
			}
			else
			{
				excelReader->mergeRowsCells(44); 
				excelReader->mergeRowsCells(46);
			}
		}

		excelReader->~ExcelManager();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - Custom", e.what());
	}
}

void MainWindow::launchInCMD(){
	ui->tabWidget_2->setCurrentIndex(2);
	on_pushButton_pressed();
}

void MainWindow::on_firstImgBtn_pressed()
{
	// Read First Image ...
	QString str = QFileDialog::getOpenFileName(0, ("Select the 1st Image"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->firstImgText->setText(str);
}

void MainWindow::on_secondImgBtn_pressed()
{
	// Read Second Image(s) ...
	QString str = (ui->oneToN->isChecked()) ? QFileDialog::getExistingDirectory(0, ("Select a Folder"), QDir::currentPath()) : QFileDialog::getOpenFileName(0, ("Select the 2nd Image"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->secondImgText->setText(str);
}

void MainWindow::on_inputBrowse_pressed()
{
	// Read Input File ...
	QString str = QFileDialog::getOpenFileName(0, ("Select the input file"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->inputPath->setText(str);
}

void MainWindow::on_refreshBddImageNames_pressed()
{
	// Reload image names ...
	if (ui->oneToN->isChecked()){
		readSetOfImages();
	}
}

void MainWindow::on_pushButton_pressed()
{
	switch (ui->tabWidget_2->currentIndex())
	{
		case 0:
		{
			if (takeTest()) exportSuccess(0);
			break;
		}
		case 1:
		{
			importExcelFile(0);
			break;
		}
		case 2:
		default:
		{
			if (!readInputFile()) {
				ui->logPlainText->appendHtml("<b style='color:red'>Error while trying to read the excel file!</b>");
				return;
			}
			importExcelFile(2);
		}
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

void MainWindow::on_refreshRankkGraph_pressed(){
	std::vector<int> rankkDataFromExcel;
	int rankValue, column, segmentationName, detectorName, descriptorName, matcherName;
	int nbRank0FromExcel = 0, maxRank = 0;

	excelRecover = new ExcelManager(true, exportFile, 0);
	if (ui->allMethodsTabs->currentIndex() == 0) excelRecover->GetIntRows(ui->defaultTabs->currentIndex() + 1);
	else excelRecover->GetIntRows(5);
	column = excelRecover->getColumnsCount();
	for (int j = 2; j <= excelRecover->getSheetCount(); j++) {
		if (excelRecover->GetCellValue(j, 5).toBool() && excelRecover->GetCellValue(j, 6).toBool() && !excelRecover->GetCellValue(j, column).toString().isEmpty())
			if (ui->allMethodsTabs->currentIndex() == 0)
			{
				rankValue = excelRecover->GetCellValue(j, column).toString().toInt();
				if (rankValue == 0) nbRank0FromExcel++;
				else rankkDataFromExcel.push_back(rankValue);
			}
			else
			{
				segmentationName = segmentationNameToInt(excelRecover->GetCellValue(j, 8).toString());
				detectorName = detectorNameToInt(excelRecover->GetCellValue(j, 13).toString());
				descriptorName = descriptorNameToInt(excelRecover->GetCellValue(j, 19).toString());
				matcherName = matcherNameToInt(excelRecover->GetCellValue(j, 25).toString());
				if ((ui->segmentationTabs->currentIndex() == segmentationName) && (ui->detectorTabs->currentIndex() == detectorName) && (ui->descriptorTabs->currentIndex() == descriptorName) && (ui->matcherTabs->currentIndex() == matcherName))
				{
					rankValue = excelRecover->GetCellValue(j, column).toString().toInt();
					if (rankValue == 0) nbRank0FromExcel++;
					else rankkDataFromExcel.push_back(rankValue);
				}
				j++;
			}
	}
	excelRecover->~ExcelManager();

	maxRank = rankkDataFromExcel.size();
	
	if (ui->rankkGraphWidget->graphCount()){ 
		ui->rankkGraphWidget->clearGraphs();
		ui->rankkGraphWidget->clearItems();
	}
	if (maxRank > 0){
		std::sort(rankkDataFromExcel.begin(), rankkDataFromExcel.end());
		rankkData = std::vector<std::pair<int, float>>(rankkDataFromExcel[maxRank - 1], std::pair<int, float>());

		int cpt = maxRank, toretrive = 0;
		rankkData[rankkDataFromExcel[maxRank - 1] - 1] = std::make_pair(rankkDataFromExcel[maxRank - 1], cpt);
		for (int i = maxRank - 2; i > 0; i--){
			if (rankkDataFromExcel[i] != rankkDataFromExcel[i + 1]) {
				cpt--;
				rankkData[rankkDataFromExcel[i] - 1] = std::make_pair(rankkDataFromExcel[i], cpt);
			}
			else cpt--;
		}
		if (rankkData[0].first == 0)rankkData[0] = std::make_pair(1, 0);
		for (int i = 1; i < rankkData.size(); i++)
		{
			if (rankkData[i].first == 0)rankkData[i] = std::make_pair(i + 1, rankkData[i - 1].second);
		}
		rankkData.insert(rankkData.begin(), std::make_pair<int, float>(0, static_cast<float>(nbRank0FromExcel) / static_cast<float>(maxRank + nbRank0FromExcel) * 100));
		connect(ui->rankkGraphWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showRankkToolTip(QMouseEvent*)));
		drowRankk(maxRank);
	}
	else {
		QMessageBox::warning(this, "Show Rank-k Graph", "No data to show! You can Show Rank-k Graph after launching some tests!");
		//disconnect(ui->rankkGraphWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showRankkToolTip(QMouseEvent*)));
	}
}

void MainWindow::on_refreshEerGraph_pressed(){
	std::map<float, std::pair<int, int>>/*<threshold, <nbFM, nbNonExists>>*/ FMR_dataFromExcel = {};
	std::map<float, std::pair<int, int>>::iterator FMR_itr = {};

	std::map<float, std::pair<int, int>>/*<threshold, <nbFNM, nbExists>>*/ FNMR_dataFromExcel = {};
	std::map<float, std::pair<int, int>>::iterator FNMR_itr = {};
		
	int column, segmentationName, detectorName, descriptorName, matcherName;
	QString bestScore, requestedScore, scoreThreshold;

	excelRecover = new ExcelManager(true, exportFile, 0);
	if (ui->allMethodsTabs->currentIndex() == 0) excelRecover->GetIntRows(ui->defaultTabs->currentIndex() + 1);
	else excelRecover->GetIntRows(5);
	column = excelRecover->getColumnsCount();

	for (int j = 2; j <= excelRecover->getSheetCount(); j++) {

		scoreThreshold = excelRecover->GetCellValue(j, column - 13).toString();

		if (excelRecover->GetCellValue(j, 5).toBool() && (!scoreThreshold.isEmpty()))
		{

			if (ui->allMethodsTabs->currentIndex() == 0)
			{
				if (excelRecover->GetCellValue(j, 6).toBool())
				{
					FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold.toFloat());
					if (FNMR_itr == FNMR_dataFromExcel.end()) FNMR_dataFromExcel.insert(std::make_pair(scoreThreshold.toFloat(), std::make_pair(0, 1)));
					else FNMR_itr->second.second = FNMR_itr->second.second + 1;

					requestedScore = excelRecover->GetCellValue(j, column - 2).toString();
					if ((!requestedScore.isEmpty()))
						if (requestedScore.toFloat() < scoreThreshold.toFloat())
						{
							FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold.toFloat());
							FNMR_itr->second.first = FNMR_itr->second.first + 1;
						}
				}
				else
				{
					FMR_itr = FMR_dataFromExcel.find(scoreThreshold.toFloat());
					if (FMR_itr == FMR_dataFromExcel.end()) FMR_dataFromExcel.insert(std::make_pair(scoreThreshold.toFloat(), std::make_pair(0, 1)));
					else FMR_itr->second.second = FMR_itr->second.second + 1;

					bestScore = excelRecover->GetCellValue(j, column - 3).toString();
					if ((!bestScore.isEmpty()))
						if (bestScore.toFloat() >= scoreThreshold.toFloat())
						{
							FMR_itr = FMR_dataFromExcel.find(scoreThreshold.toFloat());
							FMR_itr->second.first = FMR_itr->second.first + 1;
						}
				}

			}
			else
			{
				segmentationName = segmentationNameToInt(excelRecover->GetCellValue(j, 8).toString());
				detectorName = detectorNameToInt(excelRecover->GetCellValue(j, 13).toString());
				descriptorName = descriptorNameToInt(excelRecover->GetCellValue(j, 19).toString());
				matcherName = matcherNameToInt(excelRecover->GetCellValue(j, 25).toString());

				if ((ui->segmentationTabs->currentIndex() == segmentationName) && (ui->detectorTabs->currentIndex() == detectorName) && (ui->descriptorTabs->currentIndex() == descriptorName) && (ui->matcherTabs->currentIndex() == matcherName))
				{
					if (excelRecover->GetCellValue(j, 6).toBool())
					{
						FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold.toFloat());
						if (FNMR_itr == FNMR_dataFromExcel.end()) FNMR_dataFromExcel.insert(std::make_pair(scoreThreshold.toFloat(), std::make_pair(0, 1)));
						else FNMR_itr->second.second = FNMR_itr->second.second + 1;

						requestedScore = excelRecover->GetCellValue(j, column - 2).toString();
						if ((!requestedScore.isEmpty()))	
							if (requestedScore.toFloat() < scoreThreshold.toFloat())
							{
								FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold.toFloat());
								FNMR_itr->second.first = FNMR_itr->second.first + 1;
							}
					}
					else
					{
						FMR_itr = FMR_dataFromExcel.find(scoreThreshold.toFloat());
						if (FMR_itr == FMR_dataFromExcel.end()) FMR_dataFromExcel.insert(std::make_pair(scoreThreshold.toFloat(), std::make_pair(0, 1)));
						else FMR_itr->second.second = FMR_itr->second.second + 1;

						bestScore = excelRecover->GetCellValue(j, column - 3).toString();
						if ((!bestScore.isEmpty()))
							if (bestScore.toFloat() >= scoreThreshold.toFloat())
							{
								FMR_itr = FMR_dataFromExcel.find(scoreThreshold.toFloat());
								FMR_itr->second.first = FMR_itr->second.first + 1;
							}
					}
				}
				j++;
			}
		}
	}
	excelRecover->~ExcelManager();

	if (ui->eerGraphWidget->graphCount()){
		ui->eerGraphWidget->clearGraphs();
		ui->eerGraphWidget->clearItems();
	}

	if (FMR_dataFromExcel.empty() || FNMR_dataFromExcel.empty()) QMessageBox::warning(this, "Show EER Graph", "No data to show! You can Show EER Graph after having some FM et FNM tests !");
	else drowEer(FMR_dataFromExcel, FNMR_dataFromExcel);
}

int MainWindow::computeRankK(float scoreThreshold){
	int rankK = 0;
	std::vector<std::pair<float, int>> scoreSetCollection;
	for (int i = 0; i < scoreSet.size(); i++) scoreSetCollection.push_back(std::make_pair(scoreSet[i], i));
	// Sort from the best score to the least
	std::sort(scoreSetCollection.begin(), scoreSetCollection.end(), std::greater<std::pair<float, int>>());
	while ((rankK < scoreSetCollection.size()) && (scoreSetCollection[rankK].first > scoreThreshold)){
		if (scoreSetCollection[rankK].second == ui->bddImageNames->currentIndex()){
			rankK++;
			return rankK; 
		}
		else rankK++;
	}
	return 0;
}

bool MainWindow::readFirstImage(){
	// Read Image ...
	if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 1)){
		// Custom && OpponentColor
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
	}
	else{
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), cv::IMREAD_GRAYSCALE);//or CV_LOAD_IMAGE_GRAYSCALE
	}

	// Check if the Images are loaded correctly ...
	if (firstImg.empty())
	{
		return false;
	}
	if ((firstImg.cols < 100) && (firstImg.rows < 100))
	{
		cv::resize(firstImg, firstImg, cv::Size(), 200 / firstImg.rows, 200 / firstImg.cols);
	}

	for (int i = 0; i < 5; i++)ui->viewTabs->setCurrentIndex(i); // just to center contents
	displayImage(firstImg, 1);
	return true;
}

bool MainWindow::readSecondImage(){
	// Read Image ...
	if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 1)){
		// Custom && OpponentColor
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
	}
	else{
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), cv::IMREAD_GRAYSCALE); //or CV_LOAD_IMAGE_GRAYSCALE
	}

	// Check if the Images are loaded correctly ...
	if (secondImg.empty())
	{
		return false;
	}

	if ((secondImg.cols < 100) && (secondImg.rows < 100))
	{
		cv::resize(secondImg, secondImg, cv::Size(), 200 / secondImg.rows, 200 / secondImg.cols);
	}
	displayImage(secondImg, 2);

	return true;
}

bool MainWindow::readInputFile(){
	
	inputFile = ui->inputPath->text().replace("/", "\\");

	// Check if the Path is loaded correctly ...
	return (!inputFile.isEmpty());
}

bool MainWindow::readSetOfImages(){
	setImgs.clear();
	int savedIndex = ui->bddImageNames->currentIndex();
	ui->bddImageNames->clear();
	// Read Data Set of Images ...
	std::wstring wdatapath = ui->secondImgText->text().toStdWString();
	
	tinydir_dir dir;
	tinydir_open(&dir, (const TCHAR*)wdatapath.c_str());

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);

		if (!file.is_dir)
		{   // this is a file
			// get its name
			std::wstring wfilename = file.name;
			std::string datapath(wdatapath.begin(), wdatapath.end());
			std::string filename(wfilename.begin(), wfilename.end());
			std::string datapath_filename = datapath + "/" + filename;

			cv::Mat img;
			if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 4)){
				// Custom && OpponentColor
				img = cv::imread(datapath_filename, CV_LOAD_IMAGE_COLOR);
			}
			else{
				img = cv::imread(datapath_filename, cv::IMREAD_GRAYSCALE); //or CV_LOAD_IMAGE_GRAYSCALE
			}

			// Check if the Images are loaded correctly ...
			if (!img.empty())
			{
				if ((img.cols < 100) && (img.rows < 100))
				{
					cv::resize(img, img, cv::Size(), 200 / img.rows, 200 / img.cols);
				}
				// store the name and the image
				setImgs.push_back(std::make_pair(filename, img));

				// show names in the combobox
				ui->bddImageNames->addItem(QString::fromStdString(filename));
			}
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);

	if (setImgs.size() == 0) {
		return false;
	}
	else if (setImgs.size() == 1){
		// one to one image
		secondImg = setImgs[setImgs.size() - 1].second;
		oneToN = false;
	}
	ui->bddImageNames->setCurrentIndex(savedIndex);
	// display the last image
	//displayImage(setImgs[setImgs.size() - 1].second, 2);
	return true;
}

bool MainWindow::createTestFolder(){
	// create a new folder test
	if (CreateDirectory(L"Tests", NULL) || ERROR_ALREADY_EXISTS == GetLastError()){
		std::ifstream infile;
		FILE *file;
		/*first check if the file exists...*/
		infile.open("Tests/next.txt");
		/*...then open it in the appropriate way*/
		if (infile.is_open()) {
			// existing file
			while (infile.eof() == false)infile >> cpt;
			infile.close();
			file = fopen("Tests/next.txt", "r+b");
		}
		else{
			// new file
			file = fopen("Tests/next.txt", "w+b");
			cpt = 0;
			wchar_t* fileLPCWSTR = L"Tests/next.txt";
			int attr = GetFileAttributes(fileLPCWSTR);
			if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
				SetFileAttributes(fileLPCWSTR, attr | FILE_ATTRIBUTE_HIDDEN);
			}
		}

		if (file != NULL)
		{
			fprintf(file, std::to_string(cpt + 1).c_str());
			fclose(file);
		}

		wchar_t _directoryPath[256];
		wsprintfW(_directoryPath, L"Tests/%d", cpt);
		if (CreateDirectory(_directoryPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{
			// created with succes
			// Rest parameters :
			resetParams();
			directoryPath = "Tests/" + std::to_string(cpt) + "/";
			if (oneToN && setImgs.size() > 1){
				// Create a sub directory for all outmput matches
				wchar_t _outputsPath[256];
				wsprintfW(_outputsPath, L"Tests/%d/all matches", cpt);
				if (CreateDirectory(_outputsPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
				{
					// success
				}
				else {
					showError("Creating directory", "Cannot create a directory to store matches", "Make sure that " + directoryPath + " existes");
				}
			}
			return true;
		}
		else
		{
			// Failed to create directory.
			ui->logPlainText->appendHtml("<b style='color:red'>Failed to create directory for the current test!</b>");
			return false;
		}
	}
	else
	{
		// Failed to create the root.
		ui->logPlainText->appendHtml("<b style='color:red'>Failed to create directory for all tests!</b>");
		return false;
	}
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

QString MainWindow::getFlannBasedNameParamsType(){
	if (ui->matcherFlannBasedLinearIndexParams->isChecked()) return ui->matcherFlannBasedLinearIndexParams->text();
	else if (ui->matcherFlannBasedKDTreeIndexParams->isChecked()) return ui->matcherFlannBasedKDTreeIndexParams->text();
	else if (ui->matcherFlannBasedKMeansIndexParams->isChecked()) return ui->matcherFlannBasedKMeansIndexParams->text();
	else if (ui->matcherFlannBasedCompositeIndexParams->isChecked()) return ui->matcherFlannBasedCompositeIndexParams->text();
	else if (ui->matcherFlannBasedLshIndexParams->isChecked()) return ui->matcherFlannBasedLshIndexParams->text();
	else if (ui->matcherFlannBasedAutotunedIndexParams->isChecked()) return ui->matcherFlannBasedAutotunedIndexParams->text();
	else return ui->matcherFlannBasedKDTreeIndexParams->text();
}

void MainWindow::writeToFile(std::string fileName, cv::Algorithm * algoToWrite){
// Open a file and write parameters of an algorithm
	// Open the file in WRITE mode
	cv::FileStorage fs("params/" + fileName + "_params.yaml", cv::FileStorage::WRITE);
	// Write the parameters
	algoToWrite->write(fs);
	// fs in WRITE mode automatically released
}

void MainWindow::wheelEvent(QWheelEvent *event){

	// Scale the view / do the zoom
	double scaleFactor = 1.15;
	if (event->delta() < 0) scaleFactor = 1.0 / scaleFactor; // Zoom out
	
	// Zoom in or Zoom out
	ui->viewImage1->scale(scaleFactor, scaleFactor);
	ui->viewImage2->scale(scaleFactor, scaleFactor);
	ui->viewKeyPoints1->scale(scaleFactor, scaleFactor);
	ui->viewKeyPoints2->scale(scaleFactor, scaleFactor);
	ui->viewMatches->scale(scaleFactor, scaleFactor);
}

void MainWindow::resetParams()
{
	sumDistances = 0; score = 0;
	bestScoreIndex = -1;
	try{
		firstImgKeypoints.clear(); secondImgKeypoints.clear(); setImgsKeypoints.clear();
		firstImgDescriptor.release(); secondImgDescriptor.release(); setImgsDescriptors.clear();
		firstEnhancedImage.release(); secondEnhancedImage.release(); setEnhancedImages.clear();
		firstSegmentedImage.release(); secondSegmentedImage.release(); setSegmentedImages.clear();
		directMatches.clear(); inverseMatches.clear(); twoMatches.clear(); goodMatches.clear(); badMatches.clear();
		directMatchesSet.clear(); inverseMatchesSet.clear(); twoMatchesSet.clear(); goodMatchesSet.clear(); badMatchesSet.clear();
		sumDistancesSet.clear(); scoreSet.clear();
		matchingMask.release(); matchingMasks.clear();
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
void MainWindow::harrisCorners(cv::Mat thinnedImage, std::vector<cv::KeyPoint> &keypoints, float threshold){
// detect the strong minutiae using Haris corner detection (retrieved from 'OpenCV 3 Blueprints' book)
	cv::Mat harris_corners, harris_normalised;
	harris_corners = cv::Mat::zeros(thinnedImage.size(), CV_32FC1);
	cornerHarris(thinnedImage, harris_corners, 2, 3, 0.04, cv::BORDER_DEFAULT);
	// get a map with all the available corner responses rescaled to the range of[0 255] and stored as float values
	normalize(harris_corners, harris_normalised, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

	// Select the strongest corners that we want
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
				keypoints.push_back(cv::KeyPoint(x, y, 1));
			}
		}
	}
}

double MainWindow::kMeans(std::vector<cv::Mat> features_vector, int k){
// K : The number of clusters to split the samples in rawFeatureData (retrieved from 'OpenCV 3 Blueprints' book)
	int nbRows = firstImgDescriptor.rows;
	if (oneToN){
		for (cv::Mat descriptor : setImgsDescriptors)
		{
			nbRows += descriptor.rows;
		}
	}
	else nbRows += secondImgDescriptor.rows;
	cv::Mat rawFeatureData = cv::Mat::zeros(nbRows, firstImgDescriptor.cols, CV_32FC1);
	// We need to copy the data from the vector of key points features_vector to imageFeatureData:
	int cur_idx = 0;
	for (int i = 0; i < features_vector.size(); i++){
		features_vector[i].copyTo(rawFeatureData.rowRange(cur_idx, cur_idx + features_vector[i].rows));
		cur_idx += features_vector[i].rows;
	}
	double result; //This is the sum of the squared distance between each sample to the corresponding centroid;
	result = cv::kmeans(rawFeatureData, k, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 1.0),
		3, cv::KMEANS_PP_CENTERS, centers);
	return result;
}
// <--------------
void MainWindow::clustering(){
	// use kmeans algorithme to cluster descriptors
	clusteringTime = (double)cv::getTickCount();
	std::vector<cv::Mat> matrix;
	if (oneToN) {
		matrix = { firstImgDescriptor };
		matrix.insert(matrix.end(), setImgsDescriptors.begin(), setImgsDescriptors.end());
	}
	else matrix = { firstImgDescriptor, secondImgDescriptor };
	try{
		double compactness = kMeans(matrix, 40);
	}
	catch (...){
		ui->logPlainText->appendHtml("<i style='color:red'>The number of clusters is too high, try another!</i>");
		return;
	}
	// Write Kmeans parameters
	cv::FileStorage fs("params/kmeans_params.yaml", cv::FileStorage::WRITE);
	fs << "Labels" << labels;
	fs << "Centers" << centers;
	fs.release();
	clusteringTime = ((double)cv::getTickCount() - clusteringTime) / cv::getTickFrequency();
	ui->logPlainText->appendPlainText("clustering time: " + QString::number(clusteringTime) + " (s)");
}

QImage MainWindow::matToQImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS=1
	if (mat.type() == CV_8UC1)
	{
		// Set the color table (used to translate colour indexes to qRgb values)
		QVector<QRgb> colorTable;
		for (int i = 0; i<256; i++)
			colorTable.push_back(qRgb(i, i, i));
		// Copy input Mat
		const uchar *qImageBuffer = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
		img.setColorTable(colorTable);
		return img;
	}
	// 8-bits unsigned, NO. OF CHANNELS=3
	if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *qImageBuffer = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return img.rgbSwapped();
	}
	else
	{
		return QImage();
	}
}

void MainWindow::displayImage(cv::Mat imageMat, int first_second)
{
	QGraphicsScene *featureScene = new QGraphicsScene();
	QGraphicsView *myUiScene = (first_second == 1) ? ui->viewImage1 : ui->viewImage2;

	QImage img = matToQImage(imageMat);
	featureScene->addPixmap(QPixmap::fromImage(img));

	myUiScene->setScene(featureScene);
	myUiScene->fitInView(featureScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
	//myUiScene->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
}

void MainWindow::displayFeature(cv::Mat featureMat, int first_second)
{
	QGraphicsScene *featureScene = new QGraphicsScene();
	QImage featureImg((const uchar *)featureMat.data, featureMat.cols, featureMat.rows, featureMat.step, QImage::Format_RGB888);
	featureScene->addPixmap(QPixmap::fromImage(featureImg));

	QGraphicsView *myUiScene;
	if (first_second == 1) myUiScene = ui->viewKeyPoints1;
	else if (first_second == 2) myUiScene = ui->viewKeyPoints2;
	else myUiScene = ui->viewMatches;
	myUiScene->setScene(featureScene);
	myUiScene->fitInView(featureScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
}

void MainWindow::displayMatches(int imgIndex){
// Displays matches in the table and point to imgIndex if 1 to N
	initializeTable();

	disconnect(ui->viewMatchesImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
	disconnect(ui->viewTableImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
	ui->viewMatchesImageNameText->setCurrentIndex(imgIndex);
	ui->viewTableImageNameText->setCurrentIndex(imgIndex);
	connect(ui->viewMatchesImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
	connect(ui->viewTableImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));

	if (oneToN) {
		goodMatches = goodMatchesSet[imgIndex];
		badMatches = badMatchesSet[imgIndex];
		sumDistances = sumDistancesSet[imgIndex];
		score = scoreSet[imgIndex];
	}
	int i = 0;
	for (cv::DMatch match : goodMatches){
		// Add information to the table
		QStandardItem *x1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.x));
		model->setItem(i, 0, x1);
		QStandardItem *y1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.y));
		model->setItem(i, 1, y1);
		if (oneToN){
			QStandardItem *x2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.x));
			model->setItem(i, 2, x2);
			QStandardItem *y2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.y));
			model->setItem(i, 3, y2);
		}
		else{
			QStandardItem *x2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.x));
			model->setItem(i, 2, x2);
			QStandardItem *y2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.y));
			model->setItem(i, 3, y2);
		}
		QStandardItem *dist = new QStandardItem(QString::number(match.distance));
		model->setItem(i, 4, dist);

		QStandardItem *accepted = new QStandardItem("Accepted");
		accepted->setData(QColor(Qt::green), Qt::BackgroundRole);
		model->setItem(i, 5, accepted);
		i++;
	}
	for (cv::DMatch match : badMatches){
		// Add information to the table
		QStandardItem *x1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.x));
		model->setItem(i, 0, x1);
		QStandardItem *y1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.y));
		model->setItem(i, 1, y1);
		if (oneToN){
			QStandardItem *x2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.x));
			model->setItem(i, 2, x2);
			QStandardItem *y2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.y));
			model->setItem(i, 3, y2);
		}
		else{
			QStandardItem *x2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.x));
			model->setItem(i, 2, x2);
			QStandardItem *y2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.y));
			model->setItem(i, 3, y2);
		}
		QStandardItem *dist = new QStandardItem(QString::number(match.distance));
		model->setItem(i, 4, dist);

		QStandardItem *rejected = new QStandardItem("Rejected");
		rejected->setData(QColor(Qt::red), Qt::BackgroundRole);
		model->setItem(i, 5, rejected);
		i++;
	}
	ui->viewTable->setModel(model);

	// Add the image to the viewer
	QGraphicsScene *matchingScene = new QGraphicsScene();

	cv::Mat drawImg;
	if (oneToN) drawImg = cv::imread(directoryPath + "/all matches/" + std::to_string(imgIndex) + ".jpg");
	else drawImg = cv::imread(directoryPath + "output.jpg");
	QImage img = matToQImage(drawImg);
	matchingScene->addPixmap(QPixmap::fromImage(img));

	ui->viewMatches->setScene(matchingScene);
	ui->viewMatches->fitInView(matchingScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
	//ui->viewMatches->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

	// Add infos to Matcher viewer
	float goodProbability = static_cast<float>(goodMatches.size()) / static_cast<float>(goodMatches.size()+badMatches.size()) * 100;
	float badProbability = static_cast<float>(badMatches.size()) / static_cast<float>(goodMatches.size() + badMatches.size()) * 100;
	float average = sumDistances / static_cast<float>(goodMatches.size());

	ui->viewMatchesGoodMatchesText->setText("<b style='color:green'>" + QString::number(goodMatches.size()) + "</b>/" + QString::number(goodMatches.size() + badMatches.size()) + " = <b style = 'color:green'>" + QString::number(goodProbability) + "</b>%");
	ui->viewMatchesBadMatchesText->setText("<b style='color:red'>" + QString::number(badMatches.size()) + "</b>/" + QString::number(goodMatches.size() + badMatches.size()) + " = <b style = 'color:red'>" + QString::number(badProbability) + "</b>%");
	ui->viewMatchesAverageMatchesText->setText(QString::number(average));
	ui->viewMatchesScoreMatchesText->setText("<b>"+QString::number(score)+"</b>");
}

template <typename T>
void MainWindow::writeKeyPoints(cv::Mat img, std::vector<T> keyPoints, int first_second, std::string fileName, int squareSize){
	//Visualisation
	cv::Mat outImg = img.clone();
	cvtColor(img, outImg, CV_GRAY2RGB);
	for (cv::KeyPoint &keyPoint : keyPoints){
		//add a transparent square at each minutiae-location
		cv::Mat color;
		cv::Mat roi;
		try{
			roi = outImg(cv::Rect(keyPoint.pt.x - squareSize / 2, keyPoint.pt.y - squareSize / 2, squareSize, squareSize));
		}
		catch (cv::Exception e){
			goto EndDisplayingKeyPoint_LABEL;
		}
		double alpha = 0.3;
		if (typeid(keyPoints) == typeid(std::vector<Minutiae>)){
			// if minutiaes then distinguish between ridgeending and bifurcation
			if (static_cast<Minutiae&>(keyPoint).getType() == Minutiae::Type::RIDGEENDING){
				color = cv::Mat(roi.size(), CV_8UC3, cv::Scalar(255, 0, 0));    //blue square for ridgeending
			}
			else if (static_cast<Minutiae&>(keyPoint).getType() == Minutiae::Type::BIFURCATION){
				color = cv::Mat(roi.size(), CV_8UC3, cv::Scalar(0, 0, 255));    //red square for bifurcation
			}
		}
		else {
			//if simple keyPoints then use one color
			color = cv::Mat(roi.size(), CV_8UC3, cv::Scalar(0, 128, 0));    //green square for simple keyPoint
		}
		addWeighted(color, alpha, roi, 1.0 - alpha, 0.0, roi);
		EndDisplayingKeyPoint_LABEL:
		;
	}
	displayFeature(outImg, first_second);  // Show our image inside the viewer.
	if (fileName != "")cv::imwrite(directoryPath + fileName + ".bmp", outImg);
}

void MainWindow::writeMatches(int imgIndex){
// Write and Shows matches
	cv::Mat drawImg;
	disconnect(ui->viewMatchesImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
	disconnect(ui->viewTableImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));

	ui->viewMatchesImageNameText->clear();
	ui->viewTableImageNameText->clear();
	if (oneToN){
		// 1 to N and There is a best match  
		for (size_t i = 0; i < setImgs.size(); i++)
		{
			ui->viewMatchesImageNameText->addItem(QString::fromStdString(setImgs[i].first));
			ui->viewTableImageNameText->addItem(QString::fromStdString(setImgs[i].first));
			// Draw and Store bad matches
			cv::drawMatches(firstImg, firstImgKeypoints, setImgs[i].second, setImgsKeypoints[i],
				badMatchesSet[i], drawImg, cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 255));

			// Draw and Store best matches
			cv::drawMatches(firstImg, firstImgKeypoints, setImgs[i].second, setImgsKeypoints[i],
				goodMatchesSet[i], drawImg, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), std::vector<char>(), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

			std::string filename = directoryPath + "/all matches/" + std::to_string(i) + ".jpg";
			if (!cv::imwrite(filename, drawImg))
				ui->logPlainText->appendHtml("<b style='color:orange'>Image " + QString::fromStdString(filename) + "  can not be saved (may be because directory " + QString::fromStdString(directoryPath) + "  does not exist) !</b>");
				
			if (i == bestScoreIndex){
				if (!cv::imwrite(directoryPath + "output.jpg", drawImg))
					ui->logPlainText->appendHtml("<b style='color:orange'>Image " + QString::fromStdString(directoryPath + "output.jpg") + "  can not be saved (may be because directory " + QString::fromStdString(directoryPath) + "  does not exist) !</b>");

				// Add the image to the viewer
				QGraphicsScene *matchingScene = new QGraphicsScene();

				QImage img = matToQImage(drawImg);
				matchingScene->addPixmap(QPixmap::fromImage(img));

				ui->viewMatches->setScene(matchingScene);
				ui->viewMatches->fitInView(matchingScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
				//ui->viewMatches->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
			}
		}
	}
	else{
		// 1 to 1
		// Draw and Store bad matches
		cv::drawMatches(firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
			badMatches, drawImg, cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 255));
		// Draw and Store best matches
		cv::drawMatches(firstImg, firstImgKeypoints, secondImg, secondImgKeypoints,
			goodMatches, drawImg, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), std::vector<char>(), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);
		if (!cv::imwrite(directoryPath + "output.jpg", drawImg))
			ui->logPlainText->appendHtml("<b style='color:orange'>Matches Image can not be saved (may be because directory " + QString::fromStdString(directoryPath) + "  does not exist) !</b>");
		
		// Add the image to the viewer
		QGraphicsScene *matchingScene = new QGraphicsScene();

		QImage img = matToQImage(drawImg);
		matchingScene->addPixmap(QPixmap::fromImage(img));

		ui->viewMatches->setScene(matchingScene);
		ui->viewMatches->fitInView(matchingScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
		//ui->viewMatches->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
	}
	// Choose matches to display
	ui->viewMatchesImageNameText->setItemData(imgIndex, QBrush(Qt::green), Qt::BackgroundRole);
	ui->viewTableImageNameText->setItemData(imgIndex, QBrush(Qt::green), Qt::BackgroundRole);
	ui->viewMatchesImageNameText->setCurrentIndex(imgIndex);
	ui->viewTableImageNameText->setCurrentIndex(imgIndex);

	connect(ui->viewMatchesImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
	connect(ui->viewTableImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
}

bool MainWindow::takeTest() {
	// Read Images ...
	if (!readFirstImage()){
		ui->logPlainText->appendHtml("<b style='color:red'>Error while trying to read the 1st input file!</b>");
		return false;
	}
	oneToN = ui->oneToN->isChecked();

	if (oneToN) {
		readSetOfImages();
		if (setImgs.size() == 0){
			showError("Read Images", "There is no image in the folder: " + ui->secondImgText->text().toStdString(), "Make sure that the folder '<i>" + ui->secondImgText->text().toStdString() + "'</i>  contains one or more images with correct extension!");
			return false;
		}
	}
	else {
		if (!readSecondImage()) {
			ui->logPlainText->appendHtml("<b style='color:red'>Error while trying to read the 2nd input file!</b>");
			return false;
		}
	}

	// Create a test folder ...
	if (!createTestFolder()) return false;

	if (oneToN) matchingMasks = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
	else matchingMask = cv::Mat();

	// Launch the algorithm
	switch (ui->allMethodsTabs->currentIndex())
	{
	case 0:
		// default
		runDefault();
		break;
	case 1:
	default:
		// custom
		runCustom();
		break;
	}
	if (showDecision()) return true;
	else return false;
}

void MainWindow::customisingBinarization(int segmentationIndex){
	// Binarization
	if (segmentationIndex != 0 && segmentationIndex != 3){
		// First load the image to process in grayscale and transform it to a binary image using thresholding:
		// Binarization
		// The Otsu thresholding will automatically choose the best generic threshold for the image to obtain a good contrast between foreground and background information.
		// If you have only a single capturing device, then playing around with a fixed threshold value could result in a better image for that specific setup

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!		
		localThreshold::binarisation(firstImg, 41, 56);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				try{ localThreshold::binarisation(setImgs[i].second, 41, 56); }
				catch (cv::Exception e){
					showError("Binarization", "Error in the image '" + setImgs[i].first + "'", e.msg);
				}
			}
		}
		else localThreshold::binarisation(secondImg, 41, 56);

		double threshold = ui->segmentationThresholdText->text().toFloat();
		cv::threshold(firstImg, firstImg, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		if (oneToN)
			for (int i = 0; i < setImgs.size(); i++){
				try{ cv::threshold(setImgs[i].second, setImgs[i].second, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU); }
				catch (cv::Exception e){
					showError("Thresholding", "Error in the image '" + setImgs[i].first + "'", e.msg);
				}
			}
		else cv::threshold(secondImg, secondImg, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		
		//ideka::binOptimisation(firstImg);
		//ideka::binOptimisation(secondImg);
		cv::imwrite(directoryPath + "f-1_Binarization.bmp", firstImg);
		if (oneToN) cv::imwrite(directoryPath + "l-1_Binarization.bmp", setImgs[setImgs.size() - 1].second);
		else cv::imwrite(directoryPath + "s-1_Binarization.bmp", secondImg);
	}
}

void MainWindow::customisingSegmentor(int segmentationIndex){
	// Creating Segmentor...
	switch (segmentationIndex)
	{
	case 1:
		// Skeletonization of Morphological Skeleton
		// This will create more unique and stronger interest points
		firstImg = skeletonization(firstImg);
		cv::imwrite(directoryPath + "f-2_Morphological Skeleton.bmp", firstImg);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size();i++){
				setImgs[i].second = skeletonization(setImgs[i].second);
			}
			cv::imwrite(directoryPath + "l-2_Morphological Skeleton.bmp", setImgs[setImgs.size() - 1].second);
		}
		else {
			secondImg = skeletonization(secondImg);
			cv::imwrite(directoryPath + "s-2_Morphological Skeleton.bmp", secondImg);
		}
		break;
	case 2:
		// Thinning of Zhang-Suen
		//This is the same Thinning Algorithme used by BluePrints
		ZhangSuen::thinning(firstImg);
		cv::imwrite(directoryPath + "f-2_Zhang-Suen Thinning.bmp", firstImg);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				ZhangSuen::thinning(setImgs[i].second);
			}
			cv::imwrite(directoryPath + "l-2_Zhang-Suen Thinning.bmp", setImgs[setImgs.size() - 1].second);
		}
		else {
			ZhangSuen::thinning(secondImg);
			cv::imwrite(directoryPath + "s-2_Zhang-Suen Thinning.bmp", secondImg);
		}
	break;
	case 3:{
		// Thinning of Lin-Hong implemented by Mrs. Faiçal
		firstImg = Image_processing::thinning(firstImg, firstEnhancedImage, firstSegmentedImage);
		firstImg.convertTo(firstImg, CV_8UC3, 255);
		cv::imwrite(directoryPath + "f-2_Lin-Hong Thinning.bmp", firstImg);
		if (oneToN)
		{
			setEnhancedImages = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
			setSegmentedImages = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
			for (int i = 0; i < setImgs.size(); i++){
				setImgs[i].second = Image_processing::thinning(setImgs[i].second, setEnhancedImages[i], setSegmentedImages[i]);
				setImgs[i].second.convertTo(setImgs[i].second, CV_8UC3, 255);
			}
			cv::imwrite(directoryPath + "l-2_Lin-Hong Thinning.bmp", setImgs[setImgs.size() - 1].second);
		}
		else {
			secondImg = Image_processing::thinning(secondImg, secondEnhancedImage, secondSegmentedImage);
			secondImg.convertTo(secondImg, CV_8UC3, 255);
			cv::imwrite(directoryPath + "s-2_Lin-Hong Thinning.bmp", secondImg);
		}		
		break;
	}
	case 4:
		// Thinning of Guo-Hall
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  Exception !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		GuoHall::thinning(firstImg);
		cv::imwrite(directoryPath + "f-2_Guo-Hall Thinning.bmp", firstImg);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				GuoHall::thinning(setImgs[i].second);
			}
			cv::imwrite(directoryPath + "l-2_Guo-Hall Thinning.bmp", setImgs[setImgs.size() - 1].second);
		}
		else {
			GuoHall::thinning(secondImg);
			cv::imwrite(directoryPath + "s-2_Guo-Hall Thinning.bmp", secondImg);
		}
		break;

		//....
	}
}

void MainWindow::customisingDetector(int detectorIndex, std::string detectorName){
	// Creating Detector...	
	setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>());
	switch (detectorIndex){
	case 0:{
		// Minutiae-detection using Crossing Number By Dr. Faiçal
		std::vector<Minutiae> firstMinutiae, secondMinutiae;
		std::vector<std::vector<Minutiae>> setMinutiaes;
		detectionTime = (double)cv::getTickCount();
		// change this to firstImage and originalInput !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		firstMinutiae = Image_processing::extracting(firstImg, firstEnhancedImage, firstSegmentedImage, firstImg);
		if (oneToN)
		{
			setMinutiaes = std::vector<std::vector<Minutiae>>(setImgs.size(), std::vector<Minutiae>());
			for (int i = 0; i < setImgs.size(); i++){
				setMinutiaes[i] = Image_processing::extracting(setImgs[i].second, setEnhancedImages[i], setSegmentedImages[i], setImgs[i].second);
			}
		}
		else secondMinutiae = Image_processing::extracting(secondImg, secondEnhancedImage, secondSegmentedImage, secondImg);
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();

		writeKeyPoints(firstImg, firstMinutiae, 1, "keypoints1");
		if (!oneToN) writeKeyPoints(secondImg, secondMinutiae, 2, "keypoints2");

		// images must be segmented if not Minutiae will be empty
		try{
			// Adapt Minutiaes to KeyPoints (this will affect magnitudes and angles to minutiaes)
			MinutiaeToKeyPointAdapter adapter;
			// also we must add the Adapting time to detection time
			detectionTime += adapter.adapt(firstMinutiae);
			if (oneToN)
				for (int i = 0; i < setImgs.size(); i++) detectionTime += adapter.adapt(setMinutiaes[i]);
			else detectionTime += adapter.adapt(secondMinutiae);
			for (Minutiae minutiae : firstMinutiae) firstImgKeypoints.push_back(minutiae);
			if (oneToN)
			{
				setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>());
				for (int i = 0; i < setImgs.size(); i++){
					for (Minutiae minutiae : setMinutiaes[i]) setImgsKeypoints[i].push_back(minutiae);
					// use masks to matche minutiae of the same type
					matchingMasks[i] = maskMatchesByMinutiaeNature(firstMinutiae, setMinutiaes[i]);
					}
			}
			else {
				for (Minutiae minutiae : secondMinutiae) secondImgKeypoints.push_back(minutiae);
				// use masks to matche minutiae of the same type
				matchingMask = maskMatchesByMinutiaeNature(firstMinutiae, secondMinutiae);
			}
		}
		catch (...){
			ui->logPlainText->appendHtml("<i style='color:red'>Before detecting Minutiae you must select a segmentation method !</i>");
			return;
		}

	}
	break;
	case 1:{
		// Minutiae-detection using Crossing Number
		// http://www.codelooker.com/id/217/1100103.html
		std::vector<Minutiae> firstMinutiae, secondMinutiae;
		std::vector<std::vector<Minutiae>> setMinutiaes;

		detectionTime = (double)cv::getTickCount();
		firstMinutiae = crossingNumber::getMinutiae(firstImg, ui->detectorCrossingNumberBorderText->text().toInt());
		//Minutiae-filtering
		// slow with the second segmentation
		Filter::filterMinutiae(firstMinutiae);
		if (oneToN)
		{
			setMinutiaes = std::vector<std::vector<Minutiae>>(setImgs.size(), std::vector<Minutiae>());
			for (int i = 0; i < setImgs.size(); i++){
				setMinutiaes[i] = crossingNumber::getMinutiae(setImgs[i].second, ui->detectorCrossingNumberBorderText->text().toInt());
				Filter::filterMinutiae(setMinutiaes[i]);
			}
		}
		else {
			secondMinutiae = crossingNumber::getMinutiae(secondImg, ui->detectorCrossingNumberBorderText->text().toInt());
			Filter::filterMinutiae(secondMinutiae);
		}
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();

		writeKeyPoints(firstImg, firstMinutiae, 1, "keypoints1");
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				writeKeyPoints(setImgs[i].second, setMinutiaes[i], 2, "keypoints2");
			}
		}
		else writeKeyPoints(secondImg, secondMinutiae, 2, "keypoints2");

		// images must be segmented if not Minutiae will be empty
		try{
			// Adapt Minutiaes to KeyPoints (this will affect magnitudes and angles to minutiaes)
			MinutiaeToKeyPointAdapter adapter;
			// also we must add the Adapting time to detection time
			detectionTime += adapter.adapt(firstMinutiae);
			if (oneToN)
				for (int i = 0; i < setImgs.size(); i++) detectionTime += adapter.adapt(setMinutiaes[i]);
			else detectionTime += adapter.adapt(secondMinutiae);
			for (Minutiae minutiae : firstMinutiae) firstImgKeypoints.push_back(minutiae);
			if (oneToN)
			{
				setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>());
				for (int i = 0; i < setImgs.size(); i++){
					for (Minutiae minutiae : setMinutiaes[i]) setImgsKeypoints[i].push_back(minutiae);
					// use masks to matche minutiae of the same type
					matchingMasks[i] = maskMatchesByMinutiaeNature(firstMinutiae, setMinutiaes[i]);
				}
			}
			else {
				for (Minutiae minutiae : secondMinutiae) secondImgKeypoints.push_back(minutiae);
				// use masks to matche minutiae of the same type
				matchingMask = maskMatchesByMinutiaeNature(firstMinutiae, secondMinutiae);
			}
		}
		catch (...){
			ui->logPlainText->appendHtml("<i style='color:red'>Before detecting Minutiae you must select a segmentation method !</i>");
			return;
		}

	}
	break;
	case 2:{
		// Harris-Corners
		detectionTime = (double)cv::getTickCount();
		harrisCorners(firstImg, firstImgKeypoints, ui->detectorHarrisThresholdText->text().toFloat());
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				harrisCorners(setImgs[i].second, setImgsKeypoints[i], ui->detectorHarrisThresholdText->text().toFloat());
			}
		}
		else harrisCorners(secondImg, secondImgKeypoints, ui->detectorHarrisThresholdText->text().toFloat());
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
	}
		   break;
	case 3:{
		// STAR
		ptrDetector = new cv::StarFeatureDetector(ui->detectorStarMaxSizeText->text().toInt(),
			ui->detectorStarResponseThresholdText->text().toInt(),
			ui->detectorStarThresholdProjectedText->text().toInt(),
			ui->detectorStarThresholdBinarizedText->text().toInt(),
			ui->detectorStarSuppressNonmaxSizeText->text().toInt());
	}
		   break;
	case 4:
		// FAST
		if (!ui->detectorFastXCheck->isChecked())
			ptrDetector = new cv::FastFeatureDetector(ui->detectorFastThresholdText->text().toInt(), ui->detectorFastNonmaxSuppressionCheck->isChecked()); 
		else {
			// FASTX
			detectionTime = (double)cv::getTickCount();
			cv::FASTX(firstImg, firstImgKeypoints, ui->detectorFastThresholdText->text().toInt(),
				ui->detectorFastNonmaxSuppressionCheck->isChecked(),
				ui->detectorFastTypeText->currentIndex());
			if (oneToN)
			{
				for (int i = 0; i < setImgs.size(); i++){
					cv::FASTX(setImgs[i].second, setImgsKeypoints[i], ui->detectorFastThresholdText->text().toInt(),
						ui->detectorFastNonmaxSuppressionCheck->isChecked(),
						ui->detectorFastTypeText->currentIndex());
				}
			}
			else cv::FASTX(secondImg, secondImgKeypoints, ui->detectorFastThresholdText->text().toInt(),
				ui->detectorFastNonmaxSuppressionCheck->isChecked(),
				ui->detectorFastTypeText->currentIndex());
			detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
		}
		break;
	case 5:
		// SIFT
		ptrDetector = new cv::SiftFeatureDetector(ui->detectorSiftNfeaturesText->text().toInt(),
			ui->detectorSiftNOctaveLayersText->text().toInt(),
			ui->detectorSiftContrastThresholdText->text().toDouble(),
			ui->detectorSiftEdgeThresholdText->text().toDouble(),
			ui->detectorSiftSigmaText->text().toDouble());
		break;
	case 6:
		//SURF
		// we didn't need the Extended and Upright params because it is related to the SURF descriptor
		ptrDetector = new cv::SurfFeatureDetector(ui->detectorSurfHessianThresholdText->text().toDouble(),
			ui->detectorSurfNOctavesText->text().toInt(),
			ui->detectorSurfNLayersText->text().toInt(),
			true,
			false);
		break;
	case 7:
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

	if (detectorIndex > 2 && (detectorIndex != 4 || !ui->detectorFastXCheck->isChecked())){
		// Write the parameters
		writeToFile("detector_" + detectorName, ptrDetector);
		// fs in WRITE mode automatically released
		try{
			// Detecting Keypoints ...
			detectionTime = (double)cv::getTickCount();
			ptrDetector->detect(firstImg, firstImgKeypoints);
			if (oneToN){
				for (int i = 0; i < setImgs.size(); i++){
					ptrDetector->detect(setImgs[i].second, setImgsKeypoints[i]);
				}
			}else ptrDetector->detect(secondImg, secondImgKeypoints);
			detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
		}
		catch (...){
			ui->logPlainText->appendHtml("<b style='color:red'>Please select the right " + QString::fromStdString(detectorName) + " detector parameters, or use the defaults!.</b>");
			return;
		}
		writeKeyPoints(firstImg, firstImgKeypoints, 1, "keypoints1");
		if (!oneToN) writeKeyPoints(secondImg, secondImgKeypoints, 2, "keypoints2");
	}
	if (noKeyPoints("first", firstImgKeypoints) || (!oneToN && noKeyPoints("second", secondImgKeypoints))) return;
	if (oneToN) {
		ui->logPlainText->textCursor().movePosition(QTextCursor::End);
		prev_cursor_position = ui->logPlainText->textCursor().position();
	}
	ui->logPlainText->appendPlainText("Detection time: " + QString::number(detectionTime) + " (s)");
}

void MainWindow::customisingDescriptor(int descriptorIndex, std::string descriptorName){
	// Creating Descriptor...
	switch (descriptorIndex)
	{
	case 0:
	{
		// FREAK
		// trait the descriptorFreakSelectedPairsIndexes
		std::string descriptorFreakSelectedPairsText = ui->descriptorFreakSelectedPairsText->text().toStdString();
		std::stringstream stringStream(descriptorFreakSelectedPairsText);
		// get ints from a text
		int number;
		std::vector<int> descriptorFreakSelectedPairsIndexes;
		while (stringStream >> number){
			descriptorFreakSelectedPairsIndexes.push_back(number);
		}
		// create FREAK	descriptor 
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

	if (ui->opponentColor->isChecked()){
		//OpponentColor
		ptrDescriptor = new cv::OpponentColorDescriptorExtractor(ptrDescriptor);
		// Write the parameters
		writeToFile("descriptorOppCol_" + descriptorName, ptrDescriptor);
	}

	try{
		descriptionTime = (double)cv::getTickCount();
		ptrDescriptor->compute(firstImg, firstImgKeypoints, firstImgDescriptor);
		if (oneToN)
		{
			setImgsDescriptors = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
			for (int i = 0; i < setImgs.size(); i++){
				ptrDescriptor->compute(setImgs[i].second, setImgsKeypoints[i], setImgsDescriptors[i]);
			}
		}
		else ptrDescriptor->compute(secondImg, secondImgKeypoints, secondImgDescriptor);
		descriptionTime = ((double)cv::getTickCount() - descriptionTime) / cv::getTickFrequency();
		ui->logPlainText->appendPlainText("Description time: " + QString::number(descriptionTime) + " (s)");
	}
	catch (...){
		if (descriptorName == "FREAK")ui->logPlainText->appendHtml("<b style='color:red'>Please select the right pair indexes within the FREAK descriptor, or just leave it!.</b><br>(For more details read Section(4.2) in: <i>A. Alahi, R. Ortiz, and P. Vandergheynst. FREAK: Fast Retina Keypoint. In IEEE Conference on Computer Vision and Pattern Recognition, 2012.</i>)");
		else ui->logPlainText->appendHtml("<b style='color:red'>Please select the right " + QString::fromStdString(descriptorName) + " descriptor parameters, or use the defaults!.</b>");
		return;
	}
}

void MainWindow::customisingMatcher(int matcherIndex, std::string matcherName){
	// creating Matcher...
	switch (matcherIndex)
	{
	case 0:
	{
		// BruteForce
		int norm = getNormByText(ui->matcherBruteForceNormTypeText->currentText().toStdString());
		ptrMatcher = new cv::BFMatcher(norm, 
			ui->matcherBruteForceCrossCheckText->isEnabled() && ui->matcherBruteForceCrossCheckText->isChecked() && (ui->detectorTabs->currentIndex()>1));
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
}

bool MainWindow::matching(){
	// Start matching ...
	try{
		if (ui->matcherInlierNoTest->isChecked() || ui->matcherInlierInversMatches->isChecked()){
			// Only the best direct match
			matchingTime = (double)cv::getTickCount();
			if (oneToN){
				if (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()){
					ptrMatcher->add(setImgsDescriptors);
					//ptrMatcher->train();
					ptrMatcher->match(firstImgDescriptor, directMatches, matchingMasks);
				}
				else {//(ui->matcher1toNtype2->isChecked())
					directMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
					for (int i = 0; i < setImgs.size(); i++){
						ptrMatcher->match(firstImgDescriptor, setImgsDescriptors[i], directMatchesSet[i], matchingMasks[i]);
					}
				}
			}
			else {
				ptrMatcher->match(firstImgDescriptor, secondImgDescriptor, directMatches, matchingMask);
			}

			if (ui->matcherInlierInversMatches->isEnabled() && ui->matcherInlierInversMatches->isChecked()){
				// Also the best match in reverse
				cv::Mat inverseMatchingMask;
				if (oneToN){
					inverseMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
					for (int i = 0; i < setImgs.size(); i++){
						try{
							inverseMatchingMask = matchingMasks[i].t();
						}
						catch (cv::Exception){ inverseMatchingMask = cv::Mat(); }
						ptrMatcher->match(setImgsDescriptors[i], firstImgDescriptor, inverseMatchesSet[i], inverseMatchingMask);
					}
				}
				else {
					try{
						inverseMatchingMask = matchingMask.t();
					}
					catch (cv::Exception){ inverseMatchingMask = cv::Mat(); }
					ptrMatcher->match(secondImgDescriptor, firstImgDescriptor, inverseMatches, inverseMatchingMask);
				}
			}
			matchingTime = ((double)cv::getTickCount() - matchingTime) / cv::getTickFrequency();
		}
		else if (ui->matcherInlierLoweRatio->isEnabled() && ui->matcherInlierLoweRatio->isChecked()){
			// Best two set of matches
			matchingTime = (double)cv::getTickCount();
			if (oneToN){
				twoMatchesSet = std::vector<std::vector<std::vector<cv::DMatch>>>(setImgs.size(), std::vector<std::vector<cv::DMatch>>());
				for (int i = 0; i < setImgs.size(); i++){
					ptrMatcher->knnMatch(firstImgDescriptor, setImgsDescriptors[i], twoMatchesSet[i], 2, matchingMasks[i]);
				}
			}
			else ptrMatcher->knnMatch(firstImgDescriptor, secondImgDescriptor, twoMatches, 2, matchingMask);
			matchingTime = ((double)cv::getTickCount() - matchingTime) / cv::getTickFrequency();
		}
	}
	catch (cv::Exception e){
		// For example Flann-Based doesn't work with Brief desctiptor extractor
		// And also, some descriptors must be used with specific NORM_s
		if (ui->matcherTabs->currentIndex() == 0 && ui->matcherBruteForceCrossCheckText->isEnabled() && ui->matcherBruteForceCrossCheckText->isChecked() && ui->detectorTabs->currentIndex()<2)
			ui->logPlainText->appendHtml("<b style='color:orange'>Set <i>Cross Check</i> in Brute Force as false while matching Minutias!.</b>");
		else ui->logPlainText->appendHtml("<b style='color:red'>Cannot match descriptors because of an incompatible combination!, try another one.</b>");
		return false;
	}
	ui->logPlainText->appendPlainText("Matching time: " + QString::number(matchingTime) + " (s)");
	return true;
}

void MainWindow::outlierElimination(){
	// Eliminate outliers, and calculate the sum of best matches distance
	float limitDistance = ui->matcherInlierLimitDistanceText->text().toFloat();
	float scoreThreshold = ui->decisionStageThresholdText->text().toFloat();
	if (ui->matcherInlierLimitDistance->isChecked() && limitDistance < 0) {
		ui->logPlainText->appendHtml("<b style='color:red'>Invalid Limit Distance: " + QString::number(limitDistance) + ", the default value is maintained!</b>");
		limitDistance = 0.4;
	}
	/*if (use_ransac == false)
	compute_inliers_homography(matches_surf, inliers_surf, H, MAX_H_ERROR);
	else
	compute_inliers_ransac(matches_surf, inliers_surf, MAX_H_ERROR, false);*/
	if (oneToN){
		sumDistancesSet = std::vector<float>(setImgs.size());
		scoreSet = std::vector<float>(setImgs.size());
		float bestScore = 0;

		goodMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
		badMatchesSet  = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());

		if (ui->matcherInlierLoweRatio->isEnabled() && ui->matcherInlierLoweRatio->isChecked()){
			// Lowe's ratio test = 0.7 by default
			float lowesRatio = ui->matcherInlierLoweRatioText->text().toFloat();
			if (lowesRatio <= 0 || 1 <= lowesRatio) {
				ui->logPlainText->appendHtml("<b style='color:red'>Invalid Lowe's Ratio: " + QString::number(lowesRatio) + ", the default value is maintained!</b>");
				lowesRatio = 0.7;
				ui->matcherInlierLoweRatioText->setText("0.7");
			}
			for (int i = 0; i < setImgs.size(); i++){
				sumDistancesSet[i] = testOfLowe(twoMatchesSet[i], lowesRatio, limitDistance, goodMatchesSet[i], badMatchesSet[i]);
				
				if (goodMatchesSet[i].size() > 0){
					float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size()) * 100;
					float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
					scoreSet[i] = 1.0 / average * goodProbability;
					// update the best score index
					if (scoreSet[i] >= bestScore) {
						if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScore].size()){
							bestScoreIndex = i;
							bestScore = scoreSet[i];
						}
					}
				}
				else scoreSet[i] = 0.0;
			}
		}
		else if (ui->matcherInlierInversMatches->isEnabled() && ui->matcherInlierInversMatches->isChecked()){
			// in reverse matching test
			for (int i = 0; i < setImgs.size(); i++){
				sumDistancesSet[i] = testInReverse(directMatchesSet[i], inverseMatchesSet[i], firstImgKeypoints, setImgsKeypoints[i], limitDistance, goodMatchesSet[i], badMatchesSet[i]);
				
				if (goodMatchesSet[i].size() > 0){
					float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size()) * 100;
					float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
					scoreSet[i] = 1.0 / average * goodProbability;
					// update the best score index
					if (scoreSet[i] >= bestScore) {
						if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScore].size()){
							bestScoreIndex = i;
							bestScore = scoreSet[i];
						}
					}
				}
				else scoreSet[i] = 0.0;
			}
		}
		else {
			// No elimination test
			if (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()){// Type1
				for (int i = 0; i < directMatches.size(); i++){
					if (directMatches[i].distance <= limitDistance || !ui->matcherInlierLimitDistance->isChecked()){
						sumDistancesSet[directMatches[i].imgIdx] += directMatches[i].distance;
						goodMatchesSet[directMatches[i].imgIdx].push_back(directMatches[i]);
					}
					else{
						badMatchesSet[directMatches[i].imgIdx].push_back(directMatches[i]);
					}
				}
				for (int i = 0; i < setImgs.size(); i++){
					if (goodMatchesSet[i].size() > 0){
						float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size()) * 100;
						float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
						scoreSet[i] = 1.0 / average * goodProbability;
						// update the best score index
						if (scoreSet[i] >= bestScore) {
								if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScore].size()){
									bestScoreIndex = i;
									bestScore = scoreSet[i];
								}
						}
					}
					else scoreSet[i] = 0.0;
				}
			}
			else {// Type2
				for (int i = 0; i < setImgs.size(); i++){
					for (int j = 0; j < directMatchesSet[i].size(); j++){
						if (directMatchesSet[i][j].distance <= limitDistance || !ui->matcherInlierLimitDistance->isChecked()){
							sumDistancesSet[i] += directMatchesSet[i][j].distance;
							goodMatchesSet[i].push_back(directMatchesSet[i][j]);
						}
						else {
							badMatchesSet[i].push_back(directMatchesSet[i][j]);
						}
					}
					if (goodMatchesSet[i].size() > 0){
						float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size()) * 100;
						float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
						scoreSet[i] = 1.0 / average * goodProbability;
						// update the best score index
						if (scoreSet[i] >= bestScore) {
								if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScore].size()){
									bestScoreIndex = i;
									bestScore = scoreSet[i];
								}
						}
					}
					else scoreSet[i] = 0.0;
				}
			}
		}
	}
	else {// 1 to 1
		if (ui->matcherInlierLoweRatio->isChecked()){
			// Lowe's ratio test = 0.7 by default
			float lowesRatio = ui->matcherInlierLoweRatioText->text().toFloat();
			if (lowesRatio <= 0 || 1 <= lowesRatio) {
				ui->logPlainText->appendHtml("<b style='color:red'>Invalid Lowe's Ratio: " + QString::number(lowesRatio) + ", the default value is maintained!</b>");
				lowesRatio = 0.7;
			}
			sumDistances = testOfLowe(twoMatches, lowesRatio, limitDistance, goodMatches, badMatches);
		}
		else if (ui->matcherInlierInversMatches->isChecked()){
			// in reverse matching test
			sumDistances = testInReverse(directMatches, inverseMatches, firstImgKeypoints, secondImgKeypoints, limitDistance, goodMatches, badMatches);
		}
		else {
			// No elimination test
			// 1 to 1
			for (int i = 0; i < directMatches.size(); i++){
				if (directMatches[i].distance <= limitDistance || !ui->matcherInlierLimitDistance->isChecked()){
					sumDistances += directMatches[i].distance;
					goodMatches.push_back(directMatches[i]);
				}
				else{
					badMatches.push_back(directMatches[i]);
				}
			}
		}
		if (goodMatches.size() > 0){
			float average = sumDistances / static_cast<float>(goodMatches.size());
			float goodProbability = static_cast<float>(goodMatches.size()) / static_cast<float>(goodMatches.size() + badMatches.size()) * 100;
			score = 1.0 / average * goodProbability;
		}
		else score = 0.0;
	}
}

void MainWindow::maskMatchesByTrainImgIdx(const std::vector<cv::DMatch> matches, int trainImgIdx, std::vector<char>& mask)
{
	mask.resize(matches.size());
	fill(mask.begin(), mask.end(), 0);
	for (size_t i = 0; i < matches.size(); i++)
	{
		if (matches[i].imgIdx == trainImgIdx)
			mask[i] = 1;
	}
}

int MainWindow::fileCounter(std::string dir, std::string prefix, std::string suffix, std::string extension){
	int returnedCount = 0;
	int possibleMax = 5000000; //some number you can expect.

	for (int istarter = 0; istarter < possibleMax; istarter++){
		std::string fileName = "";
		fileName.append(dir);
		fileName.append(prefix);
		fileName.append(std::to_string(istarter));
		fileName.append(suffix);
		fileName.append(extension);
		bool status = fileExistenceCheck(fileName);
		if (!status) break;
		returnedCount = istarter + 1;
	}

	return returnedCount;
}

bool MainWindow::fileExistenceCheck(const std::string& name){
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void MainWindow::showError(std::string title, std::string text, std::string e_msg){
// Display a message box
	QMessageBox error;
	error.setWindowTitle(QString::fromStdString(title));
	error.setText(QString::fromStdString(text));
	error.exec();
	if(e_msg!="")ui->logPlainText->appendHtml("<b style='color:red'>Code Error: " + QString::fromStdString(e_msg) + " </b>");
}

float MainWindow::testOfLowe(std::vector<std::vector<cv::DMatch>> twoMatches, float lowesRatio, float limitDistance, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches){
	// put matches that accept the Lowe's test in goodMatches
	// and put the other matches in badMatches
	// and return sum of distances of goodMatches
	double sumDistances = 0; 
	for (int i = 0; i < twoMatches.size(); i++){
		if ((twoMatches[i][0].distance <= limitDistance || !ui->matcherInlierLimitDistance->isChecked())
		  &&(twoMatches[i][0].distance <= lowesRatio*twoMatches[i][1].distance)){
			sumDistances += twoMatches[i][0].distance;
			goodMatches.push_back(twoMatches[i][0]);
		}
		else{
			badMatches.push_back(twoMatches[i][0]);
		}
	}
	return sumDistances;
}

float MainWindow::testInReverse(std::vector<cv::DMatch> directMatches, std::vector<cv::DMatch> inverseMatches, std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints, float limitDistance, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches){
	// put matches that are in direct and reverse Matches in goodMatches
	// and put the other matches in badMatches
	// and return sum of distances of goodMatches
 	double sumDistances = 0;
	for (int i = 0; i < directMatches.size(); i++){
		if (directMatches[i].distance <= limitDistance || !ui->matcherInlierLimitDistance->isChecked()){
			// Check if the match is the same in reverse
			for (int j = 0; j < inverseMatches.size(); j++)
			{
				if ((secondImgKeypoints[j].pt == secondImgKeypoints[directMatches[i].trainIdx].pt)
					&& (firstImgKeypoints[inverseMatches[j].trainIdx].pt == firstImgKeypoints[i].pt))
				{
					sumDistances += directMatches[i].distance;
					goodMatches.push_back(directMatches[i]);
					// go out the loop
					goto AfterFindingAcceptedMatch_testInReverse_LABEL;
				}
			}
		}
		badMatches.push_back(directMatches[i]);
		AfterFindingAcceptedMatch_testInReverse_LABEL:;
	}
	return sumDistances;
}

cv::Mat MainWindow::maskMatchesByMinutiaeNature(std::vector<Minutiae> firstImgKeypoints, std::vector<Minutiae> secondImgKeypoints){
	// To matche bifurcation with bifurcation and ridge ending with ridge ending...
	// Only if keypoints are minutiae
	cv::Mat mask = cv::Mat(firstImgKeypoints.size(), secondImgKeypoints.size(), CV_8UC1);
	for (size_t i = 0; i < firstImgKeypoints.size(); i++)
	{
		for (size_t j = 0; j < secondImgKeypoints.size(); j++)
		{
			if (firstImgKeypoints[i].getType() == secondImgKeypoints[j].getType()) mask.at<uchar>(i, j) = 1;
			else mask.at<uchar>(i, j) = 0;
		}
	}
	return mask;
}

bool MainWindow::showDecision(){
	float scoreThreshold = ui->decisionStageThresholdText->text().toFloat();

	if (oneToN) {
		if (bestScoreIndex > -1){
			// Show infos related to best image
			QTextCursor current_cursor = QTextCursor(ui->logPlainText->document());
			current_cursor.setPosition(prev_cursor_position);
			current_cursor.insertText("\nFound " + QString::number(setImgsKeypoints[bestScoreIndex].size()) + " key points in the most similar image!");
			displayImage(setImgs[bestScoreIndex].second, 2);
			writeKeyPoints(setImgs[bestScoreIndex].second, setImgsKeypoints[bestScoreIndex], 2, "keypoints2");

			if (scoreSet[bestScoreIndex] >= scoreThreshold)
				ui->logPlainText->appendHtml("The image <b>" + QString::fromStdString(setImgs[bestScoreIndex].first) + "</b> has the best matching score: <b>" + QString::number(scoreSet[bestScoreIndex]) + "</b><b style='color:green'> &ge; </b>" + QString::number(scoreThreshold));
			else ui->logPlainText->appendHtml("The image <b>" + QString::fromStdString(setImgs[bestScoreIndex].first) + "</b> has the best matching score: <b>" + QString::number(scoreSet[bestScoreIndex]) + "</b><b style='color:red'> &#60; </b>" + QString::number(scoreThreshold));

			if (ui->imageExistsInBdd->isEnabled() && ui->imageExistsInBdd->isChecked()){
				ui->logPlainText->appendHtml("The first image is Rank-<b>" + QString::number(computeRankK(scoreThreshold)) + "</b> ");
				if (ui->bddImageNames->currentIndex() > -1) if (scoreSet[ui->bddImageNames->currentIndex()] < scoreThreshold) ui->logPlainText->appendHtml("There is a False Non-Match (FNM)");
			}
			else{
				if (scoreSet[bestScoreIndex] >= scoreThreshold)ui->logPlainText->appendHtml("There is a False Match (FM)");
			}
			// View results
			displayMatches(bestScoreIndex);
			writeMatches(bestScoreIndex);
			exportTable(goodMatchesSet[bestScoreIndex].size() + badMatchesSet[bestScoreIndex].size());
			return true;
		}
	}
	else {
		if (score >= scoreThreshold)
			ui->logPlainText->appendHtml("Matching score = <b>" + QString::number(score) + "</b><b style='color:green'> &ge; </b>" + QString::number(scoreThreshold));
		else ui->logPlainText->appendHtml("Matching score = <b>" + QString::number(score) + "</b><b style='color:red'> &#60; </b>" + QString::number(scoreThreshold));

		// View results
		displayMatches();
		writeMatches();
		exportTable(goodMatches.size() + badMatches.size());
		return true;
	}
}

void MainWindow::importExcelFile(int type)
{
	try
	{
		bool exist = false;
		int column;
		float scoreThreshold, acceptedMatches, rejectedMatches, bestImageAverage, bestImageScore, goodProbability, badProbability;
		if (type == 0) excelRecover = new ExcelManager(true, exportFile, 0);
		else excelRecover = new ExcelManager(true, inputFile, 0);

		for (int methodIndex = 1; methodIndex <= 5; methodIndex++) {
			excelRecover->GetIntRows(methodIndex);
			column = excelRecover->getColumnsCount();
			for (int j = 2; j <= excelRecover->getSheetCount(); j++) {
				if ((type == 2) || ((ui->tabWidget_2->currentIndex() == 1) && (excelRecover->GetCellValue(j, 1) == ui->spinBox->text()))) {
					cv::Mat image;
					ui->tabWidget_2->setCurrentIndex(0);
					ui->viewTabs->setCurrentIndex(0);
					if (methodIndex < 5)
					{
						ui->allMethodsTabs->setCurrentIndex(0);
						ui->defaultTabs->setCurrentIndex(methodIndex - 1);
					}
					else {
						ui->allMethodsTabs->setCurrentIndex(1);
						ui->customTabs->setCurrentIndex(0);
					}

					if (type == 0)
					{
						for (int i = 0; i < 5; i++) ui->viewTabs->setCurrentIndex(i); // just to center contents
						image = cv::imread(excelRecover->GetCellValue(j, 3).toString().toStdString(), CV_LOAD_IMAGE_COLOR);
						displayImage(image, 1);
					}
					ui->firstImgText->setText(excelRecover->GetCellValue(j, 3 - type).toString());

					if (type == 0)
					{
						image = cv::imread(("Tests/" + ui->spinBox->text() + "/keypoints1.bmp").toStdString(), CV_LOAD_IMAGE_COLOR);
						displayFeature(image, 1);
					}

					ui->secondImgText->setText(excelRecover->GetCellValue(j, 4 - type).toString());

					if (type == 0)
					{
						image = cv::imread(("Tests/" + ui->spinBox->text() + "/keypoints2.bmp").toStdString(), CV_LOAD_IMAGE_COLOR);
						displayFeature(image, 2);

						image = cv::imread(("Tests/" + ui->spinBox->text() + "/output.jpg").toStdString(), CV_LOAD_IMAGE_COLOR);
						displayFeature(image, 3);
					}

					switch (methodIndex) {
					case 1:
					{
						// SIFT
						ui->siftContThreshText->setText(excelRecover->GetCellValue(j, 8 - type).toString());
						ui->siftEdgeThreshText->setText(excelRecover->GetCellValue(j, 9 - type).toString());
						ui->siftNumFeatText->setText(excelRecover->GetCellValue(j, 10 - type).toString());
						ui->siftNumOctText->setText(excelRecover->GetCellValue(j, 11 - type).toString());
						ui->siftSigmaText->setText(excelRecover->GetCellValue(j, 12 - type).toString());
						ui->siftBruteForceCheck->setChecked(excelRecover->GetCellValue(j, 13 - type).toBool());
						break;
					}
					case 2:
					{
						// SURF
						ui->surfHessianThreshText->setText(excelRecover->GetCellValue(j, 8 - type).toString());
						ui->surfNumOctavesText->setText(excelRecover->GetCellValue(j, 9 - type).toString());
						ui->surfNumOctLayersText->setText(excelRecover->GetCellValue(j, 10 - type).toString());
						ui->surfExtendedText->setChecked(excelRecover->GetCellValue(j, 11 - type).toBool());
						ui->surfUprightText->setChecked(excelRecover->GetCellValue(j, 12 - type).toBool());
						ui->surfBruteForceCheck->setChecked(excelRecover->GetCellValue(j, 13 - type).toBool());
						break;
					}
					case 3:
					{
						// ORB
						ui->orbNumFeatText->setText(excelRecover->GetCellValue(j, 8 - type).toString());
						ui->orbScaleFactText->setText(excelRecover->GetCellValue(j, 9 - type).toString());
						ui->orbNumLevelsText->setText(excelRecover->GetCellValue(j, 10 - type).toString());
						ui->orbEdgeThreshText->setText(excelRecover->GetCellValue(j, 11 - type).toString());
						ui->orbFirstLevText->setText(excelRecover->GetCellValue(j, 12 - type).toString());
						ui->orbWTAKText->setText(excelRecover->GetCellValue(j, 13 - type).toString());
						if (excelRecover->GetCellValue(j, 14 - type).toString() == "Harris") ui->orbScoreHarrisRadioBtn->setChecked(true);
						else ui->orbScoreFastRadioBtn->setChecked(true);
						ui->orbPatchSizeText->setText(excelRecover->GetCellValue(j, 15 - type).toString());
						break;
					}
					case 4:
					{
						// BRISK
						ui->briskPatternScaleText->setText(excelRecover->GetCellValue(j, 8 - type).toString());
						ui->briskOctavesText->setText(excelRecover->GetCellValue(j, 9 - type).toString());
						ui->briskThreshText->setText(excelRecover->GetCellValue(j, 10 - type).toString());
						break;
					}
					default:
					{
						// Custom
						int segmentationName = segmentationNameToInt(excelRecover->GetCellValue(j, 8 - type).toString()),
							detectorName = detectorNameToInt(excelRecover->GetCellValue(j, 13 - type).toString()),
							descriptorName = descriptorNameToInt(excelRecover->GetCellValue(j, 19 - type).toString()),
							matcherName = matcherNameToInt(excelRecover->GetCellValue(j, 25 - type).toString());

						ui->segmentationTabs->setCurrentIndex(segmentationName);
						ui->detectorTabs->setCurrentIndex(detectorName);
						ui->descriptorTabs->setCurrentIndex(descriptorName);
						ui->matcherTabs->setCurrentIndex(matcherName);

						ui->segmentationThresholdText->setText(excelRecover->GetCellValue(j + 1, 9 - type).toString());

						switch (segmentationName)
						{
						case 1:
						{
							// Skeletonization of Morphological Skeleton
							ui->segmentationMorphologicalSkeletonParam1Text->setText(excelRecover->GetCellValue(j + 1, 10 - type).toString());
							ui->segmentationMorphologicalSkeletonParam2Text->setText(excelRecover->GetCellValue(j + 1, 11 - type).toString());
							ui->segmentationMorphologicalSkeletonParam3Text->setText(excelRecover->GetCellValue(j + 1, 12 - type).toString());
							break;
						}
						case 2:
						{
							// Thinning of Zhang-Suen
							ui->segmentationZhangSuenParam1Text->setText(excelRecover->GetCellValue(j + 1, 10 - type).toString());
							ui->segmentationZhangSuenParam2Text->setText(excelRecover->GetCellValue(j + 1, 11 - type).toString());
							ui->segmentationZhangSuenParam3Text->setText(excelRecover->GetCellValue(j + 1, 12 - type).toString());
							break;
						}
						case 3:
						{
							// Thinning of Lin-Hong implemented by Mrs. Faiçal
							ui->segmentationLinHongParam1Text->setText(excelRecover->GetCellValue(j + 1, 10 - type).toString());
							ui->segmentationLinHongParam2Text->setText(excelRecover->GetCellValue(j + 1, 11 - type).toString());
							ui->segmentationLinHongParam3Text->setText(excelRecover->GetCellValue(j + 1, 12 - type).toString());
							break;
						}
						case 4:
						{
							// Thinning of Guo-Hall
							ui->segmentationGuoHallParam1Text->setText(excelRecover->GetCellValue(j + 1, 10 - type).toString());
							ui->segmentationGuoHallParam2Text->setText(excelRecover->GetCellValue(j + 1, 11 - type).toString());
							ui->segmentationGuoHallParam3Text->setText(excelRecover->GetCellValue(j + 1, 12 - type).toString());
							break;
						}

						}

						switch (detectorName){
						case 0:{
							// Minutiae-detection using Crossing Number By Dr. Faiçal
							ui->detectorMinutiaeParam1Text->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorMinutiaeParam2Text->setText(excelRecover->GetCellValue(j + 1, 15 - type).toString());
							break;
						}
						case 1:{
							// Minutiae-detection using Crossing Number
							ui->detectorCrossingNumberBorderText->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorCrossingNumberParamText->setText(excelRecover->GetCellValue(j + 1, 15 - type).toString());
							break;
						}
						case 2:{
							// Harris-Corners
							ui->detectorHarrisThresholdText->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorHarrisParamText->setText(excelRecover->GetCellValue(j + 1, 15 - type).toString());
							break;
						}
						case 3:{
							// STAR
							ui->detectorStarMaxSizeText->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorStarResponseThresholdText->setText(excelRecover->GetCellValue(j + 1, 15 - type).toString());
							ui->detectorStarThresholdProjectedText->setText(excelRecover->GetCellValue(j + 1, 16 - type).toString());
							ui->detectorStarThresholdBinarizedText->setText(excelRecover->GetCellValue(j + 1, 17 - type).toString());
							ui->detectorStarSuppressNonmaxSizeText->setText(excelRecover->GetCellValue(j + 1, 18 - type).toString());
							break;
						}
						case 4: {
							// FAST
							ui->detectorFastThresholdText->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorFastNonmaxSuppressionCheck->setChecked(excelRecover->GetCellValue(j + 1, 15 - type).toBool());
							ui->detectorFastXCheck->setChecked(excelRecover->GetCellValue(j + 1, 16 - type).toBool());
							if (excelRecover->GetCellValue(j + 1, 16 - type).toBool())
							{
								ui->detectorFastTypeText->setEnabled(true);
								int index = ui->detectorFastTypeText->findText(excelRecover->GetCellValue(j + 1, 17 - type).toString());
								ui->detectorFastTypeText->setCurrentIndex(index);
							}
							else ui->detectorFastTypeText->setEnabled(false);
							break;
						}
						case 5: {
							// SIFT
							ui->detectorSiftContrastThresholdText->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorSiftEdgeThresholdText->setText(excelRecover->GetCellValue(j + 1, 15 - type).toString());
							ui->detectorSiftNfeaturesText->setText(excelRecover->GetCellValue(j + 1, 16 - type).toString());
							ui->detectorSiftNOctaveLayersText->setText(excelRecover->GetCellValue(j + 1, 17 - type).toString());
							ui->detectorSiftSigmaText->setText(excelRecover->GetCellValue(j + 1, 18 - type).toString());
							break;
						}
						case 6: {
							//SURF
							ui->detectorSurfHessianThresholdText->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorSurfNOctavesText->setText(excelRecover->GetCellValue(j + 1, 15 - type).toString());
							ui->detectorSurfNLayersText->setText(excelRecover->GetCellValue(j + 1, 16 - type).toString());
							ui->detectorSurfUprightText->setChecked(excelRecover->GetCellValue(j + 1, 17 - type).toBool());
							break;
						}
						case 7: {
							//Dense
							ui->detectorDenseInitFeatureScaleText->setText(excelRecover->GetCellValue(j + 1, 14 - type).toString());
							ui->detectorDenseFeatureScaleLevelsText->setText(excelRecover->GetCellValue(j + 1, 15 - type).toString());
							ui->detectorDenseFeatureScaleMulText->setText(excelRecover->GetCellValue(j + 1, 16 - type).toString());
							ui->detectorDenseInitXyStepText->setText(excelRecover->GetCellValue(j + 1, 17 - type).toString());
							ui->detectorDenseInitImgBoundText->setText(excelRecover->GetCellValue(j + 1, 18 - type).toString());
							break;
						}
						}

						switch (descriptorName)
						{
						case 0:
						{
							// FREAK
							ui->descriptorFreakOrientationNormalizedCheck->setChecked(excelRecover->GetCellValue(j + 1, 20 - type).toBool());
							ui->descriptorFreakScaleNormalizedCheck->setChecked(excelRecover->GetCellValue(j + 1, 21 - type).toBool());
							ui->descriptorFreakPatternScaleText->setText(excelRecover->GetCellValue(j + 1, 22 - type).toString());
							ui->descriptorFreakNOctavesText->setText(excelRecover->GetCellValue(j + 1, 23 - type).toString());
							ui->descriptorFreakSelectedPairsText->setText(excelRecover->GetCellValue(j + 1, 24 - type).toString());
							break;
						}
						case 1:
						{
							// BRIEF
							ui->descriptorBriefPATCH_SIZEText->setText(excelRecover->GetCellValue(j + 1, 20 - type).toString());
							ui->descriptorBriefKERNEL_SIZEText->setText(excelRecover->GetCellValue(j + 1, 21 - type).toString());
							ui->descriptorBriefLengthText->setText(excelRecover->GetCellValue(j + 1, 22 - type).toString());
							break;
						}
						case 2:
						{
							// SIFT
							ui->descriptorSiftLengthText->setText(excelRecover->GetCellValue(j + 1, 20 - type).toString());
							break;
						}
						case 3:
						{
							//SURF
							if (excelRecover->GetCellValue(j + 1, 20 - type).toString() == "128") ui->descriptorSurfExtended->setChecked(true);
							else ui->descriptorSurfNotExtended->setChecked(true);
							break;
						}
						default:
						{
							ui->descriptorMethod2Param1Text->setText(excelRecover->GetCellValue(j + 1, 20 - type).toString());
							ui->descriptorMethod2Param2Text->setText(excelRecover->GetCellValue(j + 1, 21 - type).toString());
							ui->descriptorMethod2Param3Text->setText(excelRecover->GetCellValue(j + 1, 22 - type).toString());
							ui->descriptorMethod2LengthText->setText(excelRecover->GetCellValue(j + 1, 23 - type).toString());
							break;
						}
						}

						if (excelRecover->GetCellValue(j + 1, 26 - type).toString() == "Type 1") ui->matcher1toNtype1->setChecked(true);
						else ui->matcher1toNtype2->setChecked(true);

						OutliersEliminationToInt(excelRecover->GetCellValue(j + 1, 27 - type).toString());

						if (excelRecover->GetCellValue(j, 28 - type).toString().isEmpty())
						{
							ui->matcherInlierLimitDistance->setChecked(false);
							ui->matcherInlierLimitDistanceText->setEnabled(false);
						}
						else
						{
							ui->matcherInlierLimitDistance->setChecked(true);
							ui->matcherInlierLimitDistanceText->setEnabled(true);
							ui->matcherInlierLimitDistanceText->setText(excelRecover->GetCellValue(j + 1, 28 - type).toString());
						}

						switch (matcherName)
						{
						case 0:
						{
							// BruteForce
							int index = ui->matcherBruteForceNormTypeText->findText(excelRecover->GetCellValue(j + 1, 29 - type).toString());
							ui->matcherBruteForceNormTypeText->setCurrentIndex(index);
							ui->matcherBruteForceCrossCheckText->setChecked(excelRecover->GetCellValue(j + 1, 30 - type).toBool());
							break;
						}
						case 1:
						{
							// FlannBased
							FlannBasedNameToIndex(excelRecover->GetCellValue(j + 1, 29 - type).toString());
							ui->matcherFlannBasedSearchParamsText->setText(excelRecover->GetCellValue(j + 1, 30 - type).toString());
							break;
						}
						default:
						{
							ui->matcherMethodParam1Text->setText(excelRecover->GetCellValue(j + 1, 29 - type).toString());
							ui->matcherMethodParam2Text->setText(excelRecover->GetCellValue(j + 1, 30 - type).toString());
							ui->matcherMethodParam3Text->setText(excelRecover->GetCellValue(j + 1, 31 - type).toString());
							break;
						}
						}

						ui->opponentColor->setChecked(excelRecover->GetCellValue(j, 32 - type).toBool());

						break;
					}

					}
					if (type == 0)
					{
						ui->logPlainText->appendHtml("<b style='color:green'>Starting " + excelRecover->GetSheetName() + " test (Done on: " + excelRecover->GetCellValue(j, 2).toString() + "):</b>");

						ui->decisionStageThresholdText->setText(excelRecover->GetCellValue(j, column - 13).toString());

						ui->logPlainText->appendHtml("Found " + excelRecover->GetCellValue(j, column - 12).toString() + " key points in the first image!");

						scoreThreshold = excelRecover->GetCellValue(j, column - 13).toString().toFloat();
						acceptedMatches = excelRecover->GetCellValue(j, column - 6).toString().toFloat();
						rejectedMatches = excelRecover->GetCellValue(j, column - 5).toString().toFloat();
						bestImageAverage = excelRecover->GetCellValue(j, column - 4).toString().toFloat();
						bestImageScore = excelRecover->GetCellValue(j, column - 3).toString().toFloat();

						goodProbability = acceptedMatches / (acceptedMatches + rejectedMatches) * 100;
						badProbability = rejectedMatches / (acceptedMatches + rejectedMatches) * 100;
					}
					else ui->decisionStageThresholdText->setText(excelRecover->GetCellValue(j, column).toString());

					//ui->refreshBddImageNames->setEnabled(false);
					ui->bddImageNames->clear();
					disconnect(ui->viewMatchesImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
					ui->viewMatchesImageNameText->clear();
					disconnect(ui->viewTableImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
					ui->viewTableImageNameText->clear();

					if (excelRecover->GetCellValue(j, 5 - type).toBool())
					{
						if (type == 0) image = cv::imread(excelRecover->GetCellValue(j, 4).toString().toStdString() + '/' + excelRecover->GetCellValue(j, column - 1).toString().toStdString(), CV_LOAD_IMAGE_COLOR);
						ui->oneToN->setChecked(true);
						ui->imageExistsInBdd->setChecked(excelRecover->GetCellValue(j, 6 - type).toBool());
						if (excelRecover->GetCellValue(j, 6 - type).toBool()) ui->bddImageNames->addItem(excelRecover->GetCellValue(j, 7 - type).toString());
						else
						{
							ui->imageExistsInBdd->setEnabled(false);
							ui->bddImageNames->setEnabled(false);
						}
						if (type == 0)
						{
							ui->logPlainText->appendHtml("Found " + excelRecover->GetCellValue(j, column - 11).toString() + " key points in the most similar image!");
							ui->logPlainText->appendHtml("Detection time : " + excelRecover->GetCellValue(j, column - 10).toString());
							ui->logPlainText->appendHtml("Description time : " + excelRecover->GetCellValue(j, column - 9).toString());
							ui->logPlainText->appendHtml("Matching time : " + excelRecover->GetCellValue(j, column - 8).toString());
							ui->logPlainText->appendHtml("Total time : " + excelRecover->GetCellValue(j, column - 7).toString());
							ui->viewMatchesImageNameText->addItem(excelRecover->GetCellValue(j, column - 1).toString());
							ui->viewTableImageNameText->addItem(excelRecover->GetCellValue(j, column - 1).toString());
							if (bestImageScore >= scoreThreshold)
								ui->logPlainText->appendHtml("The image <b>" + excelRecover->GetCellValue(j, column - 1).toString() + "</b> has the best matching score = <b>" + QString::number(bestImageScore) + "</b><b style='color:green'> &ge; </b>" + QString::number(scoreThreshold));
							else ui->logPlainText->appendHtml("The image <b>" + excelRecover->GetCellValue(j, column - 1).toString() + "</b> has the best matching score = <b>" + QString::number(bestImageScore) + "</b><b style='color:red'> &#60; </b>" + QString::number(scoreThreshold));
							if (excelRecover->GetCellValue(j, 6).toBool()) ui->logPlainText->appendHtml("The first image is Rank-<b>" + excelRecover->GetCellValue(j, column).toString() + "</b>");
						}
					}
					else
					{
						if (type == 0) image = cv::imread(excelRecover->GetCellValue(j, 4).toString().toStdString(), CV_LOAD_IMAGE_COLOR);
						ui->oneToN->setChecked(false);
						ui->imageExistsInBdd->setChecked(false);
						ui->imageExistsInBdd->setEnabled(false);
						ui->bddImageNames->setEnabled(false);
						ui->viewMatchesImageNameText->setEnabled(false);
						ui->viewTableImageNameText->setEnabled(false);
						if (type == 0)
						{
							ui->logPlainText->appendHtml("Found " + excelRecover->GetCellValue(j, column - 11).toString() + " key points in the second image!");
							ui->logPlainText->appendHtml("Detection time : " + excelRecover->GetCellValue(j, column - 10).toString());
							ui->logPlainText->appendHtml("Description time : " + excelRecover->GetCellValue(j, column - 9).toString());
							ui->logPlainText->appendHtml("Matching time : " + excelRecover->GetCellValue(j, column - 8).toString());
							ui->logPlainText->appendHtml("Total time : " + excelRecover->GetCellValue(j, column - 7).toString());
							if (bestImageScore >= scoreThreshold)
								ui->logPlainText->appendHtml("Matching score = <b>" + QString::number(bestImageScore) + "</b><b style='color:green'> &ge; </b>" + QString::number(scoreThreshold));
							else ui->logPlainText->appendHtml("Matching score = <b>" + QString::number(bestImageScore) + "</b><b style='color:red'> &#60; </b>" + QString::number(scoreThreshold));
						}
					}
					if (type == 0)
					{
						displayImage(image, 2);
						ui->viewMatchesGoodMatchesText->setText("<b style='color:green'>" + QString::number(acceptedMatches) + "</b>/" + QString::number(acceptedMatches + rejectedMatches) + " = <b style = 'color:green'>" + QString::number(goodProbability) + "</b>%");
						ui->viewMatchesBadMatchesText->setText("<b style='color:red'>" + QString::number(rejectedMatches) + "</b>/" + QString::number(acceptedMatches + rejectedMatches) + " = <b style = 'color:red'>" + QString::number(badProbability) + "</b>%");
						ui->viewMatchesAverageMatchesText->setText(QString::number(bestImageAverage));
						ui->viewMatchesScoreMatchesText->setText("<b>" + QString::number(bestImageScore) + "</b>");
						if (!excelRecover->GetCellValue(j, column - 11).toString().isEmpty()) importTable(ui->spinBox->text().toInt());
						exist = true;
						QMessageBox::information(this, "Import Excel Success!", "The test N°: " + QString(ui->spinBox->text()) + " has been imported with success !");
						break;
					}
					else
					{
						if (takeTest()) exportSuccess(1);
						if (methodIndex == 5) j++;
					}
				}
			}
		}
		if (type == 0)
		{
			if (!exist) QMessageBox::warning(this, "Import Excel Error!", "Please check the number that has been entered because no ID matches this number !");
		}
		else
		{
			//QMessageBox::information(this, "Import Input file!", "The execution of all commands has been finished with success !");
			ui->logPlainText->appendHtml("<b style='color:blue'>he execution of all commands has been finished with success !</b>");
		}
		excelRecover->~ExcelManager();
		system("taskkill /im EXCEL.EXE /f");
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error lors de l'importation du fichier excel !", e.what());
	}
}

int MainWindow::segmentationNameToInt(const QString& value)
{
	if (value == "Morphological Skeleton") return 1;
	else if (value == "Zhang-Suen") return 2;
	else if (value == "Lin-Hong") return 3;
	else if (value == "Guo-Hall") return 4;
	else return 0;
}

int MainWindow::detectorNameToInt(const QString& value)
{
	if (value == "Minutiae") return 0;
	else if (value == "Crossing-Number") return 1;
	else if (value == "Harris-Corners") return 2;
	else if (value == "STAR") return 3;
	else if (value == "FAST") return 4;
	else if (value == "SIFT") return 5;
	else if (value == "SURF") return 6;
	else return 7;
}

int MainWindow::descriptorNameToInt(const QString& value)
{
	if (value == "FREAK") return 0;
	else if (value == "BRIEF") return 1;
	else if (value == "SIFT") return 2;
	else if (value == "SURF") return 3;
	else return 4;
}

int MainWindow::matcherNameToInt(const QString& value)
{
	if (value == "Brute-Force") return 0;
	else if (value == "Flann-Based") return 1;
	else return 2;
}

void MainWindow::OutliersEliminationToInt(const QString& value)
{
	if (value == "Without test")
	{
		ui->matcherInlierNoTest->setChecked(true);
		ui->matcherInlierLoweRatioText->setEnabled(false);
	}
	else
	{
		if (value == "In reverse matching test")
		{
			ui->matcherInlierInversMatches->setChecked(true);
			ui->matcherInlierLoweRatioText->setEnabled(false);
		}
		else
		{
			if (!value.isEmpty())
			{
				ui->matcherInlierLoweRatio->setChecked(true);
				ui->matcherInlierLoweRatioText->setEnabled(true);

				QRegExp rx("(\\:)"); //RegEx for ':'
				QStringList matcher = value.split(rx);
				ui->matcherInlierLoweRatioText->setText(matcher[1]);
			}
		}
	}
}

void MainWindow::FlannBasedNameToIndex(const QString& value)
{
	if (value == "Linear Index") ui->matcherFlannBasedLinearIndexParams->setChecked(true);
	else if (value == "KD Tree Index") ui->matcherFlannBasedKDTreeIndexParams->setChecked(true);
	else if (value == "K-Means Index") ui->matcherFlannBasedKMeansIndexParams->setChecked(true);
	else if (value == "Composite Index") ui->matcherFlannBasedCompositeIndexParams->setChecked(true);
	else if (value == "LSH Index") ui->matcherFlannBasedLshIndexParams->setChecked(true);
	else ui->matcherFlannBasedAutotunedIndexParams->setChecked(true);
}

void MainWindow::setTableValue(QAxObject* sheet, int lineIndex, int columnIndex, const QString& value)
{
	QAxObject *cell = sheet->querySubObject("Cells(int,int)", lineIndex, columnIndex);
	cell->setProperty("Value", value);
	if (columnIndex == 6) cell->setProperty("HorizontalAlignment", -4108);
	if (lineIndex == 1) {
		cell->setProperty("ColumnWidth", value.size() + 4);
		cell->setProperty("HorizontalAlignment", -4108);
		QAxObject* chars = cell->querySubObject("Characters(int, int)", 1, value.size());
		QAxObject* font = chars->querySubObject("Font");
		font->setProperty("Bold", true);
	}
	delete cell;
}

void MainWindow::initializeTable() {
	model = new QStandardItemModel(0, 6, this); //0 Rows and 6 Columns
	model->setHorizontalHeaderItem(0, new QStandardItem(QString("Coordinate X1")));
	model->setHorizontalHeaderItem(1, new QStandardItem(QString("Coordinate Y1")));
	model->setHorizontalHeaderItem(2, new QStandardItem(QString("Coordinate X2")));
	model->setHorizontalHeaderItem(3, new QStandardItem(QString("Coordinate Y2")));
	model->setHorizontalHeaderItem(4, new QStandardItem(QString("Distance")));
	model->setHorizontalHeaderItem(5, new QStandardItem(QString("Accepted/Rejected")));
}

QString MainWindow::GetTableValue(QAxObject* sheet, int rowIndex, int columnIndex)
{
	QString value;
	QAxObject* cell = sheet->querySubObject("Cells(Int, Int)", rowIndex, columnIndex);
	value = cell->dynamicCall("Value()").toString();
	delete cell;
	return value;
}

void MainWindow::exportTable(int rowsCount) {
	const QString tableName = QDir::toNativeSeparators(QDir::currentPath()) + "\\Tests\\" + QString::number(cpt) + "\\table.xlsx";
	system("taskkill /fi \"WINDOWTITLE eq table.xlsx - Excel\" /f");
	QAxObject *excelApplication = new QAxObject("Excel.Application", 0);
	if (excelApplication == nullptr) throw std::invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");
	excelApplication->dynamicCall("SetVisible(bool)", false); // display excel
	excelApplication->setProperty("DisplayAlerts", 0); // disable alerts
	QAxObject *workbooks = excelApplication->querySubObject("Workbooks");
	QAxObject *workbook = workbooks->querySubObject("Add");
	QAxObject *sheets = workbook->querySubObject("Worksheets");
	QAxObject *sheet = sheets->querySubObject("Item(int)", 1);
	setTableValue(sheet, 1, 1, "Coordinate X1");
	setTableValue(sheet, 1, 2, "Coordinate Y1");
	setTableValue(sheet, 1, 3, "Coordinate X2");
	setTableValue(sheet, 1, 4, "Coordinate Y2");
	setTableValue(sheet, 1, 5, "Distance");
	setTableValue(sheet, 1, 6, "Accepted/Rejected");
	for (int i = 0; i < rowsCount; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			setTableValue(sheet, i + 2, j + 1, ui->viewTable->model()->data(model->index(i, j, QModelIndex())).toString());
		}
	}
	workbook->dynamicCall("SaveAs (const QString&)", tableName);
	workbook->dynamicCall("Close (Boolean)", true);
	excelApplication->dynamicCall("Quit (void)");
}

void MainWindow::importTable(int identifierNumber) {
	system("taskkill /fi \"WINDOWTITLE eq table.xlsx - Excel\" /f");
	const QString tableName = QDir::toNativeSeparators(QDir::currentPath()) + "\\Tests\\" + QString::number(identifierNumber) + "\\table.xlsx";
	QAxObject *excelApplication = new QAxObject("Excel.Application", 0);
	if (excelApplication == nullptr) throw std::invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");
	excelApplication->dynamicCall("SetVisible(bool)", false); // display excel
	excelApplication->setProperty("DisplayAlerts", 0); // disable alerts
	QAxObject *workbooks = excelApplication->querySubObject("Workbooks");
	QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", tableName);
	QAxObject *sheets = workbook->querySubObject("Worksheets");
	QAxObject *sheet = sheets->querySubObject("Item(int)", 1);
	QAxObject *usedrange = sheet->querySubObject("UsedRange");
	QAxObject *rows = usedrange->querySubObject("Rows");
	int rowsCount = rows->property("Count").toInt();

	initializeTable();
	QStandardItem *value;

	for (int i = 2; i <= rowsCount; i++)
	{
		for (int j = 1; j <= 6; j++)
		{
			value = new QStandardItem(QString(GetTableValue(sheet, i, j)));
			model->setItem(i - 2, j - 1, value);
			if (j == 6)
			{
				if (GetTableValue(sheet, i, 6) == "Accepted") value->setData(QColor(Qt::green), Qt::BackgroundRole);
				else value->setData(QColor(Qt::red), Qt::BackgroundRole);
			}
		}
	}
	workbook->dynamicCall("Close (Boolean)", true);
	excelApplication->dynamicCall("Quit (void)");
	ui->viewTable->setModel(model);
}

void MainWindow::exportSuccess(int showMethod)
{
	if (showMethod == 1) ui->logPlainText->appendHtml("<b style='color:green'>This test has been exported with success under the identifier number: " + QString::number(cpt) + "</b>");
	else QMessageBox::information(this, "Export Excel !", "This test has been exported with success under the identifier number: " + QString::number(cpt));
}
QString MainWindow::getCurrentTime() {
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y %H:%M");
	auto str = oss.str();
	QString curtime = QString::fromStdString(str);
	return curtime;
}

void MainWindow::makePlot(){
	// generate some data:
	QVector<double> x(1001), y(1001); // initialize with entries 0..100
	for (int i = 0; i<1001; ++i)
	{
		x[i] = i / 5.0; // x goes from 0 to 100
		y[i] = x[i] * x[i]; // let's plot a quadratic function
	}
	// create graph and assign data to it:
	ui->rankkGraphWidget->addGraph();
	ui->rankkGraphWidget->graph(0)->setData(x, y);
	ui->rankkGraphWidget->graph(0)->setName("y=x²");
	ui->rankkGraphWidget->addGraph();
	ui->rankkGraphWidget->graph(1)->setData(y, x);
	ui->rankkGraphWidget->graph(1)->setName("y=sqrt(x)");
	// give the axes some labels:
	ui->rankkGraphWidget->xAxis->setLabel("x");
	ui->rankkGraphWidget->yAxis->setLabel("y");
	// set axes ranges, so we see all data:
	ui->rankkGraphWidget->xAxis->setRange(0, 5);
	ui->rankkGraphWidget->yAxis->setRange(0, 10);
	// set legend
	ui->rankkGraphWidget->legend->setVisible(true);
	ui->rankkGraphWidget->legend->setFont(QFont("Helvetica", 9));
	// set locale to english, so we get english decimal separator:
	ui->rankkGraphWidget->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
	// configuration:
	QPen pen;
	pen.setStyle(Qt::DotLine);
	pen.setWidth(1);
	pen.setColor(QColor(180, 180, 180));
	pen.setStyle(Qt::DashLine);
	pen.setWidth(2);
	pen.setColor(Qt::red);

	ui->rankkGraphWidget->graph(1)->setPen(pen);
	ui->rankkGraphWidget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
	ui->rankkGraphWidget->graph(0)->setPen(QPen(QColor(120, 120, 120), 2));
	ui->rankkGraphWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);
	ui->rankkGraphWidget->xAxis->setSelectableParts(QCPAxis::spAxisLabel | QCPAxis::spAxis | QCPAxis::spTickLabels);
	ui->rankkGraphWidget->yAxis->setSelectableParts(QCPAxis::spAxisLabel | QCPAxis::spAxis | QCPAxis::spTickLabels);
	ui->rankkGraphWidget->replot();
}

void MainWindow::drowRankk(int maxRank){
	// drow Rank-k gragh
	// generate data:
	QVector<double> x(rankkData.size()), y(rankkData.size());
	for (int i = 0; i < rankkData.size(); i++)
	{
		x[i] = rankkData[i].first;
		if (x[i] != 0) rankkData[i].second = rankkData[i].second / static_cast<float>(maxRank)* 100;
		y[i] = rankkData[i].second;

		if (x[i] == 1 && y[i]!=0) {
			// add the text label at the top:
			QCPItemText *textLabel = new QCPItemText(ui->rankkGraphWidget);
			textLabel->setPositionAlignment(Qt::AlignTop| Qt::AlignHCenter);
			textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
			textLabel->setText("Rank-1= "+QString::number(y[i])+"%");
			textLabel->setFont(QFont(font().family(), 9)); // make font a bit larger
			textLabel->setPen(QPen(Qt::black)); // show black border around text

			// add the arrow:
			QCPItemLine *arrow = new QCPItemLine(ui->rankkGraphWidget);
			arrow->start->setParentAnchor(textLabel->top);
			double yPos = 1 - y[i] / 100 + 0.05;
			if (yPos > 0.9){
				yPos -= 0.2;
				arrow->start->setParentAnchor(textLabel->bottom);
			}
			textLabel->position->setCoords(0.10, yPos); // place position at center/top of axis rect
			arrow->end->setCoords(x[i], y[i]); // point to rank-1 plot coordinates
			arrow->setHead(QCPLineEnding::esSpikeArrow);
		}
	}
	// create graph and assign data to it:
	ui->rankkGraphWidget->addGraph();
	ui->rankkGraphWidget->graph(0)->setData(x, y);
	ui->rankkGraphWidget->graph(0)->setName("Rank-k");
	// give the axes some labels:
	ui->rankkGraphWidget->xAxis->setLabel("Rank");
	ui->rankkGraphWidget->yAxis->setLabel("Percent %");
	// set axes ranges, so we see all data:
	ui->rankkGraphWidget->xAxis->setRange(0, x[x.size()-1]);
	QCPAxisTickerFixed *fixedTicker = new QCPAxisTickerFixed();
	fixedTicker->setTickStep(1);
	ui->rankkGraphWidget->xAxis->setTicker(QSharedPointer<QCPAxisTickerFixed>(fixedTicker));
	ui->rankkGraphWidget->yAxis->setRange(0, 110);
	// set legend
	ui->rankkGraphWidget->legend->setVisible(true);
	ui->rankkGraphWidget->legend->setFont(QFont("Helvetica", 9));
	// set locale to english, so we get english decimal separator:
	ui->rankkGraphWidget->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
	// configuration:
	ui->rankkGraphWidget->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));
	ui->rankkGraphWidget->graph(0)->setPen(QPen(QColor(120, 120, 120), 2));
	ui->rankkGraphWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	ui->rankkGraphWidget->axisRect()->setRangeDrag(Qt::Horizontal); // drag only on x
	ui->rankkGraphWidget->axisRect()->setRangeZoom(Qt::Horizontal); // zoom only on x
	
	// drow
	ui->rankkGraphWidget->replot();
}

void MainWindow::showRankkToolTip(QMouseEvent *event)
{
	float x = ui->rankkGraphWidget->xAxis->pixelToCoord(event->pos().x());
	//int y = ui->rankkGraphWidget->yAxis->pixelToCoord(event->pos().y());
	if (rankkData[0].first <= x && x <= rankkData[rankkData.size() - 1].first){
		if (x <= (floor(x) + 0.5))setToolTip(QString("rank-%1 = %2%").arg(floor(x)).arg(rankkData[floor(x)].second));
		else setToolTip(QString("rank-%1 = %2%").arg(floor(x) + 1).arg(rankkData[floor(x)+1].second));
	}
	else setToolTip("");
}

void MainWindow::drowEer(std::map<float, std::pair<int, int>> FMR_dataFromExcel, std::map<float, std::pair<int, int>> FNMR_dataFromExcel){
	// drow ERR gragh
	// generate data:
	QVector<double> xFMR, yFMR;
	QVector<double> xFNMR, yFNMR;
	
	std::map<float, std::pair<int, int>>::iterator it;

	for (it = FMR_dataFromExcel.begin(); it != FMR_dataFromExcel.end(); ++it)
	{
		xFMR.push_back(it->first); // threshold
		yFMR.push_back(static_cast<float>(it->second.first) / static_cast<float>(it->second.second) * 100); // nbFM / nbExists %
	}
	for (it = FNMR_dataFromExcel.begin(); it != FNMR_dataFromExcel.end(); ++it)
	{
		xFNMR.push_back(it->first); // threshold
		yFNMR.push_back(static_cast<float>(it->second.first) / static_cast<float>(it->second.second) * 100); // nbFM / nbExists %
	}

	double *xFMRmaxValue = std::max_element(xFMR.begin(), xFMR.end()), *xFNMRmaxValue = std::max_element(xFNMR.begin(), xFNMR.end());
	double *xFMRminValue = std::min_element(xFMR.begin(), xFMR.end()), *xFNMRminValue = std::min_element(xFNMR.begin(), xFNMR.end());
	raven::cSpline FMRspline(xFMR.toStdVector(), yFMR.toStdVector());
	raven::cSpline FNMRspline(xFNMR.toStdVector(), yFNMR.toStdVector());
	QVector<double> xFMRsplined, yFMRsplined;
	QVector<double> xFNMRsplined, yFNMRsplined;
	for (int i = *xFMRminValue; i <= *xFMRmaxValue; i++)
	{
		xFMRsplined.push_back(i);
		yFMRsplined.push_back(FMRspline.getY(i));
	}
	for (int i = *xFNMRminValue; i <= *xFNMRmaxValue; i++)
	{
		xFNMRsplined.push_back(i);
		yFNMRsplined.push_back(FNMRspline.getY(i));
	}

	// spline graph
	ui->eerGraphWidget->addGraph();
	ui->eerGraphWidget->graph(0)->setData(xFMRsplined, yFMRsplined);
	ui->eerGraphWidget->graph(0)->setName("FMR");
	ui->eerGraphWidget->addGraph();
	ui->eerGraphWidget->graph(1)->setData(xFNMRsplined, yFNMRsplined);
	ui->eerGraphWidget->graph(1)->setName("FNMR");
	// create graph and assign data to it:
	ui->eerGraphWidget->addGraph();
	ui->eerGraphWidget->graph(2)->setData(xFMR, yFMR);
	ui->eerGraphWidget->addGraph();
	ui->eerGraphWidget->graph(3)->setData(xFNMR, yFNMR);
	// give the axes some labels:
	ui->eerGraphWidget->xAxis->setLabel("Threshold");
	ui->eerGraphWidget->yAxis->setLabel("Percent %");
	// set axes ranges, so we see all data:
	ui->eerGraphWidget->xAxis->rescale();
	ui->eerGraphWidget->xAxis->setRange(0, std::max(*xFMRmaxValue, *xFNMRmaxValue) + 10);
	QCPAxisTickerFixed *fixedTicker = new QCPAxisTickerFixed();
	fixedTicker->setTickStep(50);
	ui->eerGraphWidget->xAxis->setTicker(QSharedPointer<QCPAxisTickerFixed>(fixedTicker));
	ui->eerGraphWidget->yAxis->setRange(0, 110);
	// set legend
	ui->eerGraphWidget->legend->setVisible(true);
	ui->eerGraphWidget->legend->setFont(QFont("Helvetica", 9));
	// set locale to english, so we get english decimal separator:
	ui->eerGraphWidget->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
	// configuration:
	ui->eerGraphWidget->graph(0)->setPen(QPen(QColor(120, 140, 250), 2));
	ui->eerGraphWidget->graph(1)->setPen(QPen(QColor(120, 250, 120), 2));
	ui->eerGraphWidget->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 5));
	ui->eerGraphWidget->graph(2)->setPen(Qt::NoPen);
	ui->eerGraphWidget->graph(2)->removeFromLegend();
	ui->eerGraphWidget->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 5));
	ui->eerGraphWidget->graph(3)->setPen(Qt::NoPen);
	ui->eerGraphWidget->graph(3)->removeFromLegend();
	ui->eerGraphWidget->graph(0)->selectionDecorator()->setPen(QPen(QColor(120, 140, 250), 2));
	ui->eerGraphWidget->graph(1)->selectionDecorator()->setPen(QPen(QColor(120, 250, 120), 2));
	ui->eerGraphWidget->graph(2)->selectionDecorator()->setPen(QPen(QColor(120, 120, 120), 1));
	ui->eerGraphWidget->graph(3)->selectionDecorator()->setPen(QPen(QColor(120, 120, 120), 1));
	ui->eerGraphWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	ui->eerGraphWidget->axisRect()->setRangeDrag(Qt::Horizontal); // drag only on x
	ui->eerGraphWidget->axisRect()->setRangeZoom(Qt::Horizontal); // zoom only on x
	
	// drow
	ui->eerGraphWidget->replot();

	connect(ui->eerGraphWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showEerToolTip(QMouseEvent*)));
}

void MainWindow::showEerToolTip(QMouseEvent *event)
{
	int x = ui->eerGraphWidget->xAxis->pixelToCoord(event->pos().x());
	double y = ui->eerGraphWidget->yAxis->pixelToCoord(event->pos().y());

	/*auto itX = std::find(xFMRsplined.begin(), xFMRsplined.end(), x);
	auto itY = std::find(yFMRsplined.begin(), yFMRsplined.end(), y);
	if (itX != xFMRsplined.end() && itY != yFMRsplined.end())
	{
		int i = std::distance(xFMRsplined.begin(), itX);
		int j = std::distance(yFMRsplined.begin(), itY);
		setToolTip(QString("(%1, %2%)").arg(x).arg(y));
	}
	else
	{
		auto itX = std::find(xFNMRsplined.begin(), xFNMRsplined.end(), x);
		//auto itY = std::find(yFNMRsplined.begin(), yFNMRsplined.end(), y);
		if (itX != xFNMRsplined.end() && itY != yFNMRsplined.end())
		{
			int i = std::distance(xFNMRsplined.begin(), itX);
			int j = std::distance(yFNMRsplined.begin(), itY);
			setToolTip(QString("(%1, %2%)").arg(x).arg(y));
		}
		else
		{
			setToolTip("");
		}
	}*/
	setToolTip(QString("(%1, %2%)").arg(x).arg(y));
}