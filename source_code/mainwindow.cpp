#include "mainwindow.h"
#include "ui_mainwindow.h"

// Images Declaration
	cv::Mat firstImg, secondImg;
	std::vector<cv::Mat> setImgs;

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
	double /*segmentationTime,*/ detectionTime, descriptionTime, directMatchingTime, inverseMatchingTime, bestMatchingTime;
// Others
	int kBestMatches;
	std::string directoryPath;
	bool oneToN;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	// cusomizing ToolTips :
	qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white;}");
    ui->descriptorFreakSelectedPairsText->setPlaceholderText("Ex: 1 2 11 22 154 256...");

	this->setWindowState(Qt::WindowMaximized);
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
    if(ui->surfBruteForceCheck->isChecked())
    {
        ptrMatcher = new cv::BFMatcher(cv::NORM_L1);
    }
    else
    {
        ptrMatcher = new cv::FlannBasedMatcher();
    }

    ptrMatcher->match( firstImgDescriptor, secondImgDescriptor, directMatches );
    ptrMatcher->match( secondImgDescriptor, firstImgDescriptor, inverseMatches );

    // !!!!!!!!!!!!!!!!!!!!!!!!!hena yebda le pblm de difference entre hada w ta3 custom !!! !!! !!! !!
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
	
	// Binarization
	customisingBinarization(segmentationIndex);

	// Customising Segmentor...
	customisingSegmentor(segmentationIndex);
	
	// Customising Detector...	
	customisingDetector(detectorIndex, detectorName);

	//Only detection
	//return;

	// Customising Descriptor...
	customisingDescriptor(descriptorIndex, descriptorName);

	//Only detection & description
	//return;

	// Clustering descriptor
	double clusteringTime = (double)cv::getTickCount();
	if (true){ //Klustering
		std::vector<cv::Mat> matrix;
		if (oneToN) {
			matrix = { firstImgDescriptor };
			matrix.insert(matrix.end(), setImgsDescriptors.begin(), setImgsDescriptors.end());
		}
		else matrix = { firstImgDescriptor, secondImgDescriptor };
		try{
			double compactness = clusteringIntoKClusters(matrix, 40);
		}catch (...){
			ui->logPlainText->appendHtml("<i style='color:red'>The number of clusters is too high, try another!</i>");
			return;
		}

		// Write Kmeans parameters
		cv::FileStorage fs("params/kmeans_params.yaml", cv::FileStorage::WRITE);
		fs << "Labels" << labels;
		fs << "Centers" << centers;
		fs.release();
	}
	clusteringTime = ((double)cv::getTickCount() - clusteringTime) / cv::getTickFrequency();
	ui->logPlainText->appendPlainText("clustering time: " + QString::number(clusteringTime) + " (s)");

	// Customising Matcher...
	customisingMatcher(matcherIndex, matcherName);

	// Find the matching points
	matching();

	ui->logPlainText->appendPlainText("Total time: " + QString::number(detectionTime + descriptionTime + bestMatchingTime) + " (s)");

	return;

	calculateBestMatches();
}

