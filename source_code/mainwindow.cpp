#include "mainwindow.h"

// Images Declaration
	cv::Mat firstImg, secondImg;
	std::vector<std::tuple<std::string, cv::Mat, cv::Mat>> setImgs;

// Vectors Declaration
	// Segmentation parameters for First and Second Image ...
	cv::Mat firstEnhancedImage, firstSegmentedImage, secondEnhancedImage, secondSegmentedImage;
	std::vector<cv::Mat> setEnhancedImages, setSegmentedImages;
	// Keypoints Vectors for the First & Second Image ...
	std::vector<cv::KeyPoint> firstImgKeypoints, secondImgKeypoints;
	std::vector<std::vector<cv::KeyPoint>> setImgsKeypoints;
	std::vector<std::vector<Minutiae>> setMinutiaes;
	// Descriptors for the First & Second Image ...
	cv::Mat firstImgDescriptors, secondImgDescriptors;
	std::vector<cv::Mat> setImgsDescriptors;
	// Clustering parameters ...
	cv::Mat labels, centers;
	std::vector<std::vector<int>> clusterAffectation;

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
	bool masksAreEmpty=true;
	cv::Mat matchingMask;
	std::vector<cv::Mat> matchingMasks;
	
	//Excel data
	ExcelManager *excelReader, *excelRecover;
	QString RequestedImage;
	
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
	bool oneToN;
	int bestMatchesCount;
	double minKeypoints;
	int takeTestType = -1, importExcelFileType = -1, cpt;
	std::string tests_folderPath = (QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "\\Palmprint Registration").toStdString();
	std::string currentTest_folderPath;
	std::string  settings_filePath = (QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "\\Palmprint Registration\\settings").toStdString();
	QString exportFile = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "\\Palmprint Registration\\palmprint_registration_output.xlsx";
	QString inputFile = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) + "\\Palmprint Registration\\palmprint_registration_input.xlsx";
	int prev_cursor_position;
	std::string language, theme;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	// create Tests if not existes
	std::wstring stemp = std::wstring(tests_folderPath.begin(), tests_folderPath.end());
	if (CreateDirectory(stemp.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()){
		ui->setupUi(this);
		QFile *File;

		// read settings from settings_file
		std::ifstream settings_file(settings_filePath);
		if (settings_file.is_open()){
			settings_file >> cpt;
			settings_file >> language;
			std::getline(settings_file, theme);
			std::getline(settings_file, theme);
			settings_file.close();
		}
		else {
			// new file
			cpt = 0;
			language = "English";
			theme = "Dark Blue";
			// write settings
			std::ofstream settings_file(settings_filePath);
			if (settings_file.is_open()){
				settings_file << cpt << +"\n";
				settings_file << language << +"\n";
				settings_file << theme << +"\n";
				settings_file.close();
			}
			/*std::wstring stemp = std::wstring(settings_filePath.begin(), settings_filePath.end());
			int attr = GetFileAttributes(stemp.c_str());
			if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
				SetFileAttributes(stemp.c_str(), attr | FILE_ATTRIBUTE_HIDDEN);
			}*/
		}
		if (theme != "Light"){
			if (theme == "Dark Orange"){
				File = new QFile(":/themes/dark_orange");
			}
			else File = new QFile(":/themes/dark_blue");
			File->open(QFile::ReadOnly);
			QString StyleSheet = QLatin1String(File->readAll());

			qApp->setStyleSheet(StyleSheet);
		}
		// cusomizing ToolTips :
		//qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white;}");
		ui->descriptorFreakSelectedPairsText->setPlaceholderText("Ex: 1 2 11 22 154 256...");
		this->setWindowIcon(QIcon(":/MainWindow/icon"));
		QPixmap pixmap(":/MainWindow/refresh-img");
		QIcon ButtonIcon(pixmap);
		ui->refreshBddImageNames->setIcon(ButtonIcon);

		if (ui->oneToN->isChecked()) on_refreshBddImageNames_pressed();

		// Control check boxes
		connect(ui->oneToN, &QCheckBox::toggled, [=](bool checked) {
			if (checked){
				//ui->decisionStageThresholdLabel->setEnabled(false);
				//ui->decisionStageThresholdText->setEnabled(false);
				if (ui->matcher1toNtype1->isChecked()) {
					ui->eliminationLoweRatio->setEnabled(false);
					ui->eliminationLoweRatioText->setEnabled(false);
					ui->eliminationInversMatches->setEnabled(false);
					ui->eliminationNoTest->setChecked(true);
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
				//ui->decisionStageThresholdLabel->setEnabled(true);
				//ui->decisionStageThresholdText->setEnabled(true);
				ui->eliminationLoweRatio->setEnabled(true);
				ui->eliminationLoweRatioText->setEnabled(true);
				ui->eliminationInversMatches->setEnabled(true);
			}
			if (!ui->eliminationLoweRatio->isChecked()){
				ui->eliminationLoweRatioText->setEnabled(false);
				ui->matcherBruteForceCrossCheckLabel->setDisabled(ui->eliminationLoweRatio->isChecked() || (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()));
				ui->matcherBruteForceCrossCheckText->setDisabled(ui->eliminationLoweRatio->isChecked() || (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()));
			}
		});
		connect(ui->eliminationLoweRatio, &QCheckBox::toggled, [=](bool checked) {
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
			ui->eliminationNoTest->setChecked(true);
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
	else
	{
		// Failed to create the root.
		ui->logPlainText->appendHtml(tr("<b style='color:red'>Failed to create directory for all tests!</b>"));
		return;
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::runSIFT(int &excelColumn, int testType)
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

	if (testType == 0)
	{
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

void MainWindow::runSURF(int &excelColumn, int testType)
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

	if (testType == 0)
	{
		// Create SURF Objects ...
		ptrDefault = new cv::SURF(hessainThreshold, nOctaves, nOctaveLayers, extended, upright);
		// Matcher
		if (ui->surfBruteForceCheck->isChecked())
		{
			ptrMatcher = new cv::BFMatcher(cv::NORM_L1);
		}
		else
		{
			ptrMatcher = new cv::FlannBasedMatcher();
		}
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

void MainWindow::runORB(int &excelColumn, int testType)
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

	if (testType == 0)
	{
		// Create ORB Object ...
		ptrDefault = new cv::ORB(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize);
		// Matcher
		ptrMatcher = new cv::BFMatcher(cv::NORM_HAMMING);
	}

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
		QMessageBox::critical(this, tr("Error - ORB"), e.what());
	}
}

void MainWindow::runBRISK(int &excelColumn, int testType)
{
	//    BRISK(int thresh = 30, int octaves = 3,float patternScale = 1.0f);

	// Read Parameters ...
	float patternScale = ui->briskPatternScaleText->text().toFloat();
	int octaves = ui->briskOctavesText->text().toInt();
	int thresh = ui->briskThreshText->text().toInt();

	if (testType == 0)
	{
		// Create BRISK Object ...
		ptrDefault = new cv::BRISK(thresh, octaves, patternScale);
		// Matcher
		ptrMatcher = new cv::BFMatcher(cv::NORM_HAMMING);
		/*QModelIndex index = ui->viewTable->model()->index(0, 2);
		ui->logPlainText->appendHtml(ui->viewTable->model()->data(index).toString());*/
		//	ui->logPlainText->appendHtml(ui->viewTable->model()->data(ui->viewTable->model()->index(5, 3)).toString());
	}

	try
	{
		excelReader->SetCellValue(++excelColumn, 0, QString::number(patternScale));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(octaves));
		excelReader->SetCellValue(++excelColumn, 0, QString::number(thresh));
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Error - BRISK"), e.what());
	}
}

void MainWindow::runDefault(int testType)
{
	// Get choices
	int methodIndex = ui->defaultTabs->currentIndex();
	
	if (testType == 0)
	{
		std::string methodName = ui->defaultTabs->tabText(ui->defaultTabs->currentIndex()).toStdString();
		ui->logPlainText->appendHtml(tr("<b>Starting (") + QString::fromStdString(methodName) + tr(") based identification</b> "));
	}

	if (oneToN) {
		setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>()); 
		setImgsDescriptors = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
		goodMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
		badMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
		sumDistancesSet = std::vector<float>(setImgs.size());
		scoreSet = std::vector<float>(setImgs.size());
	}

	int excelColumn;
	try
	{

		if (testType == 0)
		{
			excelReader = new ExcelManager(true, exportFile, methodIndex + 1);
			excelReader->SetCellValue(1, 0, QString::number(cpt));
			excelReader->SetCellValue(2, 0, getCurrentTime());
		}
		else excelReader = new ExcelManager(true, inputFile, methodIndex + 6);

		excelReader->SetCellValue(3 - testType, 0, ui->firstImgText->text(), false);
		excelReader->SetCellValue(4 - testType, 0, ui->secondImgText->text());

		QString one2nImage = ui->oneToN->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(5 - testType, 0, one2nImage);

		if (ui->oneToN->isChecked())
		{
			QString imageExistsInBdd = ui->imageExistsInBdd->isChecked() ? "TRUE" : "FALSE";
			excelReader->SetCellValue(6 - testType, 0, imageExistsInBdd);
			if (ui->imageExistsInBdd->isChecked()) excelReader->SetCellValue(7 - testType, 0, ui->bddImageNames->currentText());
		}
		excelColumn = 7 - testType;
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Error - Excel"), e.what());
	}
	switch (methodIndex) {
		case 0:
			// SIFT
			runSIFT(excelColumn, testType);
			break;

		case 1:
			// SURF
			runSURF(excelColumn, testType);
			break;

		case 2:
			// ORB
			runORB(excelColumn, testType);
			break;

		case 3:
		default:
			// BRISK
			runBRISK(excelColumn, testType);
			break;
	}
	if (testType == 0)
	{
		// Detecting Keypoints ...
		detectionTime = (double)cv::getTickCount();
		ptrDefault->detect(firstImg, firstImgKeypoints);
		if (oneToN){
			for (int i = 0; i < setImgs.size(); i++){
				ptrDefault->detect(std::get<1>(setImgs[i]), setImgsKeypoints[i]);
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
		ui->logPlainText->appendHtml(tr("Detection time: %1(s)").arg(QString::number(detectionTime)));

		// Computing the descriptors
		descriptionTime = (double)cv::getTickCount();
		ptrDefault->compute(firstImg, firstImgKeypoints, firstImgDescriptors);
		if (oneToN){
			for (int i = 0; i < setImgs.size(); i++){
				ptrDefault->compute(std::get<1>(setImgs[i]), setImgsKeypoints[i], setImgsDescriptors[i]);
			}
		}
		else ptrDefault->compute(secondImg, secondImgKeypoints, secondImgDescriptors);
		descriptionTime = ((double)cv::getTickCount() - descriptionTime) / cv::getTickFrequency();
		ui->logPlainText->appendHtml(tr("Description time: %1(s)").arg(QString::number(descriptionTime)));

		// Only the best direct match
		matchingTime = (double)cv::getTickCount();
		if (oneToN){
			for (int i = 0; i < setImgs.size(); i++){
				ptrMatcher->match(firstImgDescriptors, setImgsDescriptors[i], goodMatchesSet[i]);
			}
		}
		else {
			ptrMatcher->match(firstImgDescriptors, secondImgDescriptors, goodMatches);
		}
		matchingTime = ((double)cv::getTickCount() - matchingTime) / cv::getTickFrequency();
		ui->logPlainText->appendHtml(tr("Matching time: %1(s)").arg(QString::number(matchingTime)));

		ui->logPlainText->appendHtml(tr("Total time: %1(s)").arg(QString::number(detectionTime + descriptionTime + clusteringTime + matchingTime)));

		// Scores
		float bestScore = 0;
		if (oneToN){
			for (int i = 0; i < setImgs.size(); i++){
				for (int j = 0; j < goodMatchesSet[i].size(); j++){
					sumDistancesSet[i] += goodMatchesSet[i][j].distance;
				}
				if (goodMatchesSet[i].size() > 0){
					float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size());
					float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
					scoreSet[i] = ui->normalizationOffsetText->text().toFloat() / (average + 1.0) * goodProbability;
					if (scoreSet[i] > 1.0) scoreSet[i] = 1.0;
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
				float goodProbability = static_cast<float>(goodMatches.size()) / static_cast<float>(goodMatches.size());
				score = ui->normalizationOffsetText->text().toFloat() / (average + 1.0) * goodProbability;
				if (score > 1.0) score = 1.0;
			}
			else score = 0.0;
		}
	}

	try{
		excelReader->SetCellValue(excelColumn + 1, 0, ui->normalizationOffsetText->text());
		excelReader->SetCellValue(excelColumn + 2, 0, ui->decisionStageThresholdText->text());

		if (testType == 0)
		{
			excelReader->SetCellValue(excelColumn + 3, 0, QString::number(firstImgKeypoints.size()));
			excelReader->SetCellValue(excelColumn + 5, 0, QString::number(detectionTime) + " (s)");
			excelReader->SetCellValue(excelColumn + 6, 0, QString::number(descriptionTime) + " (s)");
			excelReader->SetCellValue(excelColumn + 7, 0, QString::number(clusteringTime) + " (s)");
			excelReader->SetCellValue(excelColumn + 8, 0, QString::number(matchingTime) + " (s)");
			excelReader->SetCellValue(excelColumn + 9, 0, QString::number(detectionTime + descriptionTime + matchingTime) + " (s)");

			if (!ui->oneToN->isChecked()) {
				excelReader->SetCellValue(excelColumn + 4, 0, QString::number(secondImgKeypoints.size()));
				excelReader->SetCellValue(excelColumn + 10, 0, QString::number(goodMatches.size()));
				excelReader->SetCellValue(excelColumn + 11, 0, QString::number(badMatches.size()));
				excelReader->SetCellValue(excelColumn + 12, 0, QString::number(sumDistances / static_cast<float>(goodMatches.size())));
				excelReader->SetCellValue(excelColumn + 13, 0, QString::number(score));
			}
			else {
				if (bestScoreIndex > -1)
				{
					excelReader->SetCellValue(excelColumn + 4, 0, QString::number(setImgsKeypoints[bestScoreIndex].size()));
					excelReader->SetCellValue(excelColumn + 10, 0, QString::number(goodMatchesSet[bestScoreIndex].size()));
					excelReader->SetCellValue(excelColumn + 11, 0, QString::number(badMatchesSet[bestScoreIndex].size()));
					excelReader->SetCellValue(excelColumn + 12, 0, QString::number(sumDistancesSet[bestScoreIndex] / static_cast<float>(goodMatchesSet[bestScoreIndex].size())));
					excelReader->SetCellValue(excelColumn + 13, 0, QString::number(scoreSet[bestScoreIndex]));
					excelReader->SetCellValue(excelColumn + 15, 0, QString::fromStdString(std::get<0>(setImgs[bestScoreIndex])));
				}
				if (ui->imageExistsInBdd->isChecked() && ui->imageExistsInBdd->isEnabled()) {
					int ff = ui->bddImageNames->currentIndex();
					if (ui->bddImageNames->currentIndex() > -1) excelReader->SetCellValue(excelColumn + 14, 0, QString::number(scoreSet[ui->bddImageNames->currentIndex()]));
					float scoreThreshold = ui->decisionStageThresholdText->text().toFloat();
					excelReader->SetCellValue(excelColumn + 16, 0, QString::number(computeRankK(scoreThreshold)));
				}
			}
		}
		excelReader->~ExcelManager();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Error - Excel"), e.what());
	}
}

void MainWindow::runCustom(int testType)
{
	emit taskPercentageComplete(4);
	// Get choices
	int segmentationIndex = ui->segmentationTabs->currentIndex();
	int detectorIndex = ui->detectorTabs->currentIndex();
	int descriptorIndex = ui->descriptorTabs->currentIndex();
	int matcherIndex = ui->matcherTabs->currentIndex();
	std::string segmentationName = ui->segmentationTabs->tabText(ui->segmentationTabs->currentIndex()).toStdString();
	std::string detectorName = ui->detectorTabs->tabText(ui->detectorTabs->currentIndex()).toStdString();
	std::string descriptorName = ui->descriptorTabs->tabText(ui->descriptorTabs->currentIndex()).toStdString();
	std::string matcherName = ui->matcherTabs->tabText(ui->matcherTabs->currentIndex()).toStdString();
	
	if (testType == 0)
	{
		ui->logPlainText->appendHtml(tr("<b>Starting (") + QString::fromStdString(segmentationName + ", " + detectorName + ", " + descriptorName + ", " + matcherName) + tr(") based identification</b> "));

		// Binarization
		taskProgressDialog->setLabelText(tr("Segmentation ..."));
		emit taskPercentageComplete(5);
		customisingBinarization(segmentationIndex);

		// Customising Segmentor...
		emit taskPercentageComplete(10);
		customisingSegmentor(segmentationIndex);

		// Customising Detector...	
		taskProgressDialog->setLabelText(tr("Detection ..."));
		emit taskPercentageComplete(20);
		customisingDetector(detectorIndex, detectorName);

		// Customising Descriptor...
		taskProgressDialog->setLabelText(tr("Description ..."));
		emit taskPercentageComplete(40);
		customisingDescriptor(descriptorIndex, descriptorName);

		// Clustering Descriptors...
		if (ui->withClusteringChecker->isChecked()){
			taskProgressDialog->setLabelText(tr("Clustering ..."));
			emit taskPercentageComplete(60);
			int nbClusters = ui->clusteringNbClustersText->text().toInt(),
				nbAttempts = ui->clusteringNbAttemptsText->text().toInt();
			if (nbClusters < 1) {
				ui->logPlainText->appendHtml(tr("<b style='color:red'>Invalid number of clusters: %1, the default value is maintained!</b>").arg(QString::number(nbClusters)));
				nbClusters = 10;
			}
			if (nbAttempts < 1) {
				ui->logPlainText->appendHtml(tr("<b style='color:red'>Invalid number of attempts: %1, the default value is maintained!</b>").arg(QString::number(nbAttempts)));
				nbAttempts = 2;
			}
			emit taskPercentageComplete(65);
			clustering(nbClusters, nbAttempts);
			// use masks to matche descriptors from the same cluster
			if (oneToN)for (int i = 0; i < setImgs.size(); i++) maskMatchesByCluster(firstImgKeypoints, setImgsKeypoints[i], i);
			else maskMatchesByCluster(firstImgKeypoints, secondImgKeypoints);
		}

		// Customising Matcher...
		taskProgressDialog->setLabelText(tr("Matching ..."));
		emit taskPercentageComplete(70);
		customisingMatcher(matcherIndex, matcherName);

		// Find the matching points
		emit taskPercentageComplete(75);
		if (!matching()) return;

		ui->logPlainText->appendHtml(tr("Total time: %1(s)").arg(QString::number(detectionTime + descriptionTime + clusteringTime + matchingTime)));

		// Keep only best matching according to the selected test
		taskProgressDialog->setLabelText(tr("Validation ..."));
		emit taskPercentageComplete(90);
		outlierElimination();
	}
	try
	{
		taskProgressDialog->setLabelText(tr("Saving to Excel ..."));
		emit taskPercentageComplete(95);
		if (testType == 0)
		{
			excelReader = new ExcelManager(true, exportFile, 5);
			excelReader->SetCellValue(1, 1, QString::number(cpt));
			excelReader->SetCellValue(2, 1, getCurrentTime());
		}
		else excelReader = new ExcelManager(true, inputFile, 10);

		excelReader->SetCellValue(3 - testType, 1, ui->firstImgText->text(), false);
		excelReader->SetCellValue(4 - testType, 1, ui->secondImgText->text());
		QString one2nImage = ui->oneToN->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(5 - testType, 1, one2nImage);

		if (ui->oneToN->isChecked())
		{
			QString imageExistsInBdd = ui->imageExistsInBdd->isChecked() ? "TRUE" : "FALSE";
			excelReader->SetCellValue(6 - testType, 1, imageExistsInBdd);
			if (ui->imageExistsInBdd->isChecked()) excelReader->SetCellValue(7 - testType, 1, ui->bddImageNames->currentText());
			else excelReader->mergeRowsCells(7 - testType);
		}
		else
		{
			excelReader->mergeRowsCells(6 - testType);
			excelReader->mergeRowsCells(7 - testType);
		}

		excelReader->SetCellValue(8 - testType, 1, QString::fromStdString(segmentationName));

		excelReader->SetCellValue(9 - testType, 2, ui->segmentationThresholdLabel->text());
		excelReader->SetCellValueSecondRow(9 - testType, ui->segmentationThresholdText->text());

		excelReader->SetCellValue(10 - testType, 1, QString::fromStdString(detectorName));
		switch (detectorIndex){
			case 0:{
				// Minutiae-detection using Crossing Number By Dr. Faiçal
				excelReader->SetCellValue(11 - testType, 2, ui->detectorMinutiaeLimitDistanceLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorMinutiaeLimitDistanceText->text());
				excelReader->mergeCellsCustom(12 - testType, 15 - testType);
				break;
			}
			case 1:{
				// Minutiae-detection using Crossing Number
				excelReader->SetCellValue(11 - testType, 2, ui->detectorCrossingNumberLimitDistanceLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorCrossingNumberLimitDistanceText->text());
				excelReader->SetCellValue(12 - testType, 2, ui->detectorCrossingNumberBorderLabel->text());
				excelReader->SetCellValueSecondRow(12 - testType, ui->detectorCrossingNumberBorderText->text());
				excelReader->mergeCellsCustom(13 - testType, 15 - testType);
				break;
			}
			case 2:{
				// Harris-Corners
				excelReader->SetCellValue(11 - testType, 2, ui->detectorHarrisThresholdLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorHarrisThresholdText->text());
				excelReader->mergeCellsCustom(12 - testType, 15 - testType);
				break;
			}
			case 3:{
				// STAR
				excelReader->SetCellValue(11 - testType, 2, ui->detectorStarMaxSizeLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorStarMaxSizeText->text());
				excelReader->SetCellValue(12 - testType, 2, ui->detectorStarResponseThresholdLabel->text());
				excelReader->SetCellValueSecondRow(12 - testType, ui->detectorStarResponseThresholdText->text());
				excelReader->SetCellValue(13 - testType, 2, ui->detectorStarLineThresholdProjectedLabel->text());
				excelReader->SetCellValueSecondRow(13 - testType, ui->detectorStarThresholdProjectedText->text());
				excelReader->SetCellValue(14 - testType, 2, ui->detectorStarLineThresholdBinarizedLabel->text());
				excelReader->SetCellValueSecondRow(14 - testType, ui->detectorStarThresholdBinarizedText->text());
				excelReader->SetCellValue(15 - testType, 2, ui->detectorStarSuppressNonmaxSizeLabel->text());
				excelReader->SetCellValueSecondRow(15 - testType, ui->detectorStarSuppressNonmaxSizeText->text());
				break;
			}
			case 4: {
				// FAST
				QString detectorFastNonmaxSuppressionCheck = ui->detectorFastNonmaxSuppressionCheck->isChecked() ? "TRUE" : "FALSE";
				QString detectorFastXCheck = ui->detectorFastXCheck->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(11 - testType, 2, ui->detectorFastThresholdLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorFastThresholdText->text());
				excelReader->SetCellValue(12 - testType, 2, ui->detectorFastNonmaxSuppressionLabel->text());
				excelReader->SetCellValueSecondRow(12 - testType, detectorFastNonmaxSuppressionCheck);
				excelReader->SetCellValue(13 - testType, 2, ui->detectorFastXLabel->text());
				excelReader->SetCellValueSecondRow(13 - testType, detectorFastXCheck);
				if (ui->detectorFastXCheck->isChecked())
				{
					excelReader->SetCellValue(14 - testType, 2, ui->detectorFastTypeLabel->text());
					excelReader->SetCellValueSecondRow(14 - testType, ui->detectorFastTypeText->currentText());
					excelReader->mergeRowsCells(15 - testType);
				}
				else excelReader->mergeCellsCustom(14 - testType, 15 - testType);
				break;
			}
			case 5: {
				// SIFT
				excelReader->SetCellValue(11 - testType, 2, ui->detectorSiftContrastThresholdLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorSiftContrastThresholdText->text());
				excelReader->SetCellValue(12 - testType, 2, ui->detectorSiftEdgeThresholdLab->text());
				excelReader->SetCellValueSecondRow(12 - testType, ui->detectorSiftEdgeThresholdText->text());
				excelReader->SetCellValue(13 - testType, 2, ui->detectorSiftNfeaturesLabel->text());
				excelReader->SetCellValueSecondRow(13 - testType, ui->detectorSiftNfeaturesText->text());
				excelReader->SetCellValue(14 - testType, 2, ui->detectorSiftNOctaveLayersLabel->text());
				excelReader->SetCellValueSecondRow(14 - testType, ui->detectorSiftNOctaveLayersText->text());
				excelReader->SetCellValue(15 - testType, 2, ui->detectorSiftSigmaLabel->text());
				excelReader->SetCellValueSecondRow(15 - testType, ui->detectorSiftSigmaText->text());
				break;
			}
			case 6: {
				//SURF
				QString detectorSurfUprightText = ui->detectorSurfUprightText->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(11 - testType, 2, ui->detectorSurfHessianThresholdLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorSurfHessianThresholdText->text());
				excelReader->SetCellValue(12 - testType, 2, ui->detectorSurfNOctavesLabel->text());
				excelReader->SetCellValueSecondRow(12 - testType, ui->detectorSurfNOctavesText->text());
				excelReader->SetCellValue(13 - testType, 2, ui->detectorSurfNLayersLabel->text());
				excelReader->SetCellValueSecondRow(13 - testType, ui->detectorSurfNLayersText->text());
				excelReader->SetCellValue(14 - testType, 2, ui->detectorSurfUprightLabel->text());
				excelReader->SetCellValueSecondRow(14 - testType, detectorSurfUprightText);
				excelReader->mergeRowsCells(15 - testType);
				break;
			}
			case 7: {
				//Dense
				excelReader->SetCellValue(11 - testType, 2, ui->detectorDenseInitFeatureScaleLabel->text());
				excelReader->SetCellValueSecondRow(11 - testType, ui->detectorDenseInitFeatureScaleText->text());
				excelReader->SetCellValue(12 - testType, 2, ui->detectorDenseFeatureScaleLevelsLabel->text());
				excelReader->SetCellValueSecondRow(12 - testType, ui->detectorDenseFeatureScaleLevelsText->text());
				excelReader->SetCellValue(13 - testType, 2, ui->detectorDenseFeatureScaleMulLabel->text());
				excelReader->SetCellValueSecondRow(13 - testType, ui->detectorDenseFeatureScaleMulText->text());
				excelReader->SetCellValue(14 - testType, 2, ui->detectorDenseInitXyStepLabel->text());
				excelReader->SetCellValueSecondRow(14 - testType, ui->detectorDenseInitXyStepText->text());
				excelReader->SetCellValue(15 - testType, 2, ui->detectorDenseInitImgBoundLabel->text());
				excelReader->SetCellValueSecondRow(15 - testType, ui->detectorDenseInitImgBoundText->text());
				break;
			}
		}

		excelReader->SetCellValue(16 - testType, 1, QString::fromStdString(descriptorName));
		switch (descriptorIndex)
		{
			case 0:
			{
				// FREAK
				QString descriptorFreakOrientationNormalizedCheck = ui->descriptorFreakOrientationNormalizedCheck->isChecked() ? "TRUE" : "FALSE";
				QString descriptorFreakScaleNormalizedCheck = ui->descriptorFreakScaleNormalizedCheck->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(17 - testType, 2, ui->descriptorFreakOrientationNormalizedLabel->text());
				excelReader->SetCellValueSecondRow(17 - testType, descriptorFreakOrientationNormalizedCheck);
				excelReader->SetCellValue(18 - testType, 2, ui->descriptorFreakScaleNormalizedLabel->text());
				excelReader->SetCellValueSecondRow(18 - testType, descriptorFreakScaleNormalizedCheck);
				excelReader->SetCellValue(19 - testType, 2, ui->descriptorFreakPatternScaleLabel->text());
				excelReader->SetCellValueSecondRow(19 - testType, ui->descriptorFreakPatternScaleText->text());
				excelReader->SetCellValue(20 - testType, 2, ui->descriptorFreakNOctavesLabel->text());
				excelReader->SetCellValueSecondRow(20 - testType, ui->descriptorFreakNOctavesText->text());
				excelReader->SetCellValue(21 - testType, 2, ui->descriptorFreakNelectedPairsLabel->text());
				excelReader->SetCellValueSecondRow(21 - testType, ui->descriptorFreakSelectedPairsText->text());
				break;
			}
			case 1:
			{
				// BRIEF
				excelReader->SetCellValue(17 - testType, 2, ui->descriptorBriefPATCH_SIZELabel->text());
				excelReader->SetCellValueSecondRow(17 - testType, ui->descriptorBriefPATCH_SIZEText->text());
				excelReader->SetCellValue(18 - testType, 2, ui->descriptorBriefKERNEL_SIZELabel->text());
				excelReader->SetCellValueSecondRow(18 - testType, ui->descriptorBriefKERNEL_SIZEText->text());
				excelReader->SetCellValue(19 - testType, 2, ui->descriptorBriefLengthLabel->text());
				excelReader->SetCellValueSecondRow(19 - testType, ui->descriptorBriefLengthText->text());
				excelReader->mergeCellsCustom(20 - testType, 21 - testType);
				break;
			}
			case 2:
			{
				// SIFT
				excelReader->SetCellValue(17 - testType, 2, ui->descriptorSiftLengthLabel->text());
				excelReader->SetCellValueSecondRow(17 - testType, ui->descriptorSiftLengthText->text());
				excelReader->mergeCellsCustom(18 - testType, 21 - testType);
				break;
			}
			case 3:
			{
				//SURF
				QString descriptorSurfExtended = ui->descriptorSurfExtended->isChecked() ? "128" : "64";

				excelReader->SetCellValue(17 - testType, 2, ui->descriptorSurfExtendedLabel->text());
				excelReader->SetCellValueSecondRow(17 - testType, descriptorSurfExtended);
				excelReader->mergeCellsCustom(18 - testType, 21 - testType);
				break;
			}
		}

		excelReader->SetCellValue(22 - testType, 1, QString::fromStdString(matcherName));

		switch (matcherIndex)
		{
			case 0:
			{
				// BruteForce
				QString matcherBruteForceCrossCheckText = ui->matcherBruteForceCrossCheckText->isChecked() ? "TRUE" : "FALSE";

				excelReader->SetCellValue(23 - testType, 2, ui->matcherBruteForceNormTypeLabel->text());
				excelReader->SetCellValueSecondRow(23 - testType, ui->matcherBruteForceNormTypeText->currentText());
				excelReader->SetCellValue(24 - testType, 2, ui->matcherBruteForceCrossCheckLabel->text());
				excelReader->SetCellValueSecondRow(24 - testType, matcherBruteForceCrossCheckText);
				break;
			}
			case 1:
			{
				// FlannBased
				excelReader->SetCellValue(23 - testType, 2, ui->matcherFlannBasedIndexParamsLabel->text());
				excelReader->SetCellValueSecondRow(23 - testType, getFlannBasedNameParamsType());
				excelReader->SetCellValue(24 - testType, 2, ui->matcherFlannBasedSearchParamsLabel->text());
				excelReader->SetCellValueSecondRow(24 - testType, ui->matcherFlannBasedSearchParamsText->text());
				break;
			}
		}

		QString matcher1toNtype = ui->matcher1toNtype1->isChecked() ? ui->matcher1toNtype1->text() : ui->matcher1toNtype2->text();
		excelReader->SetCellValue(25 - testType, 2, ui->matcher1toNtypeLabel->text());
		excelReader->SetCellValueSecondRow(25 - testType, matcher1toNtype);

		QString withClusteringChecker = ui->withClusteringChecker->isChecked() ? "TRUE" : "FALSE";

		excelReader->SetCellValue(26 - testType, 2, ui->withClusteringChecker->text());
		excelReader->SetCellValueSecondRow(26 - testType, withClusteringChecker);

		if (ui->withClusteringChecker->isChecked()) {
			
			excelReader->SetCellValue(27 - testType, 2, ui->clusteringNbClustersLabel->text());
			excelReader->SetCellValueSecondRow(27 - testType, ui->clusteringNbClustersText->text());

			excelReader->SetCellValue(28 - testType, 2, ui->clusteringNbAttemptsLabel->text());
			excelReader->SetCellValueSecondRow(28 - testType, ui->clusteringNbAttemptsText->text());

			if (testType == 0) excelReader->SetCellValue(38, 1, QString::number(clusteringTime) + " (s)");
		}
		else
		{
			excelReader->mergeRowsCells(27 - testType);
			excelReader->mergeRowsCells(28 - testType);
			if (testType == 0) excelReader->mergeRowsCells(38 - testType);
		}
		
		excelReader->SetCellValue(29 - testType, 2, ui->customTabs->tabText(4));
		if (ui->eliminationNoTest->isChecked()) excelReader->SetCellValueSecondRow(29 - testType, ui->eliminationNoTest->text());
		else if (ui->eliminationInversMatches->isChecked()) excelReader->SetCellValueSecondRow(29 - testType, ui->eliminationInversMatches->text());
		else excelReader->SetCellValueSecondRow(29 - testType, ui->eliminationLoweRatio->text() + ":" + ui->eliminationLoweRatioText->text());

		if (ui->eliminationLimitDistance->isChecked()) {
			excelReader->SetCellValue(30 - testType, 2, ui->eliminationLimitDistance->text());
			excelReader->SetCellValueSecondRow(30 - testType, ui->eliminationLimitDistanceText->text());
		}
		else excelReader->mergeRowsCells(30 - testType);

		QString opponentColor = ui->opponentColor->isChecked() ? "TRUE" : "FALSE";
		excelReader->SetCellValue(31 - testType, 1, opponentColor);
		
		excelReader->SetCellValue(32 - testType, 1, ui->normalizationOffsetText->text());

		excelReader->SetCellValue(33 - testType, 1, ui->decisionStageThresholdText->text());

		if (testType == 0)
		{
			excelReader->SetCellValue(34, 1, QString::number(firstImgKeypoints.size()));

			excelReader->SetCellValue(36, 1, QString::number(detectionTime) + " (s)");
			excelReader->SetCellValue(37, 1, QString::number(descriptionTime) + " (s)");
			excelReader->SetCellValue(39, 1, QString::number(matchingTime) + " (s)");
			excelReader->SetCellValue(40, 1, QString::number(detectionTime + descriptionTime + clusteringTime + matchingTime) + " (s)");

			if (!ui->oneToN->isChecked()) {
				excelReader->SetCellValue(35, 1, QString::number(secondImgKeypoints.size()));
				excelReader->SetCellValue(41, 1, QString::number(goodMatches.size()));
				excelReader->SetCellValue(42, 1, QString::number(badMatches.size()));
				excelReader->SetCellValue(43, 1, QString::number(sumDistances / static_cast<float>(goodMatches.size())));
				excelReader->SetCellValue(44, 1, QString::number(score));
				excelReader->mergeRowsCells(45);
				excelReader->mergeRowsCells(46);
				excelReader->mergeRowsCells(47);
			}
			else {
				if (bestScoreIndex > -1)
				{
					excelReader->SetCellValue(35, 1, QString::number(setImgsKeypoints[bestScoreIndex].size()));
					excelReader->SetCellValue(41, 1, QString::number(goodMatchesSet[bestScoreIndex].size()));
					excelReader->SetCellValue(42, 1, QString::number(badMatchesSet[bestScoreIndex].size()));
					excelReader->SetCellValue(43, 1, QString::number(sumDistancesSet[bestScoreIndex] / static_cast<float>(goodMatchesSet[bestScoreIndex].size())));
					excelReader->SetCellValue(44, 1, QString::number(scoreSet[bestScoreIndex]));
					excelReader->SetCellValue(46, 1, QString::fromStdString(std::get<0>(setImgs[bestScoreIndex])));
				}
				else
				{
					excelReader->mergeRowsCells(35);
					excelReader->mergeRowsCells(41);
					excelReader->mergeRowsCells(42);
					excelReader->mergeRowsCells(43);
					excelReader->mergeRowsCells(44);
					excelReader->mergeRowsCells(46);
				}
				if (ui->imageExistsInBdd->isChecked() && ui->imageExistsInBdd->isEnabled()) {
					if (ui->bddImageNames->currentIndex() > -1) excelReader->SetCellValue(45, 1, QString::number(scoreSet[ui->bddImageNames->currentIndex()]));
					else excelReader->mergeRowsCells(45);
					float scoreThreshold = ui->decisionStageThresholdText->text().toFloat();
					excelReader->SetCellValue(47, 1, QString::number(computeRankK(scoreThreshold)));
				}
				else
				{
					excelReader->mergeRowsCells(45);
					excelReader->mergeRowsCells(47);
				}
			}
		}
		excelReader->~ExcelManager();
		emit taskPercentageComplete(97);
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Error - Custom"), e.what());
	}
}

void MainWindow::launchInCMD(){
	ui->tabWidget_2->setCurrentIndex(2);
	//ui->inputPath->setText(filePath);
	on_actionRun_triggered();
}

void MainWindow::on_firstImgBtn_pressed()
{
	// Read First Image ...
	QString str = QFileDialog::getOpenFileName(0, tr("Select the 1st Image"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->firstImgText->setText(str);
}

void MainWindow::on_secondImgBtn_pressed()
{
	// Read Second Image(s) ...
	QString str = (ui->oneToN->isChecked()) ? QFileDialog::getExistingDirectory(0, tr("Select a Folder"), QDir::currentPath()) : QFileDialog::getOpenFileName(0, tr("Select the 2nd Image"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->secondImgText->setText(str);
}

/*void MainWindow::on_inputBrowse_pressed()
{
	// Read Input File ...
	QString str = QFileDialog::getOpenFileName(0, ("Select the input file"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->inputPath->setText(str);
}*/

void MainWindow::on_refreshBddImageNames_pressed()
{
	// Reload image names ...
	if (!ui->secondImgText->text().trimmed().isEmpty()){
		readSetOfImages();
	}
}

void MainWindow::on_actionSettings_triggered()
{
	QUiLoader loader;
	QFile file(":/MainWindow/settings-dialog");
	file.open(QFile::ReadOnly);
	QDialog *settingsDialog = qobject_cast<QDialog*>(loader.load(&file, this));
	file.close();

	try{
		settingsDialog->findChild<QComboBox*>("languageComboBox")->setCurrentText(QString::fromStdString(language));
		settingsDialog->findChild<QComboBox*>("themeComboBox")->setCurrentText(QString::fromStdString(theme));
	}
	catch (...){}

	if (settingsDialog->exec() == QDialog::Accepted){
		language = settingsDialog->findChild<QComboBox*>("languageComboBox")->currentText().toStdString();
		theme = settingsDialog->findChild<QComboBox*>("themeComboBox")->currentText().toStdString();
		/*...then open it in the appropriate way*/
		std::ofstream settings_file(settings_filePath);
		if (settings_file.is_open())
		{
			settings_file << cpt << +"\n";
			settings_file << language << +"\n";
			settings_file << theme << +"\n";
			settings_file.close();
		}
	}
}

void MainWindow::on_actionRun_triggered()
{
	QThread* taskThread = new QThread();
	disconnect(taskThread, SIGNAL(started()), this, SLOT(takeTest()));
	disconnect(taskThread, SIGNAL(started()), this, SLOT(importExcelFile()));
	disconnect(taskThread, SIGNAL(finished()), taskProgressDialog, SLOT(deletLater()));
	disconnect(this, SIGNAL(taskPercentageComplete(int)), taskProgressDialog, SLOT(setValue(int)));

	this->moveToThread(taskThread);
	taskProgressDialog = new QProgressDialog(tr("Processing ..."), "Cancel", 0, 100, this);
	taskProgressDialog->setMinimumWidth(300);
	taskProgressDialog->setWindowModality(Qt::WindowModal);
	taskProgressDialog->setMinimumDuration(0);
	taskProgressDialog->setValue(0);

	connect(this, SIGNAL(taskPercentageComplete(int)), taskProgressDialog, SLOT(setValue(int)));
	connect(taskThread, SIGNAL(finished()), taskProgressDialog, SLOT(deletLater()));

	switch (ui->tabWidget_2->currentIndex())
	{
		case 0:
		{
			takeTestType = 0;
			taskProgressDialog->setWindowTitle(tr("New Test"));
			connect(taskThread, SIGNAL(started()), this, SLOT(takeTest()));
			break;
		}
		case 1:
		{
			importExcelFileType = 0;
			taskProgressDialog->setWindowTitle(tr("Import Test"));
			connect(taskThread, SIGNAL(started()), this, SLOT(importExcelFile()));
			break;
		}
		case 2:
		{
			/*if (!readInputFile()) {
			ui->logPlainText->appendHtml(tr("<b style='color:red'>Error while trying to read the excel file!</b>"));
			return;
			}*/
			importExcelFileType = 2;
			taskProgressDialog->setWindowTitle(tr("Execute Commands"));
			connect(taskThread, SIGNAL(started()), this, SLOT(importExcelFile()));
		}
		break;
	}
	taskThread->start();
}

void MainWindow::on_actionClear_Log_triggered()
{
    ui->logPlainText->clear();
}

void MainWindow::on_actionAdd_Command_triggered()
{
	takeTestType = 2;
	takeTest();
}

void MainWindow::on_actionDelete_All_Commands_triggered()
{
	int rowsCount = 0;
	excelRecover = new ExcelManager(true, inputFile, 11);

	for (int j = 1; j <= 5; j++)
	{	
		excelRecover->GetIntRows(j);
		for (int i = 2; i <= excelRecover->getSheetCount(); i++)
		{
			excelRecover->DeleteRow();
			if (j < 5) rowsCount++;
			else if (/*(i > 2) &&*/ (i%2 == 0)) rowsCount++;
		}
	}

	excelRecover->~ExcelManager();
	if (rowsCount > 0) QMessageBox::information(this, tr("Delete All Commands"), tr("All commands (") + QString::number(rowsCount) + tr(" commands) have been deleted with success from input file !"));
	else QMessageBox::warning(this, tr("Delete All Commands"), tr("There is no commands in the input file (Probably you haven't add a new command or you have just deleted them all) !"));
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionAbout_Me_triggered()
{
    QMessageBox::about(this,
                       tr("About Us"),
					   tr("<b>OpenCV Feature Detectors Comparison</b>"
                       "<br><br>This program uses OpenCV and Qt, and is provided as is, for educational purposes such as benchmarking of algorithms.<br>"
                       "<br>You may contact me for the source code of this program at <a href='mailto:dn_ghouila@esi.dz'>dn_ghouila@esi.dz</a> or <a href='mailto:da_belkaid@esi.dz'>da_belkaid@esi.dz</a>"
                       "<br><br>Thanks"
                       "<br><br>GHOUILA Nabil & BELKAID Aïssa"
					   "<br><br><a href='mailto:da_belkaid@esi.dz'>da_belkaid@esi.dz</a>"
					   "<br><br><a href='mailto:dn_ghouila@esi.dz'>dn_ghouila@esi.dz</a>"));
}

void MainWindow::showEvent(QShowEvent* event) {
	QWidget::showEvent(event);
	if (language == "French" || language == "Français") ui->retranslateUi(this);
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
				detectorName = detectorNameToInt(excelRecover->GetCellValue(j, 10).toString());
				descriptorName = descriptorNameToInt(excelRecover->GetCellValue(j, 16).toString());
				matcherName = matcherNameToInt(excelRecover->GetCellValue(j, 22).toString());
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
		//ui->rankkGraphWidget->clearGraphs();
		//ui->rankkGraphWidget->clearItems();
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
		QMessageBox::warning(this, tr("Show Rank-k Graph"), tr("No data to show! You can Show Rank-k Graph after launching some tests!"));
		//disconnect(ui->rankkGraphWidget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showRankkToolTip(QMouseEvent*)));
	}
}

void MainWindow::on_refreshEerGraph_pressed(){
	std::map<float, std::pair<int, int>>/*<threshold, <nbFM, nbNonExists>>*/ FMR_dataFromExcel = {};
	std::map<float, std::pair<int, int>>::iterator FMR_itr = {};

	std::map<float, std::pair<int, int>>/*<threshold, <nbFNM, nbExists>>*/ FNMR_dataFromExcel = {};
	std::map<float, std::pair<int, int>>::iterator FNMR_itr = {};
		
	int column, segmentationName, detectorName, descriptorName, matcherName;
	QString bestScore, requestedScore;
	float scoreThreshold;

	excelRecover = new ExcelManager(true, exportFile, 0);
	if (ui->allMethodsTabs->currentIndex() == 0) excelRecover->GetIntRows(ui->defaultTabs->currentIndex() + 1);
	else excelRecover->GetIntRows(5);
	column = excelRecover->getColumnsCount();

	for (float scoreThreshold = 0; scoreThreshold <= 1; scoreThreshold += ui->thresholdStepText->text().toFloat()) {
		for (int j = 2; j <= excelRecover->getSheetCount(); j++) {

			//scoreThreshold = excelRecover->GetCellValue(j, column - 14).toString();

			if (excelRecover->GetCellValue(j, 5).toBool())
			{

				if (ui->allMethodsTabs->currentIndex() == 0)
				{
					if (excelRecover->GetCellValue(j, 6).toBool())
					{
						FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold);
						if (FNMR_itr == FNMR_dataFromExcel.end()) FNMR_dataFromExcel.insert(std::make_pair(scoreThreshold, std::make_pair(0, 1)));
						else FNMR_itr->second.second = FNMR_itr->second.second + 1;

						requestedScore = excelRecover->GetCellValue(j, column - 2).toString();
						if ((!requestedScore.isEmpty()))
							if (requestedScore.toFloat() < scoreThreshold)
							{
								FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold);
								FNMR_itr->second.first = FNMR_itr->second.first + 1;
							}
					}
					else
					{
						FMR_itr = FMR_dataFromExcel.find(scoreThreshold);
						if (FMR_itr == FMR_dataFromExcel.end()) FMR_dataFromExcel.insert(std::make_pair(scoreThreshold, std::make_pair(0, 1)));
						else FMR_itr->second.second = FMR_itr->second.second + 1;

						bestScore = excelRecover->GetCellValue(j, column - 3).toString();
						if ((!bestScore.isEmpty()))
							if (bestScore.toFloat() >= scoreThreshold)
							{
								FMR_itr = FMR_dataFromExcel.find(scoreThreshold);
								FMR_itr->second.first = FMR_itr->second.first + 1;
							}
					}

				}
				else
				{
					segmentationName = segmentationNameToInt(excelRecover->GetCellValue(j, 8).toString());
					detectorName = detectorNameToInt(excelRecover->GetCellValue(j, 10).toString());
					descriptorName = descriptorNameToInt(excelRecover->GetCellValue(j, 16).toString());
					matcherName = matcherNameToInt(excelRecover->GetCellValue(j, 22).toString());

					if ((ui->segmentationTabs->currentIndex() == segmentationName) && (ui->detectorTabs->currentIndex() == detectorName) && (ui->descriptorTabs->currentIndex() == descriptorName) && (ui->matcherTabs->currentIndex() == matcherName))
					{
						if (excelRecover->GetCellValue(j, 6).toBool())
						{
							FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold);
							if (FNMR_itr == FNMR_dataFromExcel.end()) FNMR_dataFromExcel.insert(std::make_pair(scoreThreshold, std::make_pair(0, 1)));
							else FNMR_itr->second.second = FNMR_itr->second.second + 1;

							requestedScore = excelRecover->GetCellValue(j, column - 2).toString();
							if ((!requestedScore.isEmpty()))
								if (requestedScore.toFloat() < scoreThreshold)
								{
									FNMR_itr = FNMR_dataFromExcel.find(scoreThreshold);
									FNMR_itr->second.first = FNMR_itr->second.first + 1;
								}
						}
						else
						{
							FMR_itr = FMR_dataFromExcel.find(scoreThreshold);
							if (FMR_itr == FMR_dataFromExcel.end()) FMR_dataFromExcel.insert(std::make_pair(scoreThreshold, std::make_pair(0, 1)));
							else FMR_itr->second.second = FMR_itr->second.second + 1;

							bestScore = excelRecover->GetCellValue(j, column - 3).toString();
							if ((!bestScore.isEmpty()))
								if (bestScore.toFloat() >= scoreThreshold)
								{
									FMR_itr = FMR_dataFromExcel.find(scoreThreshold);
									FMR_itr->second.first = FMR_itr->second.first + 1;
								}
						}
					}
					j++;
				}
			}
		}
	}
	excelRecover->~ExcelManager();

	if (ui->eerGraphWidget->graphCount()){
		//ui->eerGraphWidget->clearGraphs();
		//ui->eerGraphWidget->clearItems();
	}

	if (FMR_dataFromExcel.empty() && FNMR_dataFromExcel.empty()) QMessageBox::warning(this, tr("Show EER Graph"), tr("No data to show! You can Show EER Graph after having some FM and FNM tests !"));
	else try{
		drowEer(FMR_dataFromExcel, FNMR_dataFromExcel);
	} catch (...){}
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

/*bool MainWindow::readInputFile(){
	
	inputFile = ui->inputPath->text().replace("/", "\\");

	// Check if the Path is loaded correctly ...
	return (!inputFile.isEmpty());
}*/

bool MainWindow::readSetOfImages(bool import){
	setImgs.clear();
	int savedIndex = ui->bddImageNames->currentIndex();
	ui->bddImageNames->clear();
	// Read Data Set of Images ...
	std::wstring wdatapath = ui->secondImgText->text().toStdWString();
	
	tinydir_dir dir;
	tinydir_open(&dir, (const TCHAR*)wdatapath.c_str());

	if (!ui->secondImgText->text().trimmed().isEmpty()) /* nothing but whitespace */{
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
				if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 1)){
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
					setImgs.push_back(std::make_tuple(filename, img, img));

					// show names in the combobox
					ui->bddImageNames->addItem(QString::fromStdString(filename));
					if (import && (QString::fromStdString(filename) == RequestedImage)) savedIndex = setImgs.size() - 1;
				}
			}
			try{ tinydir_next(&dir); }
			catch (...){}
		}
		try{ tinydir_close(&dir); }
		catch (...){}
	}
	if (setImgs.size() == 0) {
		return false;
	}
	else if (setImgs.size() == 1){
		// treat as one to one image
		secondImg = std::get<1>(setImgs[setImgs.size() - 1]);
		oneToN = false;
	}
	ui->bddImageNames->setCurrentIndex(savedIndex);
	return true;
}

bool MainWindow::createTestFolder(){
	// create a new folder test
	currentTest_folderPath = tests_folderPath + "\\"+ std::to_string(cpt+1);
	std::wstring stemp = std::wstring(currentTest_folderPath.begin(), currentTest_folderPath.end());
	if (CreateDirectory(stemp.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()){
		// created with succes
		// Rest parameters :
		resetParams();
		currentTest_folderPath += "\\";
		if (oneToN && setImgs.size() > 1){
			// Create a sub directory for all output matches
			std::string temp = currentTest_folderPath + "all matches";
			std::wstring stemp = std::wstring(temp.begin(), temp.end());
			if (CreateDirectory(stemp.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()){
				// success
			}
			else {
				showError(tr("Creating directory").toStdString(), tr("Cannot create a directory to store matches").toStdString(), tr("Make sure that ").toStdString() + currentTest_folderPath + tr(" existes").toStdString());
			}
		}

		// update the next file
		cpt++;
		std::ofstream settings_file(settings_filePath);
		if (settings_file.is_open()) {
			settings_file << cpt << "\n";
			settings_file << language << +"\n";
			settings_file << theme << +"\n";
			settings_file.close();
		}
		return true;
	}
	else
	{
		// Failed to create directory.
		showError(tr("Creating directory").toStdString(), tr("Failed to create directory for the current test").toStdString(), tr("Make sure that ").toStdString() + tests_folderPath + tr(" existes").toStdString());
		return false;
	}
}

bool MainWindow::noKeyPoints(std::string rank, std::vector<cv::KeyPoint> imgKeypoints)
{
	ui->logPlainText->appendHtml(tr("Found %1 key points in the %2 image").arg(QString::number(imgKeypoints.size())).arg(QString::fromStdString(rank)));

	if (imgKeypoints.size() <= 0)
	{
		ui->logPlainText->appendHtml(tr("Point matching can not be done because no key points detected in the %1 image!").arg(QString::fromStdString(rank)));
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
	bestScoreIndex = -1; masksAreEmpty = true;
	try{
		firstImgKeypoints.clear(); secondImgKeypoints.clear(); setImgsKeypoints.clear(); setMinutiaes.clear();
		firstImgDescriptors.release(); secondImgDescriptors.release(); setImgsDescriptors.clear();
		firstEnhancedImage.release(); secondEnhancedImage.release(); setEnhancedImages.clear();
		firstSegmentedImage.release(); secondSegmentedImage.release(); setSegmentedImages.clear();
		directMatches.clear(); inverseMatches.clear(); twoMatches.clear(); goodMatches.clear(); badMatches.clear();
		directMatchesSet.clear(); inverseMatchesSet.clear(); twoMatchesSet.clear(); goodMatchesSet.clear(); badMatchesSet.clear();
		sumDistancesSet.clear(); scoreSet.clear();
		matchingMask.release(); matchingMasks.clear();
		clusterAffectation.clear();
	}
	catch (...){
		ui->logPlainText->appendHtml(tr("<b style='color:yellow'>Unable to free some structures!</b>"));
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

double MainWindow::kMeans(std::vector<cv::Mat> features_vector, int k, int attempts){
	// K : The number of clusters to split the samples in rawFeatureData (retrieved from 'OpenCV 3 Blueprints' book)
	int nbRows = firstImgDescriptors.rows;
	if (oneToN){
		for (cv::Mat descriptor : setImgsDescriptors)
		{
			nbRows += descriptor.rows;
		}
	}
	else nbRows += secondImgDescriptors.rows;
	cv::Mat rawFeatureData = cv::Mat::zeros(nbRows, firstImgDescriptors.cols, CV_32FC1);
	// We need to copy the data from the vector of key points features_vector to imageFeatureData:
	int cur_idx = 0;
	for (int i = 0; i < features_vector.size(); i++){
		features_vector[i].copyTo(rawFeatureData.rowRange(cur_idx, cur_idx + features_vector[i].rows));
		cur_idx += features_vector[i].rows;
	}
	double compactness; //This is the sum of the squared distance between each sample to the corresponding centroid;
	compactness = cv::kmeans(rawFeatureData, k, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 1.0),
		attempts, cv::KMEANS_PP_CENTERS, centers);
	return compactness;
}
// <--------------
void MainWindow::clustering(int nbClusters, int nbAttempts){
	// use kmeans algorithme to cluster descriptors
	clusteringTime = (double)cv::getTickCount();
	std::vector<cv::Mat> matrix;
	if (oneToN) {
		matrix = { firstImgDescriptors };
		matrix.insert(matrix.end(), setImgsDescriptors.begin(), setImgsDescriptors.end());
	}
	else matrix = { firstImgDescriptors, secondImgDescriptors };
	try{
		double sumSquaredDistances = kMeans(matrix, nbClusters, nbAttempts);
		int k = firstImgKeypoints.size();
		if (oneToN){
			clusterAffectation = std::vector<std::vector<int>>(setImgs.size(), std::vector<int>());
			for (int i = 0; i < setImgs.size(); i++){
				for (int j = 0; j < setImgsKeypoints[i].size(); j++){
					clusterAffectation[i].push_back(labels.at<int>(k));
					k++;
				}
			}
		}
	}
	catch (...){
		ui->logPlainText->appendHtml(tr("<i style='color:red'>The number of clusters is too high, try another!</i>"));
		return;
	}
	// Write Kmeans parameters
	/*cv::FileStorage fs("params/kmeans_params.yaml", cv::FileStorage::WRITE);
	fs << "Labels" << labels;
	fs << "Centers" << centers;
	fs.release();*/
	clusteringTime = ((double)cv::getTickCount() - clusteringTime) / cv::getTickFrequency();
	ui->logPlainText->appendHtml(tr("Clustering time: %1(s)").arg(QString::number(clusteringTime)));
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
}

void MainWindow::displayFeature(cv::Mat featureMat, int first_second)
{
	QGraphicsScene *featureScene = new QGraphicsScene();
	//QImage featureImg((const uchar *)featureMat.data, featureMat.cols, featureMat.rows, featureMat.step, QImage::Format_RGB888);
	QImage featureImg = matToQImage(featureMat);
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
	//int i = 0;
	//for (cv::DMatch match : goodMatches){
	//	// Add information to the table
	//	QStandardItem *x1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.x));
	//	model->setItem(i, 0, x1);
	//	QStandardItem *y1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.y));
	//	model->setItem(i, 1, y1);
	//	if (oneToN){
	//		QStandardItem *x2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.x));
	//		model->setItem(i, 2, x2);
	//		QStandardItem *y2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.y));
	//		model->setItem(i, 3, y2);
	//	}
	//	else{
	//		QStandardItem *x2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.x));
	//		model->setItem(i, 2, x2);
	//		QStandardItem *y2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.y));
	//		model->setItem(i, 3, y2);
	//	}
	//	QStandardItem *dist = new QStandardItem(QString::number(match.distance));
	//	model->setItem(i, 4, dist);

	//	QStandardItem *accepted = new QStandardItem("Accepted");
	//	accepted->setData(QColor(Qt::black), Qt::TextColorRole);
	//	accepted->setData(QColor(Qt::green), Qt::BackgroundRole);
	//	model->setItem(i, 5, accepted);
	//	i++;
	//}
	//for (cv::DMatch match : badMatches){
	//	// Add information to the table
	//	QStandardItem *x1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.x));
	//	model->setItem(i, 0, x1);
	//	QStandardItem *y1 = new QStandardItem(QString::number(firstImgKeypoints[match.queryIdx].pt.y));
	//	model->setItem(i, 1, y1);
	//	if (oneToN){
	//		QStandardItem *x2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.x));
	//		model->setItem(i, 2, x2);
	//		QStandardItem *y2 = new QStandardItem(QString::number(setImgsKeypoints[imgIndex][match.trainIdx].pt.y));
	//		model->setItem(i, 3, y2);
	//	}
	//	else{
	//		QStandardItem *x2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.x));
	//		model->setItem(i, 2, x2);
	//		QStandardItem *y2 = new QStandardItem(QString::number(secondImgKeypoints[match.trainIdx].pt.y));
	//		model->setItem(i, 3, y2);
	//	}
	//	QStandardItem *dist = new QStandardItem(QString::number(match.distance));
	//	model->setItem(i, 4, dist);

	//	QStandardItem *rejected = new QStandardItem("Rejected");
	//	rejected->setData(QColor(Qt::black), Qt::TextColorRole);
	//	rejected->setData(QColor(Qt::red), Qt::BackgroundRole);
	//	model->setItem(i, 5, rejected);
	//	i++;
	//}
	//ui->viewTable->setModel(model);

	// Add the image to the viewer
	QGraphicsScene *matchingScene = new QGraphicsScene();

	cv::Mat drawImg;
	if (oneToN) drawImg = cv::imread(currentTest_folderPath + "all matches\\" + std::to_string(imgIndex) + ".jpg");
	else drawImg = cv::imread(currentTest_folderPath + "output.jpg");
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
	if (fileName != "")cv::imwrite(currentTest_folderPath + fileName + ".jpg", outImg);
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
			ui->viewMatchesImageNameText->addItem(QString::fromStdString(std::get<0>(setImgs[i])));
			ui->viewTableImageNameText->addItem(QString::fromStdString(std::get<0>(setImgs[i])));
			// Draw and Store bad matches
			cv::drawMatches(firstImg, firstImgKeypoints, std::get<1>(setImgs[i]), setImgsKeypoints[i],
				badMatchesSet[i], drawImg, cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 255));

			// Draw and Store best matches
			cv::drawMatches(firstImg, firstImgKeypoints, std::get<1>(setImgs[i]), setImgsKeypoints[i],
				goodMatchesSet[i], drawImg, cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), std::vector<char>(), cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

			std::string filename = currentTest_folderPath + "all matches\\" + std::to_string(i) + ".jpg";
			if (!cv::imwrite(filename, drawImg))
				ui->logPlainText->appendHtml(tr("<b style='color:orange'>Image %1  can not be saved (may be because directory %2 does not exist) !</b>").arg(QString::fromStdString(filename)).arg(QString::fromStdString(currentTest_folderPath)));
				
			if (i == bestScoreIndex){
				if (!cv::imwrite(currentTest_folderPath + "output.jpg", drawImg))
					ui->logPlainText->appendHtml(tr("<b style='color:orange'>Image %1output.jpg can not be saved (may be because directory %2 does not exist) !</b>").arg(QString::fromStdString(currentTest_folderPath)).arg(QString::fromStdString(currentTest_folderPath)));

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
		if (!cv::imwrite(currentTest_folderPath + "output.jpg", drawImg))
			ui->logPlainText->appendHtml(tr("<b style='color:orange'>Matches Image can not be saved (may be because directory %1  does not exist) !</b>").arg(QString::fromStdString(currentTest_folderPath)));
		
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
	int testType = takeTestType;
	bool import = (takeTestType==1);
	if (takeTestType==0 || takeTestType==1) testType = 0;
	// Read Images ...
	taskProgressDialog->setLabelText(tr("Reading images ..."));
	emit taskPercentageComplete(1);
	if (!readFirstImage()){
		ui->logPlainText->appendHtml(tr("<b style='color:red'>Error while trying to read the 1st input file!</b>"));
		emit taskPercentageComplete(100);
		return false;
	}
	oneToN = ui->oneToN->isChecked();

	if (oneToN) {
		readSetOfImages(import);
		if (setImgs.size() == 0){
			emit taskPercentageComplete(100);
			showError(tr("Read Images").toStdString(), tr("There is no image in the folder: ").toStdString() + ui->secondImgText->text().toStdString(), tr("Make sure that the folder '<i>").toStdString() + ui->secondImgText->text().toStdString() + tr("'</i>  contains one or more images with correct extension!").toStdString());
			return false;
		}
		if (ui->imageExistsInBdd->isChecked() && ui->bddImageNames->currentIndex() > -1){
			// change index of requested Image to the first
			std::swap(setImgs[0], setImgs[ui->bddImageNames->currentIndex()]);
			//std::swap(ui->bddImageNames[0], ui->bddImageNames[ui->bddImageNames->currentIndex()]);
			QString tmp = ui->bddImageNames->currentText();
			ui->bddImageNames->setItemText(ui->bddImageNames->currentIndex(), ui->bddImageNames->itemText(0));
			ui->bddImageNames->setItemText(0, tmp);
			ui->bddImageNames->setCurrentIndex(0);
			ui->bddImageNames->show();
		}
	}
	else {
		if (!readSecondImage()) {
			ui->logPlainText->appendHtml(tr("<b style='color:red'>Error while trying to read the 2nd input file!</b>"));
			emit taskPercentageComplete(100);
			return false;
		}
	}
	
	if (testType == 0)
	{
		// Create a test folder ...
		if (!createTestFolder()) {
			emit taskPercentageComplete(100);
			return false;
		}
	}

	if (oneToN) matchingMasks = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
	else matchingMask = cv::Mat();
	taskProgressDialog->setLabelText(tr("Processing ..."));
	emit taskPercentageComplete(2);

	// Launch the algorithm
	switch (ui->allMethodsTabs->currentIndex())
	{
	case 0:
		// default
		runDefault(testType);
		break;
	case 1:
	default:
		// custom
		runCustom(testType);
		break;
	}
	if (testType == 0)
	{
		taskProgressDialog->setLabelText(tr("Displaying results ..."));
		bool returnedFromShow = showDecision();
		firstImg.release(); secondImg.release(); setImgs.clear();
		emit taskPercentageComplete(100);
		if (returnedFromShow) {
			ui->logPlainText->appendHtml(tr("<b style='color:green'>This test has been exported with success under the identifier number: %1").arg(QString::number(cpt)));
			ui->logPlainText->appendHtml("--------------------------------------------------------------------------------------------------------------------------------------------------------------------");
			if (takeTestType==0) QMessageBox::information(this, tr("Export Excel !"), tr("This test has been exported with success under the identifier number: %1").arg(QString::number(cpt)));
			return true;
		}
	}
	else {
		emit taskPercentageComplete(100);
		QMessageBox::information(this, tr("Add Command"), tr("This test has been added with success to Excel input file !"));
	}
	return false;
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
		emit taskPercentageComplete(6);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				try{ localThreshold::binarisation(std::get<1>(setImgs[i]), 41, 56); }
				catch (cv::Exception e){
					showError(tr("Binarization").toStdString(), tr("Error in the image '").toStdString() + std::get<0>(setImgs[i]) + "'", e.msg);
				}
			}
		}
		else localThreshold::binarisation(secondImg, 41, 56);
		emit taskPercentageComplete(7);
		double threshold = ui->segmentationThresholdText->text().toFloat();
		emit taskPercentageComplete(8);
		cv::threshold(firstImg, firstImg, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		emit taskPercentageComplete(9);
		if (oneToN)
			for (int i = 0; i < setImgs.size(); i++){
				try{ cv::threshold(std::get<1>(setImgs[i]), std::get<1>(setImgs[i]), threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU); }
				catch (cv::Exception e){
					showError(tr("Thresholding").toStdString(), tr("Error in the image '").toStdString() + std::get<0>(setImgs[i]) + "'", e.msg);
				}
			}
		else cv::threshold(secondImg, secondImg, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		
		//ideka::binOptimisation(firstImg);
		//ideka::binOptimisation(secondImg);
		/*cv::imwrite(currentTest_folderPath + "f-1_Binarization.jpg", firstImg);
		if (oneToN) cv::imwrite(currentTest_folderPath + "l-1_Binarization.jpg", std::get<1>(setImgs[setImgs.size() - 1]));
		else cv::imwrite(currentTest_folderPath + "s-1_Binarization.jpg", secondImg);*/
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
		emit taskPercentageComplete(14);
		//cv::imwrite(currentTest_folderPath + "f-2_Morphological Skeleton.jpg", firstImg);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size();i++){
				std::get<1>(setImgs[i]) = skeletonization(std::get<1>(setImgs[i]));
			}
			//cv::imwrite(currentTest_folderPath + "l-2_Morphological Skeleton.jpg", setImgs[setImgs.size() - 1].second);
		}
		else {
			secondImg = skeletonization(secondImg);
			//cv::imwrite(currentTest_folderPath + "s-2_Morphological Skeleton.jpg", secondImg);
		}
		emit taskPercentageComplete(19);
		break;
	case 2:
		// Thinning of Zhang-Suen
		//This is the same Thinning Algorithme used by BluePrints
		ZhangSuen::thinning(firstImg);
		emit taskPercentageComplete(14);
		//cv::imwrite(currentTest_folderPath + "f-2_Zhang-Suen Thinning.jpg", firstImg);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				ZhangSuen::thinning(std::get<1>(setImgs[i]));
			}
			//cv::imwrite(currentTest_folderPath + "l-2_Zhang-Suen Thinning.jpg", setImgs[setImgs.size() - 1].second);
		}
		else {
			ZhangSuen::thinning(secondImg);
			//cv::imwrite(currentTest_folderPath + "s-2_Zhang-Suen Thinning.jpg", secondImg);
		}
		emit taskPercentageComplete(19);
		break;
	case 3:{
		// Thinning of Lin-Hong implemented by Mrs. Faiçal
		firstImg = Image_processing::thinning(firstImg, firstEnhancedImage, firstSegmentedImage);
		emit taskPercentageComplete(14);
		firstImg.convertTo(firstImg, CV_8UC3, 255);
		emit taskPercentageComplete(15);
		//cv::imwrite(currentTest_folderPath + "f-2_Lin-Hong Thinning.jpg", firstImg);
		if (oneToN)
		{
			setEnhancedImages = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
			setSegmentedImages = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
			emit taskPercentageComplete(16);
			for (int i = 0; i < setImgs.size(); i++){
				std::get<1>(setImgs[i]) = Image_processing::thinning(std::get<1>(setImgs[i]), setEnhancedImages[i], setSegmentedImages[i]);
				std::get<1>(setImgs[i]).convertTo(std::get<1>(setImgs[i]), CV_8UC3, 255);
			}
			emit taskPercentageComplete(19);
			//cv::imwrite(currentTest_folderPath + "l-2_Lin-Hong Thinning.jpg", setImgs[setImgs.size() - 1].second);
		}
		else {
			secondImg = Image_processing::thinning(secondImg, secondEnhancedImage, secondSegmentedImage);
			emit taskPercentageComplete(17);
			secondImg.convertTo(secondImg, CV_8UC3, 255);
			emit taskPercentageComplete(19);
			//cv::imwrite(currentTest_folderPath + "s-2_Lin-Hong Thinning.jpg", secondImg);
		}		
		break;
	}
	case 4:
		// Thinning of Guo-Hall
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  Exception !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		GuoHall::thinning(firstImg);
		emit taskPercentageComplete(14);
		//cv::imwrite(currentTest_folderPath + "f-2_Guo-Hall Thinning.jpg", firstImg);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				GuoHall::thinning(std::get<1>(setImgs[i]));
			}
			//cv::imwrite(currentTest_folderPath + "l-2_Guo-Hall Thinning.jpg", setImgs[setImgs.size() - 1].second);
		}
		else {
			GuoHall::thinning(secondImg);
			//cv::imwrite(currentTest_folderPath + "s-2_Guo-Hall Thinning.jpg", secondImg);
		}
		emit taskPercentageComplete(19);
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
		double distanceThreshBetweenMinutiaes = ui->detectorMinutiaeLimitDistanceText->text().toFloat();
		detectionTime = (double)cv::getTickCount();
		// change this to firstImage and originalInput !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		firstMinutiae = Image_processing::extracting(firstImg, firstEnhancedImage, firstSegmentedImage, firstImg, distanceThreshBetweenMinutiaes);
		emit taskPercentageComplete(23);
		if (oneToN)
		{
			setMinutiaes = std::vector<std::vector<Minutiae>>(setImgs.size(), std::vector<Minutiae>());
			for (int i = 0; i < setImgs.size(); i++){
				setMinutiaes[i] = Image_processing::extracting(std::get<1>(setImgs[i]), setEnhancedImages[i], setSegmentedImages[i], std::get<1>(setImgs[i]), distanceThreshBetweenMinutiaes);
			}
		}
		else secondMinutiae = Image_processing::extracting(secondImg, secondEnhancedImage, secondSegmentedImage, secondImg, distanceThreshBetweenMinutiaes);
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
		emit taskPercentageComplete(30);
		writeKeyPoints(firstImg, firstMinutiae, 1, "keypoints1");
		if (!oneToN) writeKeyPoints(secondImg, secondMinutiae, 2, "keypoints2");
		emit taskPercentageComplete(32);
		// images must be segmented if not Minutiae will be empty
		try{
			for (Minutiae minutiae : firstMinutiae) firstImgKeypoints.push_back(minutiae);
			emit taskPercentageComplete(34);
			if (oneToN)
			{
				setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>());
				for (int i = 0; i < setImgs.size(); i++){
					for (Minutiae minutiae : setMinutiaes[i]) setImgsKeypoints[i].push_back(minutiae);
					emit taskPercentageComplete(37);
					// use masks to matche minutiae of the same type
					matchingMasks[i] = maskMatchesByMinutiaeNature(firstMinutiae, setMinutiaes[i]);
				}
			}
			else {
				for (Minutiae minutiae : secondMinutiae) secondImgKeypoints.push_back(minutiae);
				emit taskPercentageComplete(37);
				// use masks to matche minutiae of the same type
				matchingMask = maskMatchesByMinutiaeNature(firstMinutiae, secondMinutiae);
			}
			emit taskPercentageComplete(39);
			masksAreEmpty = false;
		}
		catch (...){
			ui->logPlainText->appendHtml(tr("<i style='color:red'>Before detecting Minutiae you must select a segmentation method !</i>"));
			return;
		}

	}
	break;
	case 1:{
		// Minutiae-detection using Crossing Number
		// http://www.codelooker.com/id/217/1100103.html
		std::vector<Minutiae> firstMinutiae, secondMinutiae;
		double distanceThreshBetweenMinutiaes = ui->detectorCrossingNumberLimitDistanceText->text().toFloat();
		detectionTime = (double)cv::getTickCount();
		firstMinutiae = CrossingNumber::getMinutiae(firstImg, ui->detectorCrossingNumberBorderText->text().toInt());
		emit taskPercentageComplete(23);
		//Minutiae-filtering
		// slow with the second segmentation
		CrossingNumber::filterMinutiae(firstMinutiae, distanceThreshBetweenMinutiaes);
		emit taskPercentageComplete(25);
		if (oneToN)
		{
			setMinutiaes = std::vector<std::vector<Minutiae>>(setImgs.size(), std::vector<Minutiae>());
			for (int i = 0; i < setImgs.size(); i++){
				setMinutiaes[i] = CrossingNumber::getMinutiae(std::get<1>(setImgs[i]), ui->detectorCrossingNumberBorderText->text().toInt());
				CrossingNumber::filterMinutiae(setMinutiaes[i], distanceThreshBetweenMinutiaes);
			}
			emit taskPercentageComplete(30);
		}
		else {
			secondMinutiae = CrossingNumber::getMinutiae(secondImg, ui->detectorCrossingNumberBorderText->text().toInt());
			emit taskPercentageComplete(28);
			CrossingNumber::filterMinutiae(secondMinutiae, distanceThreshBetweenMinutiaes);
			emit taskPercentageComplete(30);
		}
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();

		writeKeyPoints(firstImg, firstMinutiae, 1, "keypoints1");
		if (!oneToN) writeKeyPoints(secondImg, secondMinutiae, 2, "keypoints2");
		emit taskPercentageComplete(32);
		// images must be segmented if not Minutiae will be empty
		try{
			for (Minutiae minutiae : firstMinutiae) firstImgKeypoints.push_back(minutiae);
			emit taskPercentageComplete(34);
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
				emit taskPercentageComplete(37);
				// use masks to matche minutiae of the same type
				matchingMask = maskMatchesByMinutiaeNature(firstMinutiae, secondMinutiae);
			}
			emit taskPercentageComplete(39);
			masksAreEmpty = false;
		}
		catch (...){
			ui->logPlainText->appendHtml(tr("<i style='color:red'>Before detecting Minutiae you must select a segmentation method !</i>"));
			return;
		}

	}
	break;
	case 2:{
		// Harris-Corners
		detectionTime = (double)cv::getTickCount();
		harrisCorners(firstImg, firstImgKeypoints, ui->detectorHarrisThresholdText->text().toFloat());
		emit taskPercentageComplete(25);
		if (oneToN)
		{
			for (int i = 0; i < setImgs.size(); i++){
				harrisCorners(std::get<1>(setImgs[i]), setImgsKeypoints[i], ui->detectorHarrisThresholdText->text().toFloat());
			}
		}
		else harrisCorners(secondImg, secondImgKeypoints, ui->detectorHarrisThresholdText->text().toFloat());
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
		emit taskPercentageComplete(34);
		writeKeyPoints(firstImg, firstImgKeypoints, 1, "keypoints1");
		if (!oneToN) writeKeyPoints(secondImg, secondImgKeypoints, 2, "keypoints2");
		emit taskPercentageComplete(39);
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
		   emit taskPercentageComplete(23);
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
					cv::FASTX(std::get<1>(setImgs[i]), setImgsKeypoints[i], ui->detectorFastThresholdText->text().toInt(),
						ui->detectorFastNonmaxSuppressionCheck->isChecked(),
						ui->detectorFastTypeText->currentIndex());
				}
			}
			else cv::FASTX(secondImg, secondImgKeypoints, ui->detectorFastThresholdText->text().toInt(),
				ui->detectorFastNonmaxSuppressionCheck->isChecked(),
				ui->detectorFastTypeText->currentIndex());
			detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
		}
		emit taskPercentageComplete(23);
		break;
	case 5:
		// SIFT
		ptrDetector = new cv::SiftFeatureDetector(ui->detectorSiftNfeaturesText->text().toInt(),
			ui->detectorSiftNOctaveLayersText->text().toInt(),
			ui->detectorSiftContrastThresholdText->text().toDouble(),
			ui->detectorSiftEdgeThresholdText->text().toDouble(),
			ui->detectorSiftSigmaText->text().toDouble());
		emit taskPercentageComplete(23);
		break;
	case 6:
		//SURF
		// we didn't need the Extended and Upright params because it is related to the SURF descriptor
		ptrDetector = new cv::SurfFeatureDetector(ui->detectorSurfHessianThresholdText->text().toDouble(),
			ui->detectorSurfNOctavesText->text().toInt(),
			ui->detectorSurfNLayersText->text().toInt(),
			true,
			false);
		emit taskPercentageComplete(23);
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
		emit taskPercentageComplete(23);
		break;
		//....
	default:
		ui->logPlainText->appendHtml(tr("<i style='color:yellow'>No detector selected.</i>"));
		return;
		break;
	}

	if (detectorIndex > 2 && (detectorIndex != 4 || !ui->detectorFastXCheck->isChecked())){
		try{
			// Detecting Keypoints ...
			detectionTime = (double)cv::getTickCount();
			ptrDetector->detect(firstImg, firstImgKeypoints);
			emit taskPercentageComplete(28);
			if (oneToN){
				for (int i = 0; i < setImgs.size(); i++){
					ptrDetector->detect(std::get<1>(setImgs[i]), setImgsKeypoints[i]);
				}
			}else ptrDetector->detect(secondImg, secondImgKeypoints);
			detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
			emit taskPercentageComplete(36);
		}
		catch (...){
			ui->logPlainText->appendHtml(tr("<b style='color:red'>Please select the right %1 detector parameters, or use the defaults!.</b>").arg(QString::fromStdString(detectorName)));
			return;
		}
		writeKeyPoints(firstImg, firstImgKeypoints, 1, "keypoints1");
		if (!oneToN) writeKeyPoints(secondImg, secondImgKeypoints, 2, "keypoints2");
		emit taskPercentageComplete(39);
	}
	if (noKeyPoints("first", firstImgKeypoints) || (!oneToN && noKeyPoints("second", secondImgKeypoints))) return;
	if (oneToN) {
		ui->logPlainText->textCursor().movePosition(QTextCursor::End);
		prev_cursor_position = ui->logPlainText->textCursor().position();
	}
	ui->logPlainText->appendHtml(tr("Detection time: %1(s)").arg(QString::number(detectionTime)));
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
		// we just need the Extended param because others are related to the SURF detector
		ptrDescriptor = new cv::SurfDescriptorExtractor(100.0, 4, 3, ui->descriptorSurfExtended->isChecked(), false);
		break;
		//....
	default:
		ui->logPlainText->appendHtml(tr("<i style='color:yellow'>No descriptor selected.</i>"));
		return;
		break;
	}
	emit taskPercentageComplete(44);
	if (ui->opponentColor->isChecked()){
		//OpponentColor
		ptrDescriptor = new cv::OpponentColorDescriptorExtractor(ptrDescriptor);
	}

	try{
		descriptionTime = (double)cv::getTickCount();
		ptrDescriptor->compute(firstImg, firstImgKeypoints, firstImgDescriptors);
		emit taskPercentageComplete(49);
		if (oneToN)
		{
			setImgsDescriptors = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
			for (int i = 0; i < setImgs.size(); i++){
				ptrDescriptor->compute(std::get<1>(setImgs[i]), setImgsKeypoints[i], setImgsDescriptors[i]);
			}
		}
		else ptrDescriptor->compute(secondImg, secondImgKeypoints, secondImgDescriptors);
		descriptionTime = ((double)cv::getTickCount() - descriptionTime) / cv::getTickFrequency();
		emit taskPercentageComplete(59);
		ui->logPlainText->appendHtml(tr("Description time: %1(s)").arg(QString::number(descriptionTime)));
	}
	catch (...){
		if (descriptorName == "FREAK")ui->logPlainText->appendHtml(tr("<b style='color:red'>Please select the right pair indexes within the FREAK descriptor, or just leave it!.</b><br>(For more details read Section(4.2) in: <i>A. Alahi, R. Ortiz, and P. Vandergheynst. FREAK: Fast Retina Keypoint. In IEEE Conference on Computer Vision and Pattern Recognition, 2012.</i>)"));
		else ui->logPlainText->appendHtml(tr("<b style='color:red'>Please select the right %1 descriptor parameters, or use the defaults!.</b>").arg(QString::fromStdString(descriptorName)));
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
		ui->logPlainText->appendHtml(tr("<i style='color:yellow'>No matcher selected.</i>"));
		return;
		break;
	}
}

