#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    bool isConsole = (argc>1);

	QApplication app(argc, argv);
	MainWindow mainWindow;
	if (isConsole) {
		InitializeDualMode(true);
		std::cout << "Starting console app ..." << std::endl;
		std::ifstream excelFile(argv[1]);
		if (!excelFile) {
			std::cout << "Error while trying to open: '" << argv[1] << "'" << std::endl;
			exit(-1);
		}
		else {
			std::cout << "File opened successfully" << std::endl;
			mainWindow.launchInCMD(argv[1]);
			std::cout << "Finished..." << std::endl;
			exit(0);
		}
	}
	else {
		mainWindow.show();
		return app.exec();
	}
}