void MainWindow::on_firstImgBtn_pressed()
{
	QString str = QFileDialog::getOpenFileName(0, ("Select the 1st Image"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->firstImgText->setText(str);
}

void MainWindow::on_secondImgBtn_pressed()
{
	QString str = (ui->selectFolder->isChecked()) ? QFileDialog::getExistingDirectory(0, ("Select a Folder"), QDir::currentPath()) : QFileDialog::getOpenFileName(0, ("Select the 2nd Image"), QDir::currentPath());
	if (!str.trimmed().isEmpty())
		ui->secondImgText->setText(str);
}

void MainWindow::on_pushButton_pressed()
{
	// Read Images ...
	if (!readFirstImage()) return;
	oneToN = ui->selectFolder->isChecked();
	if (oneToN){
		if (!readSetOfImages()) return;
	}
	else {
		if (!readSecondImage()) return;
	}

	// Create a test folder ...
	if (!createTestFolder()) return;
	
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
	/*QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save Log File"), "palmprint_registration_log_file",
		tr("Excel Workbook (*.xlsx);;All Files (*)"));
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
	}*/

	try
	{
        const QString fileName = "palmprint_registration_log_file.xlsx";

        //ExcelExportHelper helper;

        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y %H.%M");
        auto str = oss.str();
        QString curtime = QString::fromStdString(str);

        /*helper.Open(fileName);
        helper.SetCellValue(1, 1, ui->matcherTabs->tabText(ui->matcherTabs->currentIndex()));
        helper.SetCellValue(1, 2, curtime);*/
        QAxObject* excel = new QAxObject("Excel.Application");
        QAxObject* workbooks = excel->querySubObject("Workbooks");
        QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", fileName);
        QAxObject* worksheet = workbook->querySubObject("Worksheets(int)", 0);

        switch (ui->allMethodsTabs->currentIndex())
        {
        case 0:
        {
            // SIFT
            QAxObject* siftSheet = worksheet->querySubObject( "Item(int)", 1 );

            QAxObject* siftUsedrange = siftSheet->querySubObject("UsedRange");

            QAxObject* siftRows = siftUsedrange->querySubObject("Rows");

            int intRows = siftRows->property("Count").toInt();

            QAxObject* siftContThreshText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 1);
            siftContThreshText->setProperty("Value", ui->siftContThreshText->text());

            QAxObject* siftEdgeThreshText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 2);
            siftEdgeThreshText->setProperty("Value", ui->siftEdgeThreshText->text());

            QAxObject* siftNumFeatText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 3);
            siftNumFeatText->setProperty("Value", ui->siftNumFeatText->text());

            QAxObject* siftNumOctText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 4);
            siftNumOctText->setProperty("Value", ui->siftNumOctText->text());

            QAxObject* siftSigmaText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 5);
            siftSigmaText->setProperty("Value", ui->siftSigmaText->text());

            QAxObject* siftBruteForceCheck = excel->querySubObject("Cells(Int, Int)", intRows + 1, 6);
            siftBruteForceCheck->setProperty("Value", ui->siftBruteForceCheck->isChecked());

            break;
        }
        case 1:
        {
            // SURF
            QAxObject* surfSheet = worksheet->querySubObject( "Item(int)", 2 );

            QAxObject* surfUsedrange = surfSheet->querySubObject("UsedRange");

            QAxObject* surfRows = surfUsedrange->querySubObject("Rows");

            int intRows = surfRows->property("Count").toInt();

            QAxObject* surfHessianThreshText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 1);
            surfHessianThreshText->setProperty("Value", ui->surfHessianThreshText->text());

            QAxObject* surfNumOctavesText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 2);
            surfNumOctavesText->setProperty("Value", ui->surfNumOctavesText->text());

            QAxObject* surfNumOctLayersText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 3);
            surfNumOctLayersText->setProperty("Value", ui->surfNumOctLayersText->text());

            QAxObject* surfExtendedText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 4);
            surfExtendedText->setProperty("Value", ui->surfExtendedText->isChecked());

            QAxObject* surfUprightText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 5);
            surfUprightText->setProperty("Value", ui->surfUprightText->isChecked());

            QAxObject* surfBruteForceCheck = excel->querySubObject("Cells(Int, Int)", intRows + 1, 6);
            surfBruteForceCheck->setProperty("Value", ui->surfBruteForceCheck->isChecked());

            break;
        }

        case 2:
        {
            // ORB
            QAxObject* orbSheet = worksheet->querySubObject( "Item(int)", 3 );
            qDebug () << orbSheet->property("Name");

            QAxObject* orbUsedrange = orbSheet->querySubObject("UsedRange");

            QAxObject* orbRows = orbUsedrange->querySubObject("Rows");

            int intRows = orbRows->property("Count").toInt();

            QAxObject* orbNumFeatText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 1);
            orbNumFeatText->setProperty("Value", ui->orbNumFeatText->text());

            QAxObject* orbScaleFactText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 2);
            orbScaleFactText->setProperty("Value", ui->orbScaleFactText->text());

            QAxObject* orbNumLevelsText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 3);
            orbNumLevelsText->setProperty("Value", ui->orbNumLevelsText->text());

            QAxObject* orbEdgeThreshText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 4);
            orbEdgeThreshText->setProperty("Value", ui->orbEdgeThreshText->text());

            QAxObject* orbFirstLevText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 5);
            orbFirstLevText->setProperty("Value", ui->orbFirstLevText->text());

            QAxObject* orbWTAKText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 6);
            orbWTAKText->setProperty("Value", ui->orbWTAKText->text());

            QAxObject* scoreType = excel->querySubObject("Cells(Int, Int)", intRows + 1, 7);
            scoreType->setProperty("Value", ui->orbScoreHarrisRadioBtn->isChecked());

            QAxObject* orbPatchSizeText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 8);
            orbPatchSizeText->setProperty("Value", ui->orbPatchSizeText->text());

            break;
        }
        case 3:
        {
            // BRISK
            QAxObject* briskSheet = worksheet->querySubObject( "Item(int)", 4 );

            QAxObject* briskUsedrange = briskSheet->querySubObject("UsedRange");

            QAxObject* briskRows = briskUsedrange->querySubObject("Rows");

            int intRows = briskRows->property("Count").toInt();

            QAxObject* briskPatternScaleText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 1);
            briskPatternScaleText->setProperty("Value", ui->briskPatternScaleText->text());

            QAxObject* briskOctavesText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 2);
            briskOctavesText->setProperty("Value", ui->briskOctavesText->text());

            QAxObject* briskThreshText = excel->querySubObject("Cells(Int, Int)", intRows + 1, 3);
            briskThreshText->setProperty("Value", ui->briskThreshText->text());

            break;
        }
        default:
        {
            // Custom
            QAxObject* customSheet = worksheet->querySubObject( "Item(int)", 5 );

            QAxObject* customUsedrange = customSheet->querySubObject("UsedRange");

            QAxObject* customRows = customUsedrange->querySubObject("Rows");

            int intRows = customRows->property("Count").toInt();

            QAxObject* segmentationTabs = excel->querySubObject("Cells(Int, Int)", intRows + 1, 1);
            segmentationTabs->setProperty("Value", ui->segmentationTabs->tabText(ui->segmentationTabs->currentIndex()));

            int segmentationIndex = ui->segmentationTabs->currentIndex();
            int detectorIndex = ui->detectorTabs->currentIndex();
            int descriptorIndex = ui->descriptorTabs->currentIndex();
            int matcherIndex = ui->matcherTabs->currentIndex();

            // Customising Segmentor...
            switch (segmentationIndex)
            {
            case 0:
                break;
            case 1:
                cv::threshold(firstImg, firstImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); //127, 255, cv::THRESH_BINARY);
                cv::threshold(secondImg, secondImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU); //127, 255, cv::THRESH_BINARY);
                // Skeletonization
                // This will create more unique and stronger interest points
                firstImg = skeletonization(firstImg);
                secondImg = skeletonization(secondImg);
                break;
                //....
           default:
                return;
                break;
            }

            QAxObject* detectorTabs = excel->querySubObject("Cells(Int, Int)", intRows + 1, 3);
            detectorTabs->setProperty("Value", ui->detectorTabs->tabText(ui->detectorTabs->currentIndex()));

            QAxObject* descriptorTabs = excel->querySubObject("Cells(Int, Int)", intRows + 1, 5);
            descriptorTabs->setProperty("Value", ui->descriptorTabs->tabText(ui->descriptorTabs->currentIndex()));

            QAxObject* matcherTabs = excel->querySubObject("Cells(Int, Int)", intRows + 1, 7);
            matcherTabs->setProperty("Value", ui->matcherTabs->tabText(ui->matcherTabs->currentIndex()));

            break;


        }
        }
        excel->setProperty("Visible", true);
        workbooks->querySubObject("SaveAs()");
        workbooks->querySubObject("Close()");
        excel->querySubObject("Quit()");
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, "Error - Demo", e.what());
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