bool MainWindow::matching(){
	// Start matching ...
	try{
		if (ui->eliminationNoTest->isChecked() || ui->eliminationInversMatches->isChecked()){
			// Only the best direct match
			matchingTime = (double)cv::getTickCount();
			if (oneToN){
				if (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()){
					ptrMatcher->add(setImgsDescriptors);
					//ptrMatcher->train();
					ptrMatcher->match(firstImgDescriptors, directMatches, matchingMasks);
				}
				else {//(ui->matcher1toNtype2->isChecked())
					directMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
					for (int i = 0; i < setImgs.size(); i++){
						ptrMatcher->match(firstImgDescriptors, setImgsDescriptors[i], directMatchesSet[i], matchingMasks[i]);
					}
				}
			}
			else {
				ptrMatcher->match(firstImgDescriptors, secondImgDescriptors, directMatches, matchingMask);
			}

			if (ui->eliminationInversMatches->isEnabled() && ui->eliminationInversMatches->isChecked()){
				// Also the best match in reverse
				cv::Mat inverseMatchingMask;
				if (oneToN){
					inverseMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
					for (int i = 0; i < setImgs.size(); i++){
						try{
							inverseMatchingMask = matchingMasks[i].t();
						}
						catch (cv::Exception){ inverseMatchingMask = cv::Mat(); }
						ptrMatcher->match(setImgsDescriptors[i], firstImgDescriptors, inverseMatchesSet[i], inverseMatchingMask);
					}
				}
				else {
					try{
						inverseMatchingMask = matchingMask.t();
					}
					catch (cv::Exception){ inverseMatchingMask = cv::Mat(); }
					ptrMatcher->match(secondImgDescriptors, firstImgDescriptors, inverseMatches, inverseMatchingMask);
				}
			}
			matchingTime = ((double)cv::getTickCount() - matchingTime) / cv::getTickFrequency();
		}
		else if (ui->eliminationLoweRatio->isEnabled() && ui->eliminationLoweRatio->isChecked()){
			// Best two set of matches
			matchingTime = (double)cv::getTickCount();
			if (oneToN){
				twoMatchesSet = std::vector<std::vector<std::vector<cv::DMatch>>>(setImgs.size(), std::vector<std::vector<cv::DMatch>>());
				for (int i = 0; i < setImgs.size(); i++){
					ptrMatcher->knnMatch(firstImgDescriptors, setImgsDescriptors[i], twoMatchesSet[i], 2, matchingMasks[i]);
				}
			}
			else ptrMatcher->knnMatch(firstImgDescriptors, secondImgDescriptors, twoMatches, 2, matchingMask);
			matchingTime = ((double)cv::getTickCount() - matchingTime) / cv::getTickFrequency();
		}
	}
	catch (cv::Exception e){
		// For example Flann-Based doesn't work with Brief desctiptor extractor
		// And also, some descriptors must be used with specific NORM_s
		if (ui->matcherTabs->currentIndex() == 0 && ui->matcherBruteForceCrossCheckText->isEnabled() && ui->matcherBruteForceCrossCheckText->isChecked() && ui->detectorTabs->currentIndex()<2)
			ui->logPlainText->appendHtml(tr("<b style='color:orange'>Set <i>Cross Check</i> in Brute Force as false while matching Minutiaes!.</b>"));
		if (ui->matcherTabs->currentIndex() == 0 && ui->matcherBruteForceCrossCheckText->isEnabled() && ui->matcherBruteForceCrossCheckText->isChecked())
			ui->logPlainText->appendHtml(tr("<b style='color:orange'>Set <i>Cross Check</i> in Brute Force as false while using clustering!.</b>"));
		else ui->logPlainText->appendHtml(tr("<b style='color:red'>Cannot match descriptors because of an incompatible combination!, try another one.</b>"));
		return false;
	}
	ui->logPlainText->appendHtml(tr("Matching time: %1(s)").arg(QString::number(matchingTime)));
	return true;
}

