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
		char currentPath[] = "C:/Users/Nabil/Desktop/PFE_Project/PalmprintRegistration/PalmprintRegistration/Tests/";
		std::ifstream excelFile(strcat(currentPath, argv[1]));
		if (!excelFile) {
			std::cout << "Error while trying to open: '" << currentPath << "'" << std::endl;
			std::getchar();
			return 1;
		}
		else {
			std::cout << "File opened successfully" << std::endl;
		}
	}
	else mainWindow.show();

	return app.exec();
}