bool MainWindow::readFirstImage(){
	// Read Image ...
	if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 4)){
		// Custom && OpponentColor
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
	}
	else{
		firstImg = cv::imread(ui->firstImgText->text().toStdString(), cv::IMREAD_GRAYSCALE);//or CV_LOAD_IMAGE_GRAYSCALE
	}

	// Check if the Images are loaded correctly ...
	if (firstImg.empty())
	{
		ui->logPlainText->appendHtml("<b style='color:red'>Error while trying to read the 1st input file!</b>");
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
	if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 4)){
		// Custom && OpponentColor
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), CV_LOAD_IMAGE_COLOR);
	}
	else{
		secondImg = cv::imread(ui->secondImgText->text().toStdString(), cv::IMREAD_GRAYSCALE); //or CV_LOAD_IMAGE_GRAYSCALE
	}

	// Check if the Images are loaded correctly ...
	if (secondImg.empty())
	{
		ui->logPlainText->appendHtml("<b style='color:red'>Error while trying to read the 2nd input file!</b>");
		return false;
	}

	if ((secondImg.cols < 100) && (secondImg.rows < 100))
	{
		cv::resize(secondImg, secondImg, cv::Size(), 200 / secondImg.rows, 200 / secondImg.cols);
	}
	displayImage(secondImg, 2);

	return true;
}