void MainWindow::outlierElimination(){
	// Eliminate outliers, and calculate the sum of best matches distance
	float limitDistance = ui->eliminationLimitDistanceText->text().toFloat();
	if (ui->eliminationLimitDistance->isChecked() && limitDistance < 0) {
		ui->logPlainText->appendHtml(tr("<b style='color:red'>Invalid Limit Distance: %1, the default value is maintained!</b>").arg(QString::number(limitDistance)));
		limitDistance = 0.4f;
	}
	/*float confidence = ui->matcherRansacTestConfidence->text().toFloat();
	if (ui->matcherRansacTest->isChecked() && (confidence < 0 || confidence > 1)) {
		ui->logPlainText->appendHtml(tr("<b style='color:red'>Invalid Confidence: %1, the default value is maintained!</b>").arg(QString::number(confidence)));
		confidence = 0.99f;
	}*/
	if (oneToN){
		sumDistancesSet = std::vector<float>(setImgs.size());
		scoreSet = std::vector<float>(setImgs.size());
		float bestScore = 0;

		goodMatchesSet = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());
		badMatchesSet  = std::vector<std::vector<cv::DMatch>>(setImgs.size(), std::vector<cv::DMatch>());

		if (ui->eliminationLoweRatio->isEnabled() && ui->eliminationLoweRatio->isChecked()){
			// Lowe's ratio test = 0.7 by default
			float lowesRatio = ui->eliminationLoweRatioText->text().toFloat();
			if (lowesRatio <= 0 || 1 <= lowesRatio) {
				ui->logPlainText->appendHtml(tr("<b style='color:red'>Invalid Lowe's Ratio: %1, the default value is maintained!</b>").arg(QString::number(lowesRatio)));
				lowesRatio = 0.7;
				ui->eliminationLoweRatioText->setText("0.7");
			}
			for (int i = 0; i < setImgs.size(); i++){
				sumDistancesSet[i] = testOfLowe(twoMatchesSet[i], lowesRatio, limitDistance, goodMatchesSet[i], badMatchesSet[i]);
				
				if (goodMatchesSet[i].size() > 0){
					float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size());
					float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
					scoreSet[i] = ui->normalizationOffsetText->text().toFloat() / (average + 1.0) * goodProbability;
					if (scoreSet[i] > 1.0) scoreSet[i] = 1.0;
					// update the best score index
					if (scoreSet[i] >= bestScore) {
						if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScoreIndex].size()){
							bestScoreIndex = i;
							bestScore = scoreSet[i];
						}
					}
				}
				else scoreSet[i] = 0.0;
			}
		}
		else if (ui->eliminationInversMatches->isEnabled() && ui->eliminationInversMatches->isChecked()){
			// in reverse matching test
			for (int i = 0; i < setImgs.size(); i++){
				sumDistancesSet[i] = testInReverse(directMatchesSet[i], inverseMatchesSet[i], limitDistance, goodMatchesSet[i], badMatchesSet[i]);
				
				if (goodMatchesSet[i].size() > 0){
					float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size());
					float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
					scoreSet[i] = ui->normalizationOffsetText->text().toFloat() / (average + 1.0) * goodProbability;
					if (scoreSet[i] > 1.0) scoreSet[i] = 1.0;
					// update the best score index
					if (scoreSet[i] >= bestScore) {
						if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScoreIndex].size()){
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
					if (directMatches[i].distance <= limitDistance || !ui->eliminationLimitDistance->isChecked()){
						sumDistancesSet[directMatches[i].imgIdx] += directMatches[i].distance;
						goodMatchesSet[directMatches[i].imgIdx].push_back(directMatches[i]);
					}
					else{
						badMatchesSet[directMatches[i].imgIdx].push_back(directMatches[i]);
					}
				}
				for (int i = 0; i < setImgs.size(); i++){
					if (goodMatchesSet[i].size() > 0){
						float goodProbability;
						if (ui->matcher1toNtype1->isEnabled() && ui->matcher1toNtype1->isChecked()) {
							try { goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(directMatches.size());}
							catch(...){goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(firstImgKeypoints.size());}
						}
						else goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size());
						float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
						scoreSet[i] = ui->normalizationOffsetText->text().toFloat() / (average + 1.0) * goodProbability;
						if (scoreSet[i] > 1.0) scoreSet[i] = 1.0;
						// update the best score index
						if (scoreSet[i] >= bestScore) {
								if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScoreIndex].size()){
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
						if (directMatchesSet[i][j].distance <= limitDistance || !ui->eliminationLimitDistance->isChecked()){
							sumDistancesSet[i] += directMatchesSet[i][j].distance;
							goodMatchesSet[i].push_back(directMatchesSet[i][j]);
						}
						else {
							badMatchesSet[i].push_back(directMatchesSet[i][j]);
						}
					}
					if (goodMatchesSet[i].size() > 0){
						float goodProbability = static_cast<float>(goodMatchesSet[i].size()) / static_cast<float>(goodMatchesSet[i].size() + badMatchesSet[i].size());
						float average = sumDistancesSet[i] / static_cast<float>(goodMatchesSet[i].size());
						scoreSet[i] = ui->normalizationOffsetText->text().toFloat() / (average + 1.0) * goodProbability;
						if (scoreSet[i] > 1.0) scoreSet[i] = 1.0;
						// update the best score index
						if (scoreSet[i] >= bestScore) {
								if (scoreSet[i] > bestScore || goodMatchesSet[i].size() > goodMatchesSet[bestScoreIndex].size()){
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
		if (ui->eliminationLoweRatio->isChecked()){
			// Lowe's ratio test = 0.7 by default
			float lowesRatio = ui->eliminationLoweRatioText->text().toFloat();
			if (lowesRatio <= 0 || 1 <= lowesRatio) {
				ui->logPlainText->appendHtml(tr("<b style='color:red'>Invalid Lowe's Ratio: %1, the default value is maintained!</b>").arg(QString::number(lowesRatio)));
				lowesRatio = 0.7f;
			}
			sumDistances = testOfLowe(twoMatches, lowesRatio, limitDistance, goodMatches, badMatches);
		}
		else if (ui->eliminationInversMatches->isChecked()){
			// in reverse matching test
			sumDistances = testInReverse(directMatches, inverseMatches, limitDistance, goodMatches, badMatches);
		}
		else {
			// No elimination test
			// 1 to 1
			for (int i = 0; i < directMatches.size(); i++){
				if (directMatches[i].distance <= limitDistance || !ui->eliminationLimitDistance->isChecked()){
					sumDistances += directMatches[i].distance;
					goodMatches.push_back(directMatches[i]);
				}
				else{
					badMatches.push_back(directMatches[i]);
				}
			}
		}
		/*if (ui->matcherRansacTest->isChecked()) {
			float ransacLimitDistance = ui->eliminationLimitDistance->isChecked() ? limitDistance : std::numeric_limits<float>::max();
			sumDistances -= testRansac(firstImgKeypoints, secondImgKeypoints, ransacLimitDistance, confidence, goodMatches, badMatches, cv::Mat());
		}*/
		if (goodMatches.size() > 0){
			float average = sumDistances / static_cast<float>(goodMatches.size());
			float goodProbability = static_cast<float>(goodMatches.size()) / static_cast<float>(goodMatches.size() + badMatches.size());
			score = ui->normalizationOffsetText->text().toFloat() / (average + 1.0) * goodProbability;
			if (score > 1.0) score = 1.0;
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
	if(e_msg!="")ui->logPlainText->appendHtml(tr("<b style='color:red'>Code Error: %1 </b>").arg(QString::fromStdString(e_msg)));
}

float MainWindow::testOfLowe(std::vector<std::vector<cv::DMatch>> twoMatches, float lowesRatio, float limitDistance, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches){
	// put matches that accept the Lowe's test in goodMatches
	// and put the other matches in badMatches
	// and return sum of distances of goodMatches
	double sumDistances = 0; 
	for (int i = 0; i < twoMatches.size(); i++){
		switch (twoMatches[i].size()){
			case 1:
				if (twoMatches[i][0].distance <= limitDistance || !ui->eliminationLimitDistance->isChecked()){
					sumDistances += twoMatches[i][0].distance;
					goodMatches.push_back(twoMatches[i][0]);
				}
				else badMatches.push_back(twoMatches[i][0]);
			break;
			case 2:
				if ((twoMatches[i][0].distance <= limitDistance || !ui->eliminationLimitDistance->isChecked())
					&& (twoMatches[i][0].distance <= lowesRatio*twoMatches[i][1].distance)){
					sumDistances += twoMatches[i][0].distance;
					goodMatches.push_back(twoMatches[i][0]);
				}
				else badMatches.push_back(twoMatches[i][0]);
			break;
		}
	}
	return sumDistances;
}

float MainWindow::testInReverse(std::vector<cv::DMatch> directMatches, std::vector<cv::DMatch> inverseMatches, float limitDistance, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches){
	// put matches that are in direct and reverse Matches in goodMatches
	// and put the other matches in badMatches
	// and return sum of distances of goodMatches
 	double sumDistances = 0;
	for (int i = 0; i < directMatches.size(); i++){
		if (directMatches[i].distance <= limitDistance || !ui->eliminationLimitDistance->isChecked()){
			// Check if the match is the same in reverse
			for (int j = 0; j < inverseMatches.size(); j++)
			{
				if ((directMatches[i].queryIdx == inverseMatches[j].trainIdx) && (inverseMatches[j].queryIdx == directMatches[i].trainIdx))
				{
					// add symmetrical match
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

float MainWindow::testRansac(std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints, float limitDistance, float confidence, std::vector<cv::DMatch> &goodMatches, std::vector<cv::DMatch> &badMatches, cv::Mat &fundamental) {
	// Identify good matches using RANSAC
	// Return fundamental matrix
	float distanceToRemove = 0.0f;
	// Convert keypoints into Point2f
	std::vector<cv::Point2f> points1, points2;
	for (std::vector<cv::DMatch>::
		const_iterator it = goodMatches.begin();
		it != goodMatches.end(); ++it) {
		// Get the position of left keypoints
		float x = firstImgKeypoints[it->queryIdx].pt.x;
		float y = firstImgKeypoints[it->queryIdx].pt.y;
		points1.push_back(cv::Point2f(x, y));
		// Get the position of right keypoints
		x = secondImgKeypoints[it->trainIdx].pt.x;
		y = secondImgKeypoints[it->trainIdx].pt.y;
		points2.push_back(cv::Point2f(x, y));
	}
	// Compute F matrix using RANSAC
	std::vector<cv::DMatch> outMatches;
	std::vector<uchar> inliers(points1.size(), 0);
	if (points1.size()>0 && points2.size()>0){
		fundamental = cv::findFundamentalMat(cv::Mat(points1), cv::Mat(points2), // matching points
			inliers,       // match status (inlier or outlier)
			cv::FM_RANSAC, // RANSAC method
			limitDistance,      // distance to epipolar line
			confidence); // confidence probability
		// extract the surviving (inliers) matches
		std::vector<uchar>::const_iterator
			itIn = inliers.begin();
		std::vector<cv::DMatch>::const_iterator
			itM = goodMatches.begin();
		// for all matches
		for (; itIn != inliers.end(); ++itIn, ++itM) {
			if (*itIn) { // it is a valid match
				outMatches.push_back(*itM);
			}
			else {
				badMatches.push_back(*itM);
				distanceToRemove += (*itM).distance;
			}
		}
		goodMatches = outMatches;
	}
	return distanceToRemove;
}

void ransacTest(const std::vector<cv::DMatch> matches, const std::vector<cv::KeyPoint>&keypoints1, const std::vector<cv::KeyPoint>& keypoints2, std::vector<cv::DMatch>& goodMatches, double distance, double confidence, double minInlierRatio)
{
	goodMatches.clear();
	// Convert keypoints into Point2f
	std::vector<cv::Point2f> points1, points2;
	for (std::vector<cv::DMatch>::const_iterator it = matches.begin(); it != matches.end(); ++it)
	{
		// Get the position of left keypoints
		float x = keypoints1[it->queryIdx].pt.x;
		float y = keypoints1[it->queryIdx].pt.y;
		points1.push_back(cv::Point2f(x, y));
		// Get the position of right keypoints
		x = keypoints2[it->trainIdx].pt.x;
		y = keypoints2[it->trainIdx].pt.y;
		points2.push_back(cv::Point2f(x, y));
	}
	// Compute F matrix using RANSAC
	std::vector<uchar> inliers(points1.size(), 0);
	cv::Mat fundemental = cv::findFundamentalMat(cv::Mat(points1), cv::Mat(points2), inliers, CV_FM_RANSAC, distance, confidence); // confidence probability
	// extract the surviving (inliers) matches
	std::vector<uchar>::const_iterator
		itIn = inliers.begin();
	std::vector<cv::DMatch>::const_iterator
		itM = matches.begin();
	// for all matches
	for (; itIn != inliers.end(); ++itIn, ++itM)
	{
		if (*itIn)
		{ // it is a valid match
			goodMatches.push_back(*itM);
		}
	}
}

cv::Mat MainWindow::maskMatchesByMinutiaeNature(std::vector<Minutiae> firstImgKeypoints, std::vector<Minutiae> secondImgKeypoints){
	// To matche bifurcation with bifurcation and ridge ending with ridge ending...
	// Only if keypoints are minutiae
	cv::Mat mask = cv::Mat::zeros(firstImgKeypoints.size(), secondImgKeypoints.size(), CV_8UC1);
	for (size_t i = 0; i < firstImgKeypoints.size(); i++)
	{
		for (size_t j = 0; j < secondImgKeypoints.size(); j++)
		{
			if (firstImgKeypoints[i].getType() == secondImgKeypoints[j].getType()) mask.at<uchar>(i, j) = 1;
			//else mask.at<uchar>(i, j) = 0;
		}
	}
	return mask;
}

void MainWindow::maskMatchesByCluster(std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints){
	// To matche only descriptors in the same cluster...
	// Only if we have clusters
	if (masksAreEmpty) matchingMask = cv::Mat::ones(firstImgKeypoints.size(), secondImgKeypoints.size(), CV_8UC1);
	for (size_t i = 0; i < firstImgKeypoints.size(); i++)
	{
		for (size_t j = 0; j < secondImgKeypoints.size(); j++)
		{
			if (labels.at<int>(i) != labels.at<int>(firstImgKeypoints.size() + j)) matchingMask.at<uchar>(i, j) = 0;
		}
	}
}

void MainWindow::maskMatchesByCluster(std::vector<cv::KeyPoint> firstImgKeypoints, std::vector<cv::KeyPoint> secondImgKeypoints, int imgIndx){
	// To matche only descriptors in the same cluster...
	// Only if we have clusters
	try{
		if (masksAreEmpty) matchingMasks[imgIndx] = cv::Mat::ones(firstImgKeypoints.size(), secondImgKeypoints.size(), CV_8UC1);
		for (size_t i = 0; i < firstImgKeypoints.size(); i++)
		{
			for (size_t j = 0; j < secondImgKeypoints.size(); j++)
			{
				if (labels.at<int>(i) != clusterAffectation[imgIndx][j]) matchingMasks[imgIndx].at<uchar>(i, j) = 0;
			}
		}
	}
	catch (cv::Exception e){
		ui->logPlainText->appendHtml(tr("<i style='color:red'>Cannot mask some matches by clusters! (between first and %1 image)</i>").arg(imgIndx));
		return;
	}
}

bool MainWindow::showDecision(){
	float scoreThreshold = ui->decisionStageThresholdText->text().toFloat();

	if (oneToN) {
		if (bestScoreIndex > -1){
			// Show infos related to best image
			QTextCursor current_cursor = QTextCursor(ui->logPlainText->document());
			current_cursor.setPosition(prev_cursor_position);
			current_cursor.insertText(tr("\nFound %1 key points in the most similar image").arg(QString::number(setImgsKeypoints[bestScoreIndex].size())));
			displayImage(std::get<2>(setImgs[bestScoreIndex]), 2);
			if (setMinutiaes.size()) writeKeyPoints(std::get<1>(setImgs[bestScoreIndex]), setMinutiaes[bestScoreIndex], 2, "keypoints2");
			else writeKeyPoints(std::get<1>(setImgs[bestScoreIndex]), setImgsKeypoints[bestScoreIndex], 2, "keypoints2");

			if (scoreSet[bestScoreIndex] >= scoreThreshold)
				ui->logPlainText->appendHtml(tr("The image <b>%1</b> has the best matching score: <b>%2</b><b style='color:green'> &ge; </b>%3").arg(QString::fromStdString(std::get<0>(setImgs[bestScoreIndex]))).arg(QString::number(scoreSet[bestScoreIndex])).arg(QString::number(scoreThreshold)));
			else ui->logPlainText->appendHtml(tr("The image <b>%1</b> has the best matching score: <b>%2</b><b style='color:red'> &#60; </b>%3").arg(QString::fromStdString(std::get<0>(setImgs[bestScoreIndex]))).arg(QString::number(scoreSet[bestScoreIndex])).arg(QString::number(scoreThreshold)));

			if (ui->imageExistsInBdd->isEnabled() && ui->imageExistsInBdd->isChecked()){
				ui->logPlainText->appendHtml(tr("The first image is Rank-<b>%1</b> ").arg(QString::number(computeRankK(scoreThreshold))));
				if (ui->bddImageNames->currentIndex() > -1) if (scoreSet[ui->bddImageNames->currentIndex()] < scoreThreshold) ui->logPlainText->appendHtml(tr("There is a False Non-Match (FNM)"));
			}
			else{
				if (scoreSet[bestScoreIndex] >= scoreThreshold)ui->logPlainText->appendHtml(tr("There is a False Match (FM)"));
			}
			// View results
			displayMatches(bestScoreIndex);
			writeMatches(bestScoreIndex);
			//exportTable(goodMatchesSet[bestScoreIndex].size() + badMatchesSet[bestScoreIndex].size());
			return true;
		}
		else {
			if (ui->imageExistsInBdd->isEnabled() && ui->imageExistsInBdd->isChecked()){
				ui->logPlainText->appendHtml(tr("The first image is Rank-<b>%1</b> ").arg(QString::number(computeRankK(scoreThreshold))));
				if (ui->bddImageNames->currentIndex() > -1 && scoreSet.size()>0)
					if (scoreSet[ui->bddImageNames->currentIndex()] < scoreThreshold) ui->logPlainText->appendHtml(tr("There is a False Non-Match (FNM)"));
			}
			// View results
			ui->logPlainText->appendHtml(tr("<b style='color:red'>All obtained scores are null.</b>"));
			displayMatches(0);
			writeMatches(0);
		}
	}
	else {
		if (score >= scoreThreshold)
			ui->logPlainText->appendHtml(tr("Matching score = <b>%1</b><b style='color:green'> &ge; </b>%2").arg(QString::number(score)).arg(QString::number(scoreThreshold)));
		else ui->logPlainText->appendHtml(tr("Matching score = <b>%1</b><b style='color:red'> &#60; </b>%2").arg(QString::number(score)).arg(QString::number(scoreThreshold)));

		// View results
		displayMatches();
		writeMatches();
		//exportTable(goodMatches.size() + badMatches.size());
		return true;
	}
}

void MainWindow::importExcelFile()
{
	try
	{
		bool exist = false, taskIsCanceled = false;
		int column;
		float scoreThreshold, acceptedMatches, rejectedMatches, bestImageAverage, bestImageScore, goodProbability, badProbability;
		if (importExcelFileType == 0) excelRecover = new ExcelManager(true, exportFile, 0);
		else excelRecover = new ExcelManager(true, inputFile, 11);
		emit taskPercentageComplete(1);
		for (int methodIndex = 1; methodIndex <= 5; methodIndex++) {
			excelRecover->GetIntRows(methodIndex);
			column = excelRecover->getColumnsCount();
			int nbTasks = excelRecover->getSheetCount();
			int progressPercentage = 2;
			while (!taskIsCanceled && progressPercentage <= nbTasks) {
				if (taskProgressDialog->wasCanceled()){
					taskIsCanceled = true;
					break;
				}
				if ((importExcelFileType == 2) || ((ui->tabWidget_2->currentIndex() == 1) && (excelRecover->GetCellValue(progressPercentage, 1) == ui->spinBox->text()))) {
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

					if (importExcelFileType == 0)
					{
						for (int i = 0; i < 5; i++) ui->viewTabs->setCurrentIndex(i); // just to center contents
						image = cv::imread(excelRecover->GetCellValue(progressPercentage, 3).toString().toStdString(), CV_LOAD_IMAGE_COLOR);
						displayImage(image, 1);
					}
					ui->firstImgText->setText(excelRecover->GetCellValue(progressPercentage, 3 - importExcelFileType).toString());

					if (importExcelFileType == 0)
					{
						image = cv::imread((QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Palmprint Registration/" + ui->spinBox->text() + "/keypoints1.jpg").toStdString(), CV_LOAD_IMAGE_COLOR);
						displayFeature(image, 1);
					}

					ui->secondImgText->setText(excelRecover->GetCellValue(progressPercentage, 4 - importExcelFileType).toString());

					if (importExcelFileType == 0)
					{
						image = cv::imread((QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Palmprint Registration/" + ui->spinBox->text() + "/keypoints2.jpg").toStdString(), CV_LOAD_IMAGE_COLOR);
						displayFeature(image, 2);

						image = cv::imread((QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Palmprint Registration/" + ui->spinBox->text() + "/output.jpg").toStdString(), CV_LOAD_IMAGE_COLOR);
						displayFeature(image, 3);
					}

					switch (methodIndex) {
					case 1:
					{
						// SIFT
						ui->siftContThreshText->setText(excelRecover->GetCellValue(progressPercentage, 8 - importExcelFileType).toString());
						ui->siftEdgeThreshText->setText(excelRecover->GetCellValue(progressPercentage, 9 - importExcelFileType).toString());
						ui->siftNumFeatText->setText(excelRecover->GetCellValue(progressPercentage, 10 - importExcelFileType).toString());
						ui->siftNumOctText->setText(excelRecover->GetCellValue(progressPercentage, 11 - importExcelFileType).toString());
						ui->siftSigmaText->setText(excelRecover->GetCellValue(progressPercentage, 12 - importExcelFileType).toString());
						ui->siftBruteForceCheck->setChecked(excelRecover->GetCellValue(progressPercentage, 13 - importExcelFileType).toBool());
						break;
					}
					case 2:
					{
						// SURF
						ui->surfHessianThreshText->setText(excelRecover->GetCellValue(progressPercentage, 8 - importExcelFileType).toString());
						ui->surfNumOctavesText->setText(excelRecover->GetCellValue(progressPercentage, 9 - importExcelFileType).toString());
						ui->surfNumOctLayersText->setText(excelRecover->GetCellValue(progressPercentage, 10 - importExcelFileType).toString());
						ui->surfExtendedText->setChecked(excelRecover->GetCellValue(progressPercentage, 11 - importExcelFileType).toBool());
						ui->surfUprightText->setChecked(excelRecover->GetCellValue(progressPercentage, 12 - importExcelFileType).toBool());
						ui->surfBruteForceCheck->setChecked(excelRecover->GetCellValue(progressPercentage, 13 - importExcelFileType).toBool());
						break;
					}
					case 3:
					{
						// ORB
						ui->orbNumFeatText->setText(excelRecover->GetCellValue(progressPercentage, 8 - importExcelFileType).toString());
						ui->orbScaleFactText->setText(excelRecover->GetCellValue(progressPercentage, 9 - importExcelFileType).toString());
						ui->orbNumLevelsText->setText(excelRecover->GetCellValue(progressPercentage, 10 - importExcelFileType).toString());
						ui->orbEdgeThreshText->setText(excelRecover->GetCellValue(progressPercentage, 11 - importExcelFileType).toString());
						ui->orbFirstLevText->setText(excelRecover->GetCellValue(progressPercentage, 12 - importExcelFileType).toString());
						ui->orbWTAKText->setText(excelRecover->GetCellValue(progressPercentage, 13 - importExcelFileType).toString());
						if (excelRecover->GetCellValue(progressPercentage, 14 - importExcelFileType).toString() == "Harris") ui->orbScoreHarrisRadioBtn->setChecked(true);
						else ui->orbScoreFastRadioBtn->setChecked(true);
						ui->orbPatchSizeText->setText(excelRecover->GetCellValue(progressPercentage, 15 - importExcelFileType).toString());
						break;
					}
					case 4:
					{
						// BRISK
						ui->briskPatternScaleText->setText(excelRecover->GetCellValue(progressPercentage, 8 - importExcelFileType).toString());
						ui->briskOctavesText->setText(excelRecover->GetCellValue(progressPercentage, 9 - importExcelFileType).toString());
						ui->briskThreshText->setText(excelRecover->GetCellValue(progressPercentage, 10 - importExcelFileType).toString());
						break;
					}
					default:
					{
						// Custom
						int segmentationName = segmentationNameToInt(excelRecover->GetCellValue(progressPercentage, 8 - importExcelFileType).toString()),
							detectorName = detectorNameToInt(excelRecover->GetCellValue(progressPercentage, 10 - importExcelFileType).toString()),
							descriptorName = descriptorNameToInt(excelRecover->GetCellValue(progressPercentage, 16 - importExcelFileType).toString()),
							matcherName = matcherNameToInt(excelRecover->GetCellValue(progressPercentage, 22 - importExcelFileType).toString());

						ui->segmentationTabs->setCurrentIndex(segmentationName);
						ui->detectorTabs->setCurrentIndex(detectorName);
						ui->descriptorTabs->setCurrentIndex(descriptorName);
						ui->matcherTabs->setCurrentIndex(matcherName);

						ui->segmentationThresholdText->setText(excelRecover->GetCellValue(progressPercentage + 1, 9 - importExcelFileType).toString());

						switch (detectorName){
						case 0:{
							// Minutiae-detection using Crossing Number By Dr. Faiçal
							ui->detectorMinutiaeLimitDistanceText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							break;
						}
						case 1:{
							// Minutiae-detection using Crossing Number
							ui->detectorCrossingNumberLimitDistanceText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							ui->detectorCrossingNumberBorderText->setText(excelRecover->GetCellValue(progressPercentage + 1, 12 - importExcelFileType).toString());
							break;
						}
						case 2:{
							// Harris-Corners
							ui->detectorHarrisThresholdText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							break;
						}
						case 3:{
							// STAR
							ui->detectorStarMaxSizeText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							ui->detectorStarResponseThresholdText->setText(excelRecover->GetCellValue(progressPercentage + 1, 12 - importExcelFileType).toString());
							ui->detectorStarThresholdProjectedText->setText(excelRecover->GetCellValue(progressPercentage + 1, 13 - importExcelFileType).toString());
							ui->detectorStarThresholdBinarizedText->setText(excelRecover->GetCellValue(progressPercentage + 1, 14 - importExcelFileType).toString());
							ui->detectorStarSuppressNonmaxSizeText->setText(excelRecover->GetCellValue(progressPercentage + 1, 15 - importExcelFileType).toString());
							break;
						}
						case 4: {
							// FAST
							ui->detectorFastThresholdText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							ui->detectorFastNonmaxSuppressionCheck->setChecked(excelRecover->GetCellValue(progressPercentage + 1, 12 - importExcelFileType).toBool());
							ui->detectorFastXCheck->setChecked(excelRecover->GetCellValue(progressPercentage + 1, 13 - importExcelFileType).toBool());
							if (excelRecover->GetCellValue(progressPercentage + 1, 13 - importExcelFileType).toBool())
							{
								ui->detectorFastTypeText->setEnabled(true);
								int index = ui->detectorFastTypeText->findText(excelRecover->GetCellValue(progressPercentage + 1, 14 - importExcelFileType).toString());
								ui->detectorFastTypeText->setCurrentIndex(index);
							}
							else ui->detectorFastTypeText->setEnabled(false);
							break;
						}
						case 5: {
							// SIFT
							ui->detectorSiftContrastThresholdText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							ui->detectorSiftEdgeThresholdText->setText(excelRecover->GetCellValue(progressPercentage + 1, 12 - importExcelFileType).toString());
							ui->detectorSiftNfeaturesText->setText(excelRecover->GetCellValue(progressPercentage + 1, 13 - importExcelFileType).toString());
							ui->detectorSiftNOctaveLayersText->setText(excelRecover->GetCellValue(progressPercentage + 1, 14 - importExcelFileType).toString());
							ui->detectorSiftSigmaText->setText(excelRecover->GetCellValue(progressPercentage + 1, 15 - importExcelFileType).toString());
							break;
						}
						case 6: {
							//SURF
							ui->detectorSurfHessianThresholdText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							ui->detectorSurfNOctavesText->setText(excelRecover->GetCellValue(progressPercentage + 1, 12 - importExcelFileType).toString());
							ui->detectorSurfNLayersText->setText(excelRecover->GetCellValue(progressPercentage + 1, 13 - importExcelFileType).toString());
							ui->detectorSurfUprightText->setChecked(excelRecover->GetCellValue(progressPercentage + 1, 14 - importExcelFileType).toBool());
							break;
						}
						case 7: {
							//Dense
							ui->detectorDenseInitFeatureScaleText->setText(excelRecover->GetCellValue(progressPercentage + 1, 11 - importExcelFileType).toString());
							ui->detectorDenseFeatureScaleLevelsText->setText(excelRecover->GetCellValue(progressPercentage + 1, 12 - importExcelFileType).toString());
							ui->detectorDenseFeatureScaleMulText->setText(excelRecover->GetCellValue(progressPercentage + 1, 13 - importExcelFileType).toString());
							ui->detectorDenseInitXyStepText->setText(excelRecover->GetCellValue(progressPercentage + 1, 14 - importExcelFileType).toString());
							ui->detectorDenseInitImgBoundText->setText(excelRecover->GetCellValue(progressPercentage + 1, 15 - importExcelFileType).toString());
							break;
						}
						}

						switch (descriptorName)
						{
						case 0:
						{
							// FREAK
							ui->descriptorFreakOrientationNormalizedCheck->setChecked(excelRecover->GetCellValue(progressPercentage + 1, 17 - importExcelFileType).toBool());
							ui->descriptorFreakScaleNormalizedCheck->setChecked(excelRecover->GetCellValue(progressPercentage + 1, 18 - importExcelFileType).toBool());
							ui->descriptorFreakPatternScaleText->setText(excelRecover->GetCellValue(progressPercentage + 1, 19 - importExcelFileType).toString());
							ui->descriptorFreakNOctavesText->setText(excelRecover->GetCellValue(progressPercentage + 1, 20 - importExcelFileType).toString());
							ui->descriptorFreakSelectedPairsText->setText(excelRecover->GetCellValue(progressPercentage + 1, 21 - importExcelFileType).toString());
							break;
						}
						case 1:
						{
							// BRIEF
							ui->descriptorBriefPATCH_SIZEText->setText(excelRecover->GetCellValue(progressPercentage + 1, 17 - importExcelFileType).toString());
							ui->descriptorBriefKERNEL_SIZEText->setText(excelRecover->GetCellValue(progressPercentage + 1, 18 - importExcelFileType).toString());
							ui->descriptorBriefLengthText->setText(excelRecover->GetCellValue(progressPercentage + 1, 19 - importExcelFileType).toString());
							break;
						}
						case 2:
						{
							// SIFT
							ui->descriptorSiftLengthText->setText(excelRecover->GetCellValue(progressPercentage + 1, 17 - importExcelFileType).toString());
							break;
						}
						case 3:
						{
							//SURF
							if (excelRecover->GetCellValue(progressPercentage + 1, 17 - importExcelFileType).toString() == "128") ui->descriptorSurfExtended->setChecked(true);
							else ui->descriptorSurfNotExtended->setChecked(true);
							break;
						}
						}

						switch (matcherName)
						{
						case 0:
						{
							// BruteForce
							int index = ui->matcherBruteForceNormTypeText->findText(excelRecover->GetCellValue(progressPercentage + 1, 23 - importExcelFileType).toString());
							ui->matcherBruteForceNormTypeText->setCurrentIndex(index);
							ui->matcherBruteForceCrossCheckText->setChecked(excelRecover->GetCellValue(progressPercentage + 1, 24 - importExcelFileType).toBool());
							break;
						}
						case 1:
						{
							// FlannBased
							FlannBasedNameToIndex(excelRecover->GetCellValue(progressPercentage + 1, 23 - importExcelFileType).toString());
							ui->matcherFlannBasedSearchParamsText->setText(excelRecover->GetCellValue(progressPercentage + 1, 24 - importExcelFileType).toString());
							break;
						}
						}

						if (excelRecover->GetCellValue(progressPercentage + 1, 25 - importExcelFileType).toString() == ui->matcher1toNtype1->text()) ui->matcher1toNtype1->setChecked(true);
						else ui->matcher1toNtype2->setChecked(true);

						ui->withClusteringChecker->setChecked(excelRecover->GetCellValue(progressPercentage + 1, 26 - importExcelFileType).toBool());

						if (ui->withClusteringChecker->isChecked())
						{
							ui->clusteringNbClustersText->setEnabled(true);
							ui->clusteringNbAttemptsText->setEnabled(true);
							ui->clusteringNbClustersText->setText(excelRecover->GetCellValue(progressPercentage + 1, 27 - importExcelFileType).toString());
							ui->clusteringNbAttemptsText->setText(excelRecover->GetCellValue(progressPercentage + 1, 28 - importExcelFileType).toString());
						}
						else
						{
							ui->clusteringNbClustersText->setEnabled(false);
							ui->clusteringNbAttemptsText->setEnabled(false);
						}


						OutliersEliminationToInt(excelRecover->GetCellValue(progressPercentage + 1, 29 - importExcelFileType).toString());

						if (excelRecover->GetCellValue(progressPercentage, 30 - importExcelFileType).toString().isEmpty())
						{
							ui->eliminationLimitDistance->setChecked(false);
							ui->eliminationLimitDistanceText->setEnabled(false);
						}
						else
						{
							ui->eliminationLimitDistance->setChecked(true);
							ui->eliminationLimitDistanceText->setEnabled(true);
							ui->eliminationLimitDistanceText->setText(excelRecover->GetCellValue(progressPercentage + 1, 30 - importExcelFileType).toString());
						}

						ui->opponentColor->setChecked(excelRecover->GetCellValue(progressPercentage, 31 - importExcelFileType).toBool());

						break;
					}

					}
					if (importExcelFileType == 0)
					{
						ui->logPlainText->appendHtml(tr("<b style='color:green'>Starting %1 test (Done on: %2):</b> ").arg(excelRecover->GetSheetName()).arg(excelRecover->GetCellValue(progressPercentage, 2).toString()));

						ui->normalizationOffsetText->setText(excelRecover->GetCellValue(progressPercentage, column - 15).toString());
						ui->decisionStageThresholdText->setText(excelRecover->GetCellValue(progressPercentage, column - 14).toString());

						ui->logPlainText->appendHtml(tr("Found %1 key points in the first image").arg(excelRecover->GetCellValue(progressPercentage, column - 13).toString()));

						scoreThreshold = excelRecover->GetCellValue(progressPercentage, column - 14).toString().toFloat();
						acceptedMatches = excelRecover->GetCellValue(progressPercentage, column - 6).toString().toFloat();
						rejectedMatches = excelRecover->GetCellValue(progressPercentage, column - 5).toString().toFloat();
						bestImageAverage = excelRecover->GetCellValue(progressPercentage, column - 4).toString().toFloat();
						bestImageScore = excelRecover->GetCellValue(progressPercentage, column - 3).toString().toFloat();

						goodProbability = acceptedMatches / (acceptedMatches + rejectedMatches) * 100;
						badProbability = rejectedMatches / (acceptedMatches + rejectedMatches) * 100;
					}
					else
					{
						ui->normalizationOffsetText->setText(excelRecover->GetCellValue(progressPercentage, column - 1).toString()); 
						ui->decisionStageThresholdText->setText(excelRecover->GetCellValue(progressPercentage, column).toString());
					}

					//ui->refreshBddImageNames->setEnabled(false);
					ui->bddImageNames->clear();
					disconnect(ui->viewMatchesImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
					ui->viewMatchesImageNameText->clear();
					disconnect(ui->viewTableImageNameText, SIGNAL(currentIndexChanged(int)), this, SLOT(displayMatches(int)));
					ui->viewTableImageNameText->clear();

					if (excelRecover->GetCellValue(progressPercentage, 5 - importExcelFileType).toBool())
					{
						if (importExcelFileType == 0) image = cv::imread(excelRecover->GetCellValue(progressPercentage, 4).toString().toStdString() + '/' + excelRecover->GetCellValue(progressPercentage, column - 1).toString().toStdString(), CV_LOAD_IMAGE_COLOR);
						ui->oneToN->setChecked(true);
						ui->imageExistsInBdd->setEnabled(true);
						ui->bddImageNames->setEnabled(true);
						ui->imageExistsInBdd->setChecked(excelRecover->GetCellValue(progressPercentage, 6 - importExcelFileType).toBool());
						if (excelRecover->GetCellValue(progressPercentage, 6 - importExcelFileType).toBool())
						{
							if (importExcelFileType == 0) ui->bddImageNames->addItem(excelRecover->GetCellValue(progressPercentage, 7 - importExcelFileType).toString());
							else RequestedImage = excelRecover->GetCellValue(progressPercentage, 7 - importExcelFileType).toString();
						}
						else
						{
							ui->bddImageNames->setEnabled(false);
						}
						if (importExcelFileType == 0)
						{
							ui->logPlainText->appendHtml(tr("Found %1 key points in the most similar image").arg(excelRecover->GetCellValue(progressPercentage, column - 12).toString()));
							ui->logPlainText->appendHtml(tr("Detection time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 11).toString()));
							ui->logPlainText->appendHtml(tr("Description time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 10).toString()));
							if (!excelRecover->GetCellValue(progressPercentage, column - 9).toString().isEmpty()) ui->logPlainText->appendHtml(tr("Clustering time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 9).toString()));
							ui->logPlainText->appendHtml(tr("Matching time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 8).toString()));
							ui->logPlainText->appendHtml(tr("Total time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 7).toString()));
							ui->viewMatchesImageNameText->addItem(excelRecover->GetCellValue(progressPercentage, column - 1).toString());
							ui->viewTableImageNameText->addItem(excelRecover->GetCellValue(progressPercentage, column - 1).toString());
							if (bestImageScore >= scoreThreshold)
								ui->logPlainText->appendHtml(tr("The image <b>%1</b> has the best matching score = <b>%2</b><b style='color:green'> &ge; </b>%3").arg(excelRecover->GetCellValue(progressPercentage, column - 1).toString()).arg(QString::number(bestImageScore)).arg(QString::number(scoreThreshold)));
							else ui->logPlainText->appendHtml(tr("The image <b>%1</b> has the best matching score = <b>%2</b><b style='color:red'> &#60; </b>%3").arg(excelRecover->GetCellValue(progressPercentage, column - 1).toString()).arg(QString::number(bestImageScore)).arg(QString::number(scoreThreshold)));
							if (excelRecover->GetCellValue(progressPercentage, 6).toBool()) ui->logPlainText->appendHtml(tr("The first image is Rank-<b>%1</b> ").arg(excelRecover->GetCellValue(progressPercentage, column).toString()));
						}
					}
					else
					{
						if (importExcelFileType == 0) image = cv::imread(excelRecover->GetCellValue(progressPercentage, 4).toString().toStdString(), CV_LOAD_IMAGE_COLOR);
						ui->oneToN->setChecked(false);
						ui->imageExistsInBdd->setChecked(false);
						ui->imageExistsInBdd->setEnabled(false);
						ui->bddImageNames->setEnabled(false);
						ui->viewMatchesImageNameText->setEnabled(false);
						ui->viewTableImageNameText->setEnabled(false);
						if (importExcelFileType == 0)
						{
							ui->logPlainText->appendHtml(tr("Found %1 key points in the second image").arg(excelRecover->GetCellValue(progressPercentage, column - 12).toString()));
							ui->logPlainText->appendHtml(tr("Detection time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 11).toString()));
							ui->logPlainText->appendHtml(tr("Description time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 10).toString()));
							if (!excelRecover->GetCellValue(progressPercentage, column - 9).toString().isEmpty()) ui->logPlainText->appendHtml(tr("Clustering time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 9).toString()));
							ui->logPlainText->appendHtml(tr("Matching time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 8).toString()));
							ui->logPlainText->appendHtml(tr("Total time: %1(s)").arg(excelRecover->GetCellValue(progressPercentage, column - 7).toString()));
							if (bestImageScore >= scoreThreshold)
								ui->logPlainText->appendHtml(tr("Matching score = <b>%1</b><b style='color:green'> &ge; </b>%2").arg(QString::number(bestImageScore)).arg(QString::number(scoreThreshold)));
							else ui->logPlainText->appendHtml(tr("Matching score = <b>%1</b><b style='color:red'> &#60; </b>%2").arg(QString::number(bestImageScore)).arg(QString::number(scoreThreshold)));
						}
					}
					if (importExcelFileType == 0)
					{
						displayImage(image, 2);
						emit taskPercentageComplete(progressPercentage * 100 / nbTasks);
						ui->viewMatchesGoodMatchesText->setText("<b style='color:green'>" + QString::number(acceptedMatches) + "</b>/" + QString::number(acceptedMatches + rejectedMatches) + " = <b style = 'color:green'>" + QString::number(goodProbability) + "</b>%");
						ui->viewMatchesBadMatchesText->setText("<b style='color:red'>" + QString::number(rejectedMatches) + "</b>/" + QString::number(acceptedMatches + rejectedMatches) + " = <b style = 'color:red'>" + QString::number(badProbability) + "</b>%");
						ui->viewMatchesAverageMatchesText->setText(QString::number(bestImageAverage));
						ui->viewMatchesScoreMatchesText->setText("<b>" + QString::number(bestImageScore) + "</b>");
						//if (!excelRecover->GetCellValue(progressPercentage, column - 12).toString().isEmpty()) importTable(ui->spinBox->text().toInt());
						exist = true;
						emit taskPercentageComplete(100);
						QMessageBox::information(this, tr("Import Excel Success!"), tr("The test N°: %1 has been imported with success !").arg(QString(ui->spinBox->text())));
						ui->logPlainText->appendHtml("--------------------------------------------------------------------------------------------------------------------------------------------------------------------");
						break;
					}
					else
					{
						takeTestType = 1;
						disconnect(this, SIGNAL(taskPercentageComplete(int)), taskProgressDialog, SLOT(setValue(int)));
						takeTest();
						taskProgressDialog->setLabelText(tr("Processing ...")+" (" + QString::number(progressPercentage/2)+"/"+QString::number(nbTasks/2)+")");
						connect(this, SIGNAL(taskPercentageComplete(int)), taskProgressDialog, SLOT(setValue(int)));
						if (methodIndex == 5) progressPercentage++;
					}
				}
				emit taskPercentageComplete(progressPercentage * 100 / nbTasks);
				progressPercentage++;
			}
		}
		if (importExcelFileType == 0)
		{
			if (!exist) QMessageBox::warning(this, tr("Import Excel Error!"), tr("Please check the number that has been entered because no ID matches this number !"));
		}
		else if (!taskIsCanceled){
			QMessageBox::information(this, tr("Import Input file!"), tr("The execution of all commands has been finished with success !"));
			//ui->logPlainText->appendHtml(tr("<b style='color:blue'>The execution of all commands has been finished with success !</b>"));
		}
		excelRecover->~ExcelManager();
		system("taskkill /im EXCEL.EXE /f");
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Error while importing from Excel file!"), e.what());
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
		ui->eliminationNoTest->setChecked(true);
		ui->eliminationLoweRatioText->setEnabled(false);
	}
	else
	{
		if (value == "In reverse matching test")
		{
			ui->eliminationInversMatches->setChecked(true);
			ui->eliminationLoweRatioText->setEnabled(false);
		}
		else
		{
			if (!value.isEmpty())
			{
				ui->eliminationLoweRatio->setChecked(true);
				ui->eliminationLoweRatioText->setEnabled(true);

				QRegExp rx("(\\:)"); //RegEx for ':'
				QStringList matcher = value.split(rx);
				ui->eliminationLoweRatioText->setText(matcher[1]);
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
	//const QString tableName = QStandardPaths::DocumentsLocation + "\\Palmprint Registration\\" + QString::number(cpt) + "\\table.xlsx";
	//system("taskkill /fi \"WINDOWTITLE eq table.xlsx - Excel\" /f");
	//QAxObject *excelApplication = new QAxObject("Excel.Application", 0);
	//if (excelApplication == nullptr) throw std::invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");
	//excelApplication->dynamicCall("SetVisible(bool)", false); // display excel
	//excelApplication->setProperty("DisplayAlerts", 0); // disable alerts
	//QAxObject *workbooks = excelApplication->querySubObject("Workbooks");
	//QAxObject *workbook = workbooks->querySubObject("Add");
	//QAxObject *sheets = workbook->querySubObject("Worksheets");
	//QAxObject *sheet = sheets->querySubObject("Item(int)", 1);
	//setTableValue(sheet, 1, 1, "Coordinate X1");
	//setTableValue(sheet, 1, 2, "Coordinate Y1");
	//setTableValue(sheet, 1, 3, "Coordinate X2");
	//setTableValue(sheet, 1, 4, "Coordinate Y2");
	//setTableValue(sheet, 1, 5, "Distance");
	//setTableValue(sheet, 1, 6, "Accepted/Rejected");
	//for (int i = 0; i < rowsCount; i++)
	//{
	//	for (int j = 0; j < 6; j++)
	//	{
	//		setTableValue(sheet, i + 2, j + 1, ui->viewTable->model()->data(model->index(i, j, QModelIndex())).toString());
	//	}
	//}
	//workbook->dynamicCall("SaveAs (const QString&)", tableName);
	//workbook->dynamicCall("Close (Boolean)", true);
	//excelApplication->dynamicCall("Quit (void)");
}

void MainWindow::importTable(int identifierNumber) {
	//system("taskkill /fi \"WINDOWTITLE eq table.xlsx - Excel\" /f");
	//const QString tableName = QStandardPaths::DocumentsLocation + "\\Palmprint Registration\\" + QString::number(identifierNumber) + "\\table.xlsx";
	//QAxObject *excelApplication = new QAxObject("Excel.Application", 0);
	//if (excelApplication == nullptr) throw std::invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");
	//excelApplication->dynamicCall("SetVisible(bool)", false); // display excel
	//excelApplication->setProperty("DisplayAlerts", 0); // disable alerts
	//QAxObject *workbooks = excelApplication->querySubObject("Workbooks");
	//QAxObject *workbook = workbooks->querySubObject("Open(const QString&)", tableName);
	//QAxObject *sheets = workbook->querySubObject("Worksheets");
	//QAxObject *sheet = sheets->querySubObject("Item(int)", 1);
	//QAxObject *usedrange = sheet->querySubObject("UsedRange");
	//QAxObject *rows = usedrange->querySubObject("Rows");
	//int rowsCount = rows->property("Count").toInt();

	//initializeTable();
	//QStandardItem *value;

	//for (int i = 2; i <= rowsCount; i++)
	//{
	//	for (int j = 1; j <= 6; j++)
	//	{
	//		value = new QStandardItem(QString(GetTableValue(sheet, i, j)));
	//		model->setItem(i - 2, j - 1, value);
	//		if (j == 6)
	//		{
	//			if (GetTableValue(sheet, i, 6) == "Accepted")
	//			{
	//				value->setData(QColor(Qt::black), Qt::TextColorRole);
	//				value->setData(QColor(Qt::green), Qt::BackgroundRole);
	//			}
	//			else
	//			{
	//				value->setData(QColor(Qt::black), Qt::TextColorRole);
	//				value->setData(QColor(Qt::red), Qt::BackgroundRole);
	//			}
	//		}
	//	}
	//}
	//workbook->dynamicCall("Close (Boolean)", true);
	//excelApplication->dynamicCall("Quit (void)");
	//ui->viewTable->setModel(model);
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

		if (x[i] == 1 && y[i] != 0) {
			// add the text label at the top:
			QCPItemText *textLabel = new QCPItemText(ui->rankkGraphWidget);
			textLabel->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
			textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
			textLabel->setText("Rank-1= " + QString::number(y[i]) + "%");
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
	ui->rankkGraphWidget->xAxis->setRange(0, x[x.size() - 1]);
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
		xFMR.push_back(it->first*100); // threshold
		yFMR.push_back(static_cast<float>(it->second.first) / static_cast<float>(it->second.second) * 100); // nbFM / nbExists %
	}
	for (it = FNMR_dataFromExcel.begin(); it != FNMR_dataFromExcel.end(); ++it)
	{
		xFNMR.push_back(it->first*100); // threshold
		yFNMR.push_back(static_cast<float>(it->second.first) / static_cast<float>(it->second.second) * 100); // nbFM / nbExists %
	}

	double *xFMRmaxValue = std::max_element(xFMR.begin(), xFMR.end()), *xFNMRmaxValue = std::max_element(xFNMR.begin(), xFNMR.end());
	double *xFMRminValue = std::min_element(xFMR.begin(), xFMR.end()), *xFNMRminValue = std::min_element(xFNMR.begin(), xFNMR.end());
	/*raven::cSpline FMRspline(xFMR.toStdVector(), yFMR.toStdVector());
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
	}*/

	// spline graph
	ui->eerGraphWidget->addGraph();
	ui->eerGraphWidget->graph(0)->setData(xFMR, yFMR);
	ui->eerGraphWidget->graph(0)->setName("FMR");
	ui->eerGraphWidget->addGraph();
	ui->eerGraphWidget->graph(1)->setData(xFNMR, yFNMR);
	ui->eerGraphWidget->graph(1)->setName("FNMR");
	// create graph and assign data to it:
	//ui->eerGraphWidget->addGraph();
	//ui->eerGraphWidget->graph(2)->setData(xFMR, yFMR);
	//ui->eerGraphWidget->addGraph();
	//ui->eerGraphWidget->graph(3)->setData(xFNMR, yFNMR);
	// give the axes some labels:
	ui->eerGraphWidget->xAxis->setLabel(tr("Threshold (x100)"));
	ui->eerGraphWidget->yAxis->setLabel(tr("Percent %"));
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
	ui->eerGraphWidget->graph(0)->selectionDecorator()->setPen(QPen(QColor(120, 140, 250), 2));
	ui->eerGraphWidget->graph(1)->selectionDecorator()->setPen(QPen(QColor(120, 250, 120), 2));
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
