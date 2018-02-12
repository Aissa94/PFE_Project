/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionDestroy_All_Windows;
    QAction *actionRun;
    QAction *actionClear_Log;
    QAction *actionSave_Log_File_As;
    QAction *actionAbout_Qt;
    QAction *actionAbout_Me;
    QWidget *centralWidget;
    QGridLayout *gridLayout_6;
    QGridLayout *gridLayout_3;
    QPlainTextEdit *logPlainText;
    QLabel *label_6;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget_2;
    QWidget *tab_5;
    QGridLayout *gridLayout_9;
    QLabel *label_3;
    QLineEdit *firstImgText;
    QPushButton *firstImgBtn;
    QLabel *label_4;
    QPushButton *secondImgBtn;
    QLineEdit *secondImgText;
    QPushButton *pushButton;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLineEdit *siftEdgeThreshText;
    QLineEdit *siftContThreshText;
    QLabel *label;
    QLabel *label_5;
    QLineEdit *siftNumFeatText;
    QLabel *label_7;
    QLineEdit *siftNumOctText;
    QLineEdit *siftSigmaText;
    QLabel *label_8;
    QCheckBox *siftBruteForceCheck;
    QWidget *tab_2;
    QGridLayout *gridLayout_8;
    QGridLayout *gridLayout_7;
    QLineEdit *surfHessianThreshText;
    QLabel *label_10;
    QLineEdit *surfNumOctavesText;
    QLabel *label_9;
    QLineEdit *surfExtendedText;
    QLabel *label_11;
    QLineEdit *surfNumOctLayersText;
    QLabel *label_13;
    QLabel *label_12;
    QLineEdit *surfUprightText;
    QCheckBox *surfBruteForceCheck;
    QWidget *tab_3;
    QGridLayout *gridLayout_14;
    QGridLayout *gridLayout_15;
    QTabWidget *tabWidget_3;
    QWidget *tab_10;
    QGridLayout *gridLayout_19;
    QGridLayout *gridLayout_18;
    QLabel *label_33;
    QLineEdit *briefSiftEdgeThreshText;
    QLineEdit *briefSiftContThreshText;
    QLabel *label_34;
    QLabel *label_35;
    QLineEdit *briefSiftNumFeatText;
    QLabel *label_36;
    QLineEdit *briefSiftNumOctText;
    QLineEdit *briefSiftSigmaText;
    QLabel *label_37;
    QWidget *tab_11;
    QGridLayout *gridLayout_17;
    QGridLayout *gridLayout_16;
    QLabel *label_28;
    QLineEdit *briefSurfNumOctavesText;
    QLineEdit *briefSurfHessianThreshText;
    QLabel *label_29;
    QLabel *label_30;
    QLineEdit *briefSurfNumOctLayersText;
    QLabel *label_31;
    QLineEdit *briefSurfExtendedText;
    QLabel *label_32;
    QLineEdit *briefSurfUprightText;
    QLabel *label_26;
    QLabel *label_27;
    QLineEdit *briefDescLenText;
    QWidget *tab_4;
    QGridLayout *gridLayout_13;
    QGridLayout *gridLayout_12;
    QLabel *label_18;
    QLabel *label_20;
    QLabel *label_24;
    QLabel *label_19;
    QLabel *label_25;
    QLabel *label_23;
    QLabel *label_21;
    QRadioButton *orbScoreHarrisRadioBtn;
    QLabel *label_22;
    QRadioButton *orbScoreFastRadioBtn;
    QLineEdit *orbFirstLevText;
    QLineEdit *orbEdgeThreshText;
    QLineEdit *orbNumLevelsText;
    QLineEdit *orbScaleFactText;
    QLineEdit *orbNumFeatText;
    QLineEdit *orbWTAKText;
    QLineEdit *orbPatchSizeText;
    QWidget *tab_6;
    QGridLayout *gridLayout_20;
    QLabel *label_39;
    QLineEdit *briskThreshText;
    QLabel *label_44;
    QLineEdit *briskOctavesText;
    QLineEdit *briskPatternScaleText;
    QLabel *label_38;
    QTabWidget *tabWidget_4;
    QWidget *tab_8;
    QGraphicsView *graphicsView_2;
    QGridLayout *gridLayout_10;
    QWidget *tab_7;
    QGraphicsView *graphicsView_3;
    QGridLayout *gridLayout_11;
    QWidget *tabWidget_4Page1;
    QGraphicsView *graphicsView;
    QGridLayout *gridLayout_5;
    QWidget *tabWidget_4Page2;
    QTableView *tableView;
    QMenuBar *menuBar;
    QMenu *menuWindows;
    QMenu *menuFile;
    QMenu *menuHelp;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1147, 626);
        actionDestroy_All_Windows = new QAction(MainWindow);
        actionDestroy_All_Windows->setObjectName(QStringLiteral("actionDestroy_All_Windows"));
        actionRun = new QAction(MainWindow);
        actionRun->setObjectName(QStringLiteral("actionRun"));
        actionClear_Log = new QAction(MainWindow);
        actionClear_Log->setObjectName(QStringLiteral("actionClear_Log"));
        actionSave_Log_File_As = new QAction(MainWindow);
        actionSave_Log_File_As->setObjectName(QStringLiteral("actionSave_Log_File_As"));
        actionAbout_Qt = new QAction(MainWindow);
        actionAbout_Qt->setObjectName(QStringLiteral("actionAbout_Qt"));
        actionAbout_Me = new QAction(MainWindow);
        actionAbout_Me->setObjectName(QStringLiteral("actionAbout_Me"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_6 = new QGridLayout(centralWidget);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        logPlainText = new QPlainTextEdit(centralWidget);
        logPlainText->setObjectName(QStringLiteral("logPlainText"));
        logPlainText->setReadOnly(true);

        gridLayout_3->addWidget(logPlainText, 1, 0, 1, 1);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_3->addWidget(label_6, 0, 0, 1, 1);


        gridLayout_6->addLayout(gridLayout_3, 2, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tabWidget_2 = new QTabWidget(centralWidget);
        tabWidget_2->setObjectName(QStringLiteral("tabWidget_2"));
        tabWidget_2->setEnabled(true);
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        gridLayout_9 = new QGridLayout(tab_5);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        label_3 = new QLabel(tab_5);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_9->addWidget(label_3, 0, 0, 1, 1);

        firstImgText = new QLineEdit(tab_5);
        firstImgText->setObjectName(QStringLiteral("firstImgText"));

        gridLayout_9->addWidget(firstImgText, 0, 1, 1, 1);

        firstImgBtn = new QPushButton(tab_5);
        firstImgBtn->setObjectName(QStringLiteral("firstImgBtn"));

        gridLayout_9->addWidget(firstImgBtn, 0, 2, 1, 1);

        label_4 = new QLabel(tab_5);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_9->addWidget(label_4, 1, 0, 1, 1);

        secondImgBtn = new QPushButton(tab_5);
        secondImgBtn->setObjectName(QStringLiteral("secondImgBtn"));

        gridLayout_9->addWidget(secondImgBtn, 1, 2, 1, 1);

        secondImgText = new QLineEdit(tab_5);
        secondImgText->setObjectName(QStringLiteral("secondImgText"));

        gridLayout_9->addWidget(secondImgText, 1, 1, 1, 1);

        tabWidget_2->addTab(tab_5, QString());

        gridLayout->addWidget(tabWidget_2, 0, 0, 1, 1);

        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 0, 1, 1, 1);


        gridLayout_6->addLayout(gridLayout, 0, 0, 1, 1);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setEnabled(true);
        tabWidget->setAcceptDrops(false);
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_4 = new QGridLayout(tab);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        siftEdgeThreshText = new QLineEdit(tab);
        siftEdgeThreshText->setObjectName(QStringLiteral("siftEdgeThreshText"));

        gridLayout_2->addWidget(siftEdgeThreshText, 1, 1, 1, 1);

        siftContThreshText = new QLineEdit(tab);
        siftContThreshText->setObjectName(QStringLiteral("siftContThreshText"));

        gridLayout_2->addWidget(siftContThreshText, 0, 1, 1, 1);

        label = new QLabel(tab);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_5 = new QLabel(tab);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_5, 2, 0, 1, 1);

        siftNumFeatText = new QLineEdit(tab);
        siftNumFeatText->setObjectName(QStringLiteral("siftNumFeatText"));

        gridLayout_2->addWidget(siftNumFeatText, 2, 1, 1, 1);

        label_7 = new QLabel(tab);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_7, 3, 0, 1, 1);

        siftNumOctText = new QLineEdit(tab);
        siftNumOctText->setObjectName(QStringLiteral("siftNumOctText"));

        gridLayout_2->addWidget(siftNumOctText, 3, 1, 1, 1);

        siftSigmaText = new QLineEdit(tab);
        siftSigmaText->setObjectName(QStringLiteral("siftSigmaText"));

        gridLayout_2->addWidget(siftSigmaText, 4, 1, 1, 1);

        label_8 = new QLabel(tab);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_8, 4, 0, 1, 1);


        gridLayout_4->addLayout(gridLayout_2, 0, 0, 1, 1);

        siftBruteForceCheck = new QCheckBox(tab);
        siftBruteForceCheck->setObjectName(QStringLiteral("siftBruteForceCheck"));

        gridLayout_4->addWidget(siftBruteForceCheck, 1, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout_8 = new QGridLayout(tab_2);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_7 = new QGridLayout();
        gridLayout_7->setSpacing(6);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        surfHessianThreshText = new QLineEdit(tab_2);
        surfHessianThreshText->setObjectName(QStringLiteral("surfHessianThreshText"));

        gridLayout_7->addWidget(surfHessianThreshText, 0, 1, 1, 1);

        label_10 = new QLabel(tab_2);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(label_10, 0, 0, 1, 1);

        surfNumOctavesText = new QLineEdit(tab_2);
        surfNumOctavesText->setObjectName(QStringLiteral("surfNumOctavesText"));

        gridLayout_7->addWidget(surfNumOctavesText, 1, 1, 1, 1);

        label_9 = new QLabel(tab_2);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(label_9, 1, 0, 1, 1);

        surfExtendedText = new QLineEdit(tab_2);
        surfExtendedText->setObjectName(QStringLiteral("surfExtendedText"));

        gridLayout_7->addWidget(surfExtendedText, 3, 1, 1, 1);

        label_11 = new QLabel(tab_2);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(label_11, 2, 0, 1, 1);

        surfNumOctLayersText = new QLineEdit(tab_2);
        surfNumOctLayersText->setObjectName(QStringLiteral("surfNumOctLayersText"));

        gridLayout_7->addWidget(surfNumOctLayersText, 2, 1, 1, 1);

        label_13 = new QLabel(tab_2);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(label_13, 4, 0, 1, 1);

        label_12 = new QLabel(tab_2);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(label_12, 3, 0, 1, 1);

        surfUprightText = new QLineEdit(tab_2);
        surfUprightText->setObjectName(QStringLiteral("surfUprightText"));

        gridLayout_7->addWidget(surfUprightText, 4, 1, 1, 1);


        gridLayout_8->addLayout(gridLayout_7, 0, 0, 1, 1);

        surfBruteForceCheck = new QCheckBox(tab_2);
        surfBruteForceCheck->setObjectName(QStringLiteral("surfBruteForceCheck"));

        gridLayout_8->addWidget(surfBruteForceCheck, 1, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_14 = new QGridLayout(tab_3);
        gridLayout_14->setSpacing(6);
        gridLayout_14->setContentsMargins(11, 11, 11, 11);
        gridLayout_14->setObjectName(QStringLiteral("gridLayout_14"));
        gridLayout_15 = new QGridLayout();
        gridLayout_15->setSpacing(6);
        gridLayout_15->setObjectName(QStringLiteral("gridLayout_15"));
        tabWidget_3 = new QTabWidget(tab_3);
        tabWidget_3->setObjectName(QStringLiteral("tabWidget_3"));
        tab_10 = new QWidget();
        tab_10->setObjectName(QStringLiteral("tab_10"));
        gridLayout_19 = new QGridLayout(tab_10);
        gridLayout_19->setSpacing(6);
        gridLayout_19->setContentsMargins(11, 11, 11, 11);
        gridLayout_19->setObjectName(QStringLiteral("gridLayout_19"));
        gridLayout_18 = new QGridLayout();
        gridLayout_18->setSpacing(6);
        gridLayout_18->setObjectName(QStringLiteral("gridLayout_18"));
        label_33 = new QLabel(tab_10);
        label_33->setObjectName(QStringLiteral("label_33"));
        label_33->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_18->addWidget(label_33, 1, 0, 1, 1);

        briefSiftEdgeThreshText = new QLineEdit(tab_10);
        briefSiftEdgeThreshText->setObjectName(QStringLiteral("briefSiftEdgeThreshText"));

        gridLayout_18->addWidget(briefSiftEdgeThreshText, 1, 1, 1, 1);

        briefSiftContThreshText = new QLineEdit(tab_10);
        briefSiftContThreshText->setObjectName(QStringLiteral("briefSiftContThreshText"));

        gridLayout_18->addWidget(briefSiftContThreshText, 0, 1, 1, 1);

        label_34 = new QLabel(tab_10);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_18->addWidget(label_34, 0, 0, 1, 1);

        label_35 = new QLabel(tab_10);
        label_35->setObjectName(QStringLiteral("label_35"));
        label_35->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_18->addWidget(label_35, 2, 0, 1, 1);

        briefSiftNumFeatText = new QLineEdit(tab_10);
        briefSiftNumFeatText->setObjectName(QStringLiteral("briefSiftNumFeatText"));

        gridLayout_18->addWidget(briefSiftNumFeatText, 2, 1, 1, 1);

        label_36 = new QLabel(tab_10);
        label_36->setObjectName(QStringLiteral("label_36"));
        label_36->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_18->addWidget(label_36, 3, 0, 1, 1);

        briefSiftNumOctText = new QLineEdit(tab_10);
        briefSiftNumOctText->setObjectName(QStringLiteral("briefSiftNumOctText"));

        gridLayout_18->addWidget(briefSiftNumOctText, 3, 1, 1, 1);

        briefSiftSigmaText = new QLineEdit(tab_10);
        briefSiftSigmaText->setObjectName(QStringLiteral("briefSiftSigmaText"));

        gridLayout_18->addWidget(briefSiftSigmaText, 4, 1, 1, 1);

        label_37 = new QLabel(tab_10);
        label_37->setObjectName(QStringLiteral("label_37"));
        label_37->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_18->addWidget(label_37, 4, 0, 1, 1);


        gridLayout_19->addLayout(gridLayout_18, 0, 0, 1, 1);

        tabWidget_3->addTab(tab_10, QString());
        tab_11 = new QWidget();
        tab_11->setObjectName(QStringLiteral("tab_11"));
        gridLayout_17 = new QGridLayout(tab_11);
        gridLayout_17->setSpacing(6);
        gridLayout_17->setContentsMargins(11, 11, 11, 11);
        gridLayout_17->setObjectName(QStringLiteral("gridLayout_17"));
        gridLayout_16 = new QGridLayout();
        gridLayout_16->setSpacing(6);
        gridLayout_16->setObjectName(QStringLiteral("gridLayout_16"));
        label_28 = new QLabel(tab_11);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_16->addWidget(label_28, 1, 0, 1, 1);

        briefSurfNumOctavesText = new QLineEdit(tab_11);
        briefSurfNumOctavesText->setObjectName(QStringLiteral("briefSurfNumOctavesText"));

        gridLayout_16->addWidget(briefSurfNumOctavesText, 1, 1, 1, 1);

        briefSurfHessianThreshText = new QLineEdit(tab_11);
        briefSurfHessianThreshText->setObjectName(QStringLiteral("briefSurfHessianThreshText"));

        gridLayout_16->addWidget(briefSurfHessianThreshText, 0, 1, 1, 1);

        label_29 = new QLabel(tab_11);
        label_29->setObjectName(QStringLiteral("label_29"));
        label_29->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_16->addWidget(label_29, 0, 0, 1, 1);

        label_30 = new QLabel(tab_11);
        label_30->setObjectName(QStringLiteral("label_30"));
        label_30->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_16->addWidget(label_30, 2, 0, 1, 1);

        briefSurfNumOctLayersText = new QLineEdit(tab_11);
        briefSurfNumOctLayersText->setObjectName(QStringLiteral("briefSurfNumOctLayersText"));

        gridLayout_16->addWidget(briefSurfNumOctLayersText, 2, 1, 1, 1);

        label_31 = new QLabel(tab_11);
        label_31->setObjectName(QStringLiteral("label_31"));
        label_31->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_16->addWidget(label_31, 3, 0, 1, 1);

        briefSurfExtendedText = new QLineEdit(tab_11);
        briefSurfExtendedText->setObjectName(QStringLiteral("briefSurfExtendedText"));

        gridLayout_16->addWidget(briefSurfExtendedText, 3, 1, 1, 1);

        label_32 = new QLabel(tab_11);
        label_32->setObjectName(QStringLiteral("label_32"));
        label_32->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_16->addWidget(label_32, 4, 0, 1, 1);

        briefSurfUprightText = new QLineEdit(tab_11);
        briefSurfUprightText->setObjectName(QStringLiteral("briefSurfUprightText"));

        gridLayout_16->addWidget(briefSurfUprightText, 4, 1, 1, 1);


        gridLayout_17->addLayout(gridLayout_16, 0, 0, 1, 1);

        tabWidget_3->addTab(tab_11, QString());

        gridLayout_15->addWidget(tabWidget_3, 1, 0, 1, 1);

        label_26 = new QLabel(tab_3);
        label_26->setObjectName(QStringLiteral("label_26"));

        gridLayout_15->addWidget(label_26, 0, 0, 1, 1);


        gridLayout_14->addLayout(gridLayout_15, 1, 0, 1, 1);

        label_27 = new QLabel(tab_3);
        label_27->setObjectName(QStringLiteral("label_27"));

        gridLayout_14->addWidget(label_27, 2, 0, 1, 1);

        briefDescLenText = new QLineEdit(tab_3);
        briefDescLenText->setObjectName(QStringLiteral("briefDescLenText"));

        gridLayout_14->addWidget(briefDescLenText, 3, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        gridLayout_13 = new QGridLayout(tab_4);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        gridLayout_12 = new QGridLayout();
        gridLayout_12->setSpacing(6);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        label_18 = new QLabel(tab_4);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_18, 1, 0, 1, 1);

        label_20 = new QLabel(tab_4);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_20, 2, 0, 1, 1);

        label_24 = new QLabel(tab_4);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_24, 6, 0, 1, 1);

        label_19 = new QLabel(tab_4);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_19, 0, 0, 1, 1);

        label_25 = new QLabel(tab_4);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_25, 7, 0, 1, 1);

        label_23 = new QLabel(tab_4);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_23, 5, 0, 1, 1);

        label_21 = new QLabel(tab_4);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_21, 3, 0, 1, 1);

        orbScoreHarrisRadioBtn = new QRadioButton(tab_4);
        orbScoreHarrisRadioBtn->setObjectName(QStringLiteral("orbScoreHarrisRadioBtn"));
        orbScoreHarrisRadioBtn->setChecked(true);

        gridLayout_12->addWidget(orbScoreHarrisRadioBtn, 6, 1, 1, 1);

        label_22 = new QLabel(tab_4);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_12->addWidget(label_22, 4, 0, 1, 1);

        orbScoreFastRadioBtn = new QRadioButton(tab_4);
        orbScoreFastRadioBtn->setObjectName(QStringLiteral("orbScoreFastRadioBtn"));

        gridLayout_12->addWidget(orbScoreFastRadioBtn, 6, 2, 1, 1);

        orbFirstLevText = new QLineEdit(tab_4);
        orbFirstLevText->setObjectName(QStringLiteral("orbFirstLevText"));

        gridLayout_12->addWidget(orbFirstLevText, 4, 1, 1, 2);

        orbEdgeThreshText = new QLineEdit(tab_4);
        orbEdgeThreshText->setObjectName(QStringLiteral("orbEdgeThreshText"));

        gridLayout_12->addWidget(orbEdgeThreshText, 3, 1, 1, 2);

        orbNumLevelsText = new QLineEdit(tab_4);
        orbNumLevelsText->setObjectName(QStringLiteral("orbNumLevelsText"));

        gridLayout_12->addWidget(orbNumLevelsText, 2, 1, 1, 2);

        orbScaleFactText = new QLineEdit(tab_4);
        orbScaleFactText->setObjectName(QStringLiteral("orbScaleFactText"));

        gridLayout_12->addWidget(orbScaleFactText, 1, 1, 1, 2);

        orbNumFeatText = new QLineEdit(tab_4);
        orbNumFeatText->setObjectName(QStringLiteral("orbNumFeatText"));

        gridLayout_12->addWidget(orbNumFeatText, 0, 1, 1, 2);

        orbWTAKText = new QLineEdit(tab_4);
        orbWTAKText->setObjectName(QStringLiteral("orbWTAKText"));

        gridLayout_12->addWidget(orbWTAKText, 5, 1, 1, 2);

        orbPatchSizeText = new QLineEdit(tab_4);
        orbPatchSizeText->setObjectName(QStringLiteral("orbPatchSizeText"));

        gridLayout_12->addWidget(orbPatchSizeText, 7, 1, 1, 2);


        gridLayout_13->addLayout(gridLayout_12, 0, 0, 1, 1);

        tabWidget->addTab(tab_4, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        gridLayout_20 = new QGridLayout(tab_6);
        gridLayout_20->setSpacing(6);
        gridLayout_20->setContentsMargins(11, 11, 11, 11);
        gridLayout_20->setObjectName(QStringLiteral("gridLayout_20"));
        label_39 = new QLabel(tab_6);
        label_39->setObjectName(QStringLiteral("label_39"));
        label_39->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_20->addWidget(label_39, 1, 0, 1, 1);

        briskThreshText = new QLineEdit(tab_6);
        briskThreshText->setObjectName(QStringLiteral("briskThreshText"));

        gridLayout_20->addWidget(briskThreshText, 2, 1, 1, 2);

        label_44 = new QLabel(tab_6);
        label_44->setObjectName(QStringLiteral("label_44"));
        label_44->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_20->addWidget(label_44, 2, 0, 1, 1);

        briskOctavesText = new QLineEdit(tab_6);
        briskOctavesText->setObjectName(QStringLiteral("briskOctavesText"));

        gridLayout_20->addWidget(briskOctavesText, 1, 1, 1, 2);

        briskPatternScaleText = new QLineEdit(tab_6);
        briskPatternScaleText->setObjectName(QStringLiteral("briskPatternScaleText"));

        gridLayout_20->addWidget(briskPatternScaleText, 0, 1, 1, 2);

        label_38 = new QLabel(tab_6);
        label_38->setObjectName(QStringLiteral("label_38"));
        label_38->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_20->addWidget(label_38, 0, 0, 1, 1);

        tabWidget->addTab(tab_6, QString());

        gridLayout_6->addWidget(tabWidget, 1, 0, 1, 1);

        tabWidget_4 = new QTabWidget(centralWidget);
        tabWidget_4->setObjectName(QStringLiteral("tabWidget_4"));
        tabWidget_4->setEnabled(true);
        tabWidget_4->setMouseTracking(false);
        tab_8 = new QWidget();
        tab_8->setObjectName(QStringLiteral("tab_8"));
        graphicsView_2 = new QGraphicsView(tab_8);
        graphicsView_2->setObjectName(QStringLiteral("graphicsView_2"));
        graphicsView_2->setEnabled(true);
        graphicsView_2->setGeometry(QRect(0, 10, 551, 541));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(graphicsView_2->sizePolicy().hasHeightForWidth());
        graphicsView_2->setSizePolicy(sizePolicy);
        graphicsView_2->setFrameShape(QFrame::NoFrame);
        gridLayout_10 = new QGridLayout(graphicsView_2);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        tabWidget_4->addTab(tab_8, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QStringLiteral("tab_7"));
        graphicsView_3 = new QGraphicsView(tab_7);
        graphicsView_3->setObjectName(QStringLiteral("graphicsView_3"));
        graphicsView_3->setEnabled(true);
        graphicsView_3->setGeometry(QRect(0, 10, 551, 541));
        sizePolicy.setHeightForWidth(graphicsView_3->sizePolicy().hasHeightForWidth());
        graphicsView_3->setSizePolicy(sizePolicy);
        graphicsView_3->setFrameShape(QFrame::NoFrame);
        gridLayout_11 = new QGridLayout(graphicsView_3);
        gridLayout_11->setSpacing(6);
        gridLayout_11->setContentsMargins(11, 11, 11, 11);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        tabWidget_4->addTab(tab_7, QString());
        tabWidget_4Page1 = new QWidget();
        tabWidget_4Page1->setObjectName(QStringLiteral("tabWidget_4Page1"));
        tabWidget_4Page1->setMinimumSize(QSize(555, 560));
        tabWidget_4Page1->setMaximumSize(QSize(555, 560));
        graphicsView = new QGraphicsView(tabWidget_4Page1);
        graphicsView->setObjectName(QStringLiteral("graphicsView"));
        graphicsView->setEnabled(true);
        graphicsView->setGeometry(QRect(0, 10, 551, 541));
        sizePolicy.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
        graphicsView->setSizePolicy(sizePolicy);
        graphicsView->setFrameShape(QFrame::NoFrame);
        gridLayout_5 = new QGridLayout(graphicsView);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        tabWidget_4->addTab(tabWidget_4Page1, QString());
        tabWidget_4Page2 = new QWidget();
        tabWidget_4Page2->setObjectName(QStringLiteral("tabWidget_4Page2"));
        tabWidget_4Page2->setMinimumSize(QSize(555, 560));
        tableView = new QTableView(tabWidget_4Page2);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setGeometry(QRect(0, 10, 551, 541));
        sizePolicy.setHeightForWidth(tableView->sizePolicy().hasHeightForWidth());
        tableView->setSizePolicy(sizePolicy);
        tableView->setAutoFillBackground(false);
        tableView->setFrameShape(QFrame::NoFrame);
        tableView->setProperty("showDropIndicator", QVariant(true));
        tableView->setShowGrid(true);
        tableView->setSortingEnabled(true);
        tabWidget_4->addTab(tabWidget_4Page2, QString());

        gridLayout_6->addWidget(tabWidget_4, 0, 1, 3, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1147, 21));
        menuWindows = new QMenu(menuBar);
        menuWindows->setObjectName(QStringLiteral("menuWindows"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindow->setMenuBar(menuBar);
#ifndef QT_NO_SHORTCUT
        label_4->setBuddy(label_4);
#endif // QT_NO_SHORTCUT

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuWindows->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuWindows->addAction(actionDestroy_All_Windows);
        menuFile->addAction(actionRun);
        menuFile->addSeparator();
        menuFile->addAction(actionClear_Log);
        menuFile->addAction(actionSave_Log_File_As);
        menuHelp->addAction(actionAbout_Qt);
        menuHelp->addAction(actionAbout_Me);

        retranslateUi(MainWindow);

        tabWidget_2->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);
        tabWidget_3->setCurrentIndex(1);
        tabWidget_4->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "OpenCV Feature Points Comparison", 0));
        actionDestroy_All_Windows->setText(QApplication::translate("MainWindow", "Destroy All Windows", 0));
        actionDestroy_All_Windows->setShortcut(QApplication::translate("MainWindow", "Ctrl+Backspace", 0));
        actionRun->setText(QApplication::translate("MainWindow", "Run", 0));
        actionRun->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0));
        actionClear_Log->setText(QApplication::translate("MainWindow", "Clear Log", 0));
        actionSave_Log_File_As->setText(QApplication::translate("MainWindow", "Save Log File As ...", 0));
        actionAbout_Qt->setText(QApplication::translate("MainWindow", "About Qt", 0));
        actionAbout_Me->setText(QApplication::translate("MainWindow", "About Me", 0));
        label_6->setText(QApplication::translate("MainWindow", "Log :", 0));
        label_3->setText(QApplication::translate("MainWindow", "First Image :", 0));
        firstImgText->setText(QApplication::translate("MainWindow", "C:/Users/Aissa/Documents/Visual Studio 2013/Projects/PFE_Project/source_code/images/box1.png", 0));
        firstImgBtn->setText(QApplication::translate("MainWindow", "Browse", 0));
        label_4->setText(QApplication::translate("MainWindow", "Second Image :", 0));
        secondImgBtn->setText(QApplication::translate("MainWindow", "Browse", 0));
        secondImgText->setText(QApplication::translate("MainWindow", "C:/Users/Aissa/Documents/Visual Studio 2013/Projects//PFE_Project/source_code/images/box2.png", 0));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_5), QApplication::translate("MainWindow", "Image && Image", 0));
        pushButton->setText(QApplication::translate("MainWindow", "Run", 0));
#ifndef QT_NO_TOOLTIP
        tabWidget->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("MainWindow", "Edge Threshold :", 0));
        siftEdgeThreshText->setText(QApplication::translate("MainWindow", "10", 0));
        siftContThreshText->setText(QApplication::translate("MainWindow", "0.04", 0));
        label->setText(QApplication::translate("MainWindow", "Contrast Threshold :", 0));
        label_5->setText(QApplication::translate("MainWindow", "Number of Features :", 0));
        siftNumFeatText->setText(QApplication::translate("MainWindow", "0", 0));
        label_7->setText(QApplication::translate("MainWindow", "Number of Octave Layers :", 0));
        siftNumOctText->setText(QApplication::translate("MainWindow", "3", 0));
        siftSigmaText->setText(QApplication::translate("MainWindow", "1.6", 0));
        label_8->setText(QApplication::translate("MainWindow", "Sigma :", 0));
        siftBruteForceCheck->setText(QApplication::translate("MainWindow", "Use Brute Force Matching", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "SIFT", 0));
        surfHessianThreshText->setText(QApplication::translate("MainWindow", "100", 0));
        label_10->setText(QApplication::translate("MainWindow", "Hessian Threshold :", 0));
        surfNumOctavesText->setText(QApplication::translate("MainWindow", "4", 0));
        label_9->setText(QApplication::translate("MainWindow", "Number of Octaves :", 0));
        surfExtendedText->setText(QApplication::translate("MainWindow", "yes", 0));
        label_11->setText(QApplication::translate("MainWindow", "Number of Octave Layers :", 0));
        surfNumOctLayersText->setText(QApplication::translate("MainWindow", "2", 0));
        label_13->setText(QApplication::translate("MainWindow", "Upright :", 0));
        label_12->setText(QApplication::translate("MainWindow", "Extended :", 0));
        surfUprightText->setText(QApplication::translate("MainWindow", "no", 0));
        surfBruteForceCheck->setText(QApplication::translate("MainWindow", "Use Brute Force Matching", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "SURF", 0));
        label_33->setText(QApplication::translate("MainWindow", "Edge Threshold :", 0));
        briefSiftEdgeThreshText->setText(QApplication::translate("MainWindow", "10", 0));
        briefSiftContThreshText->setText(QApplication::translate("MainWindow", "0.04", 0));
        label_34->setText(QApplication::translate("MainWindow", "Contrast Threshold :", 0));
        label_35->setText(QApplication::translate("MainWindow", "Number of Features :", 0));
        briefSiftNumFeatText->setText(QApplication::translate("MainWindow", "0", 0));
        label_36->setText(QApplication::translate("MainWindow", "Number of Octave Layers :", 0));
        briefSiftNumOctText->setText(QApplication::translate("MainWindow", "3", 0));
        briefSiftSigmaText->setText(QApplication::translate("MainWindow", "1.6", 0));
        label_37->setText(QApplication::translate("MainWindow", "Sigma :", 0));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_10), QApplication::translate("MainWindow", "SIFT", 0));
        label_28->setText(QApplication::translate("MainWindow", "Number of Octaves :", 0));
        briefSurfNumOctavesText->setText(QApplication::translate("MainWindow", "4", 0));
        briefSurfHessianThreshText->setText(QApplication::translate("MainWindow", "100", 0));
        label_29->setText(QApplication::translate("MainWindow", "Hessian Threshold :", 0));
        label_30->setText(QApplication::translate("MainWindow", "Number of Octave Layers :", 0));
        briefSurfNumOctLayersText->setText(QApplication::translate("MainWindow", "2", 0));
        label_31->setText(QApplication::translate("MainWindow", "Extended :", 0));
        briefSurfExtendedText->setText(QApplication::translate("MainWindow", "yes", 0));
        label_32->setText(QApplication::translate("MainWindow", "Upright :", 0));
        briefSurfUprightText->setText(QApplication::translate("MainWindow", "no", 0));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_11), QApplication::translate("MainWindow", "SURF", 0));
        label_26->setText(QApplication::translate("MainWindow", "Key Point Finding Algorithm :", 0));
        label_27->setText(QApplication::translate("MainWindow", "Descriptor Length in Bytes :", 0));
        briefDescLenText->setText(QApplication::translate("MainWindow", "32", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "BRIEF", 0));
        label_18->setText(QApplication::translate("MainWindow", "Scale Factor :", 0));
        label_20->setText(QApplication::translate("MainWindow", "Number of Levels :", 0));
        label_24->setText(QApplication::translate("MainWindow", "Score Type :", 0));
        label_19->setText(QApplication::translate("MainWindow", "Number of Features :", 0));
        label_25->setText(QApplication::translate("MainWindow", "Patch Size :", 0));
        label_23->setText(QApplication::translate("MainWindow", "WTA K :", 0));
        label_21->setText(QApplication::translate("MainWindow", "Edge Threshold :", 0));
        orbScoreHarrisRadioBtn->setText(QApplication::translate("MainWindow", "Harris", 0));
        label_22->setText(QApplication::translate("MainWindow", "First Level :", 0));
        orbScoreFastRadioBtn->setText(QApplication::translate("MainWindow", "FAST", 0));
        orbFirstLevText->setText(QApplication::translate("MainWindow", "0", 0));
        orbEdgeThreshText->setText(QApplication::translate("MainWindow", "31", 0));
        orbNumLevelsText->setText(QApplication::translate("MainWindow", "8", 0));
        orbScaleFactText->setText(QApplication::translate("MainWindow", "1.2", 0));
        orbNumFeatText->setText(QApplication::translate("MainWindow", "500", 0));
        orbWTAKText->setText(QApplication::translate("MainWindow", "2", 0));
        orbPatchSizeText->setText(QApplication::translate("MainWindow", "31", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWindow", "ORB", 0));
        label_39->setText(QApplication::translate("MainWindow", "Number of Octaves :", 0));
        briskThreshText->setText(QApplication::translate("MainWindow", "30", 0));
        label_44->setText(QApplication::translate("MainWindow", "Threshold Score :", 0));
        briskOctavesText->setText(QApplication::translate("MainWindow", "3", 0));
        briskPatternScaleText->setText(QApplication::translate("MainWindow", "1.0", 0));
        label_38->setText(QApplication::translate("MainWindow", "Pattern Scale :", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("MainWindow", "BRISK", 0));
#ifndef QT_NO_STATUSTIP
        tabWidget_4->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        tabWidget_4->setTabText(tabWidget_4->indexOf(tab_8), QApplication::translate("MainWindow", "Keypoints 1", 0));
        tabWidget_4->setTabText(tabWidget_4->indexOf(tab_7), QApplication::translate("MainWindow", "Keypoints 2", 0));
        tabWidget_4->setTabText(tabWidget_4->indexOf(tabWidget_4Page1), QApplication::translate("MainWindow", "Matching", 0));
        tabWidget_4->setTabText(tabWidget_4->indexOf(tabWidget_4Page2), QApplication::translate("MainWindow", "Table", 0));
        menuWindows->setTitle(QApplication::translate("MainWindow", "Windows", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