bool MainWindow::readSetOfImages(){
	// Read Data Set of Images ...
	std::string datapath = ui->secondImgText->text().toStdString()+"/";
	int nbFile = fileCounter(datapath, "(", ")", ".jpg");
	for (int f = 1; f <= nbFile; f++){
		std::ostringstream filename;
		filename << datapath << "(" << f << ").jpg";
		//open the file
		cv::Mat img;
		if (ui->opponentColor->isChecked() && (ui->allMethodsTabs->currentIndex() == 4)){
			// Custom && OpponentColor
			img = cv::imread(filename.str(), CV_LOAD_IMAGE_COLOR);
		}
		else{
			img = cv::imread(filename.str(), cv::IMREAD_GRAYSCALE); //or CV_LOAD_IMAGE_GRAYSCALE
		}

		// Check if the Images are loaded correctly ...
		if (img.empty())
		{
			ui->logPlainText->appendHtml("<b style='color:red'>Error while trying to read the " + QString::number(f) + "th input of '" + QString::fromStdString(datapath) + "' folder!</b>");
			return false;
		}
		if ((img.cols < 100) && (img.rows < 100))
		{
			cv::resize(img, img, cv::Size(), 200 / img.rows, 200 / img.cols);
		}
		//displayImage(img, 2);
		setImgs.push_back(img);
	}
	if (setImgs.size() == 0) {
		showError("Read Images", "There is no image in the folder: " + directoryPath, "Make sure that the folder contains one or more images with correct names!");
		return false;
	}
	else if (setImgs.size() == 1){
		// one to one image
		secondImg = setImgs[setImgs.size() - 1];
		oneToN = false;
	}
	// display the last image
	displayImage(setImgs[setImgs.size() - 1], 2);
	return true;
}

