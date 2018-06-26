#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindow mainWindow;

	mainWindow.trs.load(":/MainWindow/palmprintregistration_fr");
	app.installTranslator(&mainWindow.trs);
	
	InitializeDualMode(true);
	bool isConsole = false;
	isConsole = (argc > 1 && !strcmp(argv[1], "-c"));
	/*if (!isConsole){
		std::cout << "Do you want to start the application with console mode?" << std::endl;
		std::cout << "[y/n] ";
		char c = std::getchar();
		isConsole = (c == 'y' || c == 'Y');
	*/
	if (isConsole) {
		std::cout << "Starting console app ..." << std::endl;
		mainWindow.launchInCMD();
		std::cout << "Finished..." << std::endl;
	}
	else {
		mainWindow.show();
		return app.exec();
	}

}