bool MainWindow::createTestFolder(){
	// create a new folder test
	if (CreateDirectory(L"Tests", NULL) || ERROR_ALREADY_EXISTS == GetLastError()){
		std::ifstream infile;
		FILE *file;
		/*first check if the file exists...*/
		infile.open("Tests/next.txt");
		int cpt;
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
	try{
		firstImgKeypoints.clear(); secondImgKeypoints.clear(); setImgsKeypoints.clear();
		firstImgDescriptor.release(); secondImgDescriptor.release(); setImgsDescriptors.clear();
		firstEnhancedImage.release(); secondEnhancedImage.release(); setEnhancedImages.clear();
		firstSegmentedImage.release(); secondSegmentedImage.release(); setSegmentedImages.clear();
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

double MainWindow::clusteringIntoKClusters(std::vector<cv::Mat> features_vector, int k){
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
	//cv::Mat imgFeatureShow;
	//cv::drawKeypoints(featureMat, (first_second == 1) ? firstImgKeypoints : secondImgKeypoints, imgFeatureShow, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

	QGraphicsScene *featureScene = new QGraphicsScene();
	QImage featureImg((const uchar *)featureMat.data, featureMat.cols, featureMat.rows, featureMat.step, QImage::Format_RGB888);
	featureScene->addPixmap(QPixmap::fromImage(featureImg));

	QGraphicsView *myUiScene = (first_second == 1) ? ui->viewKeyPoints1 : ui->viewKeyPoints2;
	myUiScene->setScene(featureScene);
	myUiScene->fitInView(featureScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
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
			int i = 0;
			for (cv::Mat &img : setImgs){
				i++;
				try{ localThreshold::binarisation(img, 41, 56); }
				catch (cv::Exception e){
					showError("Binarization", "Error in the " + std::to_string(i) + " image", e.msg);
				}
			}
		}
		else localThreshold::binarisation(secondImg, 41, 56);

		double threshold = ui->segmentationThresholdText->text().toFloat();
		cv::threshold(firstImg, firstImg, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		if (oneToN)
			for (cv::Mat &img : setImgs) {
				cv::threshold(img, img, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
			}
		else cv::threshold(secondImg, secondImg, threshold, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
		
		//ideka::binOptimisation(firstImg);
		//ideka::binOptimisation(secondImg);
		cv::imwrite(directoryPath + "f-1_Binarization.bmp", firstImg);
		if (oneToN) cv::imwrite(directoryPath + "l-1_Binarization.bmp", setImgs[setImgs.size()-1]);
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
			int i = 0;
			for (cv::Mat img : setImgs){
				setImgs[i] = skeletonization(img);
				i++;
			}
			cv::imwrite(directoryPath + "l-2_Morphological Skeleton.bmp", setImgs[setImgs.size() - 1]);
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
			for (cv::Mat &img : setImgs){
				ZhangSuen::thinning(img);
			}
			cv::imwrite(directoryPath + "l-2_Zhang-Suen Thinning.bmp", setImgs[setImgs.size() - 1]);
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
			int i = 0;
			for (cv::Mat img : setImgs){
				setImgs[i] = Image_processing::thinning(img, setEnhancedImages[i], setSegmentedImages[i]);
				setImgs[i].convertTo(setImgs[i], CV_8UC3, 255);
				i++;
			}
			cv::imwrite(directoryPath + "l-2_Lin-Hong Thinning.bmp", setImgs[setImgs.size() - 1]);
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
			for (cv::Mat &img : setImgs){
				GuoHall::thinning(img);
			}
			cv::imwrite(directoryPath + "l-2_Guo-Hall Thinning.bmp", setImgs[setImgs.size() - 1]);
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
			int i = 0;
			for (cv::Mat &img : setImgs){
				setMinutiaes[i] = Image_processing::extracting(img, setEnhancedImages[i], setSegmentedImages[i], img);
				i++;
			}
		}
		else secondMinutiae = Image_processing::extracting(secondImg, secondEnhancedImage, secondSegmentedImage, secondImg);
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();

		writeKeyPoints(firstImg, firstMinutiae, 1, "f-3_Minutiae");
		if (oneToN) writeKeyPoints(setImgs[setImgs.size() - 1], setMinutiaes[setMinutiaes.size() - 1], 2, "l-3_Minutiae");
		else writeKeyPoints(secondImg, secondMinutiae, 2, "s-3_Minutiae");

		// images must be segmented if not Minutiae will be empty
		try{
			// Adapt Minutiaes to KeyPoints (this will affect magnitudes and angles to minutiaes)
			MinutiaeToKeyPointAdapter adapter;
			// also we must add the Adapting time to detection time
			detectionTime += adapter.adapt(firstMinutiae);
			if (oneToN)
			{
				for (int i = 0; i < setImgs.size(); i++){
					detectionTime += adapter.adapt(setMinutiaes[i]);
				}
			}
			else detectionTime += adapter.adapt(secondMinutiae);
			for (Minutiae minutiae : firstMinutiae)
			{
				firstImgKeypoints.push_back(minutiae);
			}
			if (oneToN)
			{
				setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>());
				int i = 0;
				for (std::vector<Minutiae> minutiaes : setMinutiaes){
					for (Minutiae minutiae : minutiaes)
					{
						setImgsKeypoints[i].push_back(minutiae);
					}
					i++;
				}
			}
			else for (Minutiae minutiae : secondMinutiae)
			{
				secondImgKeypoints.push_back(minutiae);
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
		firstMinutiae = crossingNumber::getMinutiae(firstImg, ui->detectorMinutiae2BorderText->text().toInt());
		//Minutiae-filtering
		// slow with the second segmentation
		Filter::filterMinutiae(firstMinutiae);
		if (oneToN)
		{
			setMinutiaes = std::vector<std::vector<Minutiae>>(setImgs.size(), std::vector<Minutiae>());
			int i = 0;
			for (cv::Mat &img : setImgs){
				setMinutiaes[i] = crossingNumber::getMinutiae(img, ui->detectorMinutiae2BorderText->text().toInt());
				Filter::filterMinutiae(setMinutiaes[i]);
				i++;
			}
		}
		else {
			secondMinutiae = crossingNumber::getMinutiae(secondImg, ui->detectorMinutiae2BorderText->text().toInt());
			Filter::filterMinutiae(secondMinutiae);
		}
		detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();

		writeKeyPoints(firstImg, firstMinutiae, 1, "f-3_Minutiae2");
		if (oneToN)
		{
			int i = 0;
			for (cv::Mat img : setImgs){
				writeKeyPoints(img, setMinutiaes[i], 2, "l-3_Minutiae2");
				i++;
			}
		}
		else writeKeyPoints(secondImg, secondMinutiae, 2, "s-3_Minutiae2");

		// images must be segmented if not Minutiae will be empty
		try{
			// Adapt Minutiaes to KeyPoints (this will affect magnitudes and angles to minutiaes)
			MinutiaeToKeyPointAdapter adapter;
			// also we must add the Adapting time to detection time
			detectionTime += adapter.adapt(firstMinutiae);
			if (oneToN)
			{
				for (std::vector<Minutiae> &minutiaes : setMinutiaes){
					detectionTime += adapter.adapt(minutiaes);
				}
			}
			else detectionTime += adapter.adapt(secondMinutiae);
			for (Minutiae minutiae : firstMinutiae)
			{
				firstImgKeypoints.push_back(minutiae);
			}
			if (oneToN)
			{
				setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>());
				int i = 0;
				for (std::vector<Minutiae> minutiaes : setMinutiaes){
					for (Minutiae minutiae : minutiaes)
					{
						setImgsKeypoints[i].push_back(minutiae);
					}
					i++;
				}
			}
			else for (Minutiae minutiae : secondMinutiae)
			{
				secondImgKeypoints.push_back(minutiae);
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
			int i = 0;
			for (cv::Mat img : setImgs){
				harrisCorners(img, setImgsKeypoints[i], ui->detectorHarrisThresholdText->text().toFloat());
				i++;
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
				int i = 0;
				for (cv::Mat img : setImgs){
					cv::FASTX(img, setImgsKeypoints[i], ui->detectorFastThresholdText->text().toInt(),
						ui->detectorFastNonmaxSuppressionCheck->isChecked(),
						ui->detectorFastTypeText->currentIndex());
					i++;
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
				setImgsKeypoints = std::vector<std::vector<cv::KeyPoint>>(setImgs.size(), std::vector<cv::KeyPoint>());
				int i = 0;
				for (cv::Mat &img : setImgs){
					ptrDetector->detect(img, setImgsKeypoints[i]);
					i++;
				}
			}else ptrDetector->detect(secondImg, secondImgKeypoints);
			detectionTime = ((double)cv::getTickCount() - detectionTime) / cv::getTickFrequency();
		}
		catch (...){
			ui->logPlainText->appendHtml("<b style='color:red'>Please select the right " + QString::fromStdString(detectorName) + " detector parameters, or use the defaults!.</b>");
			return;
		}
		writeKeyPoints(firstImg, firstImgKeypoints, 1, "f-3_KeyPoints");
		if (oneToN) writeKeyPoints(setImgs[setImgs.size() - 1], setImgsKeypoints[setImgs.size() - 1], 2, "l-3_KeyPoints");
		else writeKeyPoints(secondImg, secondImgKeypoints, 2, "s-3_KeyPoints");
	}
	if (noKeyPoints("first", firstImgKeypoints) || (!oneToN && noKeyPoints("second", secondImgKeypoints))) return;
	if (oneToN) noKeyPoints("last", setImgsKeypoints[setImgs.size() - 1]);
	ui->logPlainText->appendPlainText("detection time: " + QString::number(detectionTime) + " (s)");
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

	if (ui->opponentColor->isChecked())
		//OpponentColor
		ptrDescriptor = new cv::OpponentColorDescriptorExtractor(ptrDescriptor);
	// Write the parameters
	writeToFile("descriptorOppCol_" + descriptorName, ptrDescriptor);

	try{
		// Aissa !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! affichinna firstImgKeypoints avant et après pour voir si compute va les changer ou pas!!!!
		descriptionTime = (double)cv::getTickCount();
		ptrDescriptor->compute(firstImg, firstImgKeypoints, firstImgDescriptor);
		if (oneToN)
		{
			setImgsDescriptors = std::vector<cv::Mat>(setImgs.size(), cv::Mat());
			int i = 0;
			for (cv::Mat &img : setImgs){
				ptrDescriptor->compute(img, setImgsKeypoints[i], setImgsDescriptors[i]);
				i++;
			}
		}
		else ptrDescriptor->compute(secondImg, secondImgKeypoints, secondImgDescriptor);
		descriptionTime = ((double)cv::getTickCount() - descriptionTime) / cv::getTickFrequency();
		ui->logPlainText->appendPlainText("description time: " + QString::number(descriptionTime) + " (s)");
	}
	catch (...){
		if (descriptorName == "FREAK")ui->logPlainText->appendHtml("<b style='color:red'>Please select the right pair indexes within the FREAK descriptor, or just leave it!.</b><br>(For more details read Section(4.2) in: <i>A. Alahi, R. Ortiz, and P. Vandergheynst. FREAK: Fast Retina Keypoint. In IEEE Conference on Computer Vision and Pattern Recognition, 2012.</i>)");
		else ui->logPlainText->appendHtml("<b style='color:red'>Please select the right " + QString::fromStdString(descriptorName) + " descriptor parameters, or use the defaults!.</b>");
		return;
	}
}

void MainWindow::customisingMatcher(int matcherIndex, std::string matcherName){
	// creating Matcher...
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
}

void MainWindow::matching(){
	// Start matching ...
	try{
		if (kBestMatches < 2 || ui->matcherInlierInversMatches->isChecked()){
			// direct and reverse set of the best matches (simple match)
			directMatchingTime = (double)cv::getTickCount();
			if (oneToN){
				ptrMatcher->add(setImgsDescriptors);
				ptrMatcher->match(firstImgDescriptor, directMatches);
			}
			else ptrMatcher->match(firstImgDescriptor, secondImgDescriptor, directMatches);
			directMatchingTime = ((double)cv::getTickCount() - directMatchingTime) / cv::getTickFrequency();

			inverseMatchingTime = (double)cv::getTickCount();
			if (oneToN){
				/*ptrMatcher->add(setImgsDescriptors);
				ptrMatcher->match(firstImgDescriptor, directMatches);*/
			}
			else ptrMatcher->match(secondImgDescriptor, firstImgDescriptor, inverseMatches);
			inverseMatchingTime = ((double)cv::getTickCount() - inverseMatchingTime) / cv::getTickFrequency();

			bestMatchingTime = std::min(directMatchingTime, inverseMatchingTime);
		}
		else { // ui->matcherInlierLoweRatio->isChecked()
			// only direct set of the k best matches
			directMatchingTime = (double)cv::getTickCount();
			if (oneToN){
				ptrMatcher->add(setImgsDescriptors);
				ptrMatcher->knnMatch(firstImgDescriptor, knnMatches, kBestMatches, cv::Mat(), false);
			}
			else ptrMatcher->knnMatch(firstImgDescriptor, secondImgDescriptor, knnMatches, kBestMatches, cv::Mat(), false);
			directMatchingTime = ((double)cv::getTickCount() - directMatchingTime) / cv::getTickFrequency();

			bestMatchingTime = directMatchingTime;
		}
	}
	catch (cv::Exception e){
		// For example Flann-Based doesn't work with Brief desctiptor extractor
		// And also, some descriptors must be used with specific NORM_s
		ui->logPlainText->appendHtml("<b style='color:red'>Cannot match descriptors because of an incompatible combination!, try another one.</b>");
		return;
	}

	ui->logPlainText->appendPlainText("matching time: " + QString::number(bestMatchingTime) + " (s)");

}

void MainWindow::calculateBestMatches(){
	// Calculate the number of best matches between two sets of matches
	int bestMatchesCount = 0;
	float sumDistances = 0;
	QStandardItemModel *model = new QStandardItemModel(2, 5, this); //2 Rows and 5 Columns
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
				ui->viewTable->setModel(model);

				sumDistances += directMatches[i].distance;
				bestMatches.push_back(directMatches[i]);
				bestMatchesCount++;
			}
		}
		ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount) + "/" + QString::number(std::min(directMatches.size(), inverseMatches.size())));
	}
	else if (kBestMatches == 2 && ui->matcherInlierLoweRatio->isChecked()){
		// knn = 2
		// Lowe's ratio test = 0.7
		for (std::vector<cv::DMatch> match : knnMatches)
		{
			if (match[0].distance < 0.7*match[1].distance){
				bestMatches.push_back(match[0]);
				bestMatchesCount++;
			}
		}
		ui->logPlainText->appendPlainText("Number of Best key point matches = " + QString::number(bestMatchesCount) + "/" + QString::number(knnMatches.size()));
	}
	else {// knn > 2
		for (std::vector<cv::DMatch> match : knnMatches)
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
	cv::imwrite(directoryPath + "output.png", bestImgMatches);

	//-- Draw only "good" matches
	QGraphicsScene *scene = new QGraphicsScene();
	QImage dest((const uchar *)bestImgMatches.data, bestImgMatches.cols, bestImgMatches.rows, bestImgMatches.step, QImage::Format_RGB888);
	//dest.bits();
	scene->addPixmap(QPixmap::fromImage(dest));
	ui->viewMatches->setScene(scene);
	ui->viewMatches->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
	ui->viewMatches->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
}

int MainWindow::fileCounter(std::string dir, std::string prefix, std::string suffix, std::string extension){
	int returnedCount = 0;
	int possibleMax = 5000000; //some number you can expect.

	for (int istarter = 1; istarter < possibleMax; istarter++){
		std::string fileName = "";
		fileName.append(dir);
		fileName.append(prefix);
		fileName.append(std::to_string(istarter));
		fileName.append(suffix);
		fileName.append(extension);
		bool status = fileExistenceCheck(fileName);
		returnedCount = istarter;

		if (!status)
			break;
	}

	return returnedCount-1;
}

ExcelExportHelper::ExcelExportHelper(bool closeExcelOnExit)
{
	m_closeExcelOnExit = closeExcelOnExit;
	m_excelApplication = nullptr;
	m_sheet = nullptr;
	m_sheets = nullptr;
	m_workbook = nullptr;
	m_workbooks = nullptr;
	m_excelApplication = nullptr;

	m_excelApplication = new QAxObject("Excel.Application", 0);//{00024500-0000-0000-C000-000000000046}

	if (m_excelApplication == nullptr)
		throw std::invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");

	m_excelApplication->dynamicCall("SetVisible(bool)", false); // hide excel
	m_excelApplication->setProperty("DisplayAlerts", 0); // disable alerts

	m_workbooks = m_excelApplication->querySubObject("Workbooks");
	m_workbook = m_workbooks->querySubObject("Add");
	m_sheets = m_workbook->querySubObject("Worksheets");
	m_sheet = m_sheets->querySubObject("Add");
}

void ExcelExportHelper::SetCellValue(int lineIndex, int columnIndex, const QString& value)
{
    QAxObject *cell = m_sheet->querySubObject("Cells(int,int)", lineIndex, columnIndex);
	cell->setProperty("Value", value);
	delete cell;
}

void ExcelExportHelper::Open(const QString& fileName)
{
    if (fileName == "")
		throw std::invalid_argument("'fileName' is empty!");
	if (fileName.contains("/"))
		throw std::invalid_argument("'/' character in 'fileName' is not supported by excel!");

    if (!QFile::exists(fileName))
	{
        /*if (!QFile::remove(fileName))
		{
			throw new std::exception(QString("Failed to remove file '%1'").arg(fileName).toStdString().c_str());
        }*/
        m_workbooks->dynamicCall("Open (const QString&)", fileName);
        m_sheets->querySubObject("Worksheets(int)", 1);
    }

    //m_workbook->dynamicCall("SaveAs (const QString&)", fileName);
}

ExcelExportHelper::~ExcelExportHelper()
{
	if (m_excelApplication != nullptr)
	{
		if (!m_closeExcelOnExit)
		{
			m_excelApplication->setProperty("DisplayAlerts", 1);
			m_excelApplication->dynamicCall("SetVisible(bool)", true);
		}

		if (m_workbook != nullptr && m_closeExcelOnExit)
		{
			m_workbook->dynamicCall("Close (Boolean)", true);
			m_excelApplication->dynamicCall("Quit (void)");
		}
	}

	delete m_sheet;
	delete m_sheets;
	delete m_workbook;
	delete m_workbooks;
	delete m_excelApplication;
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