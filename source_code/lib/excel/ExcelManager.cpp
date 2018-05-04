#include <QFile>
#include <QColor>

#include "ExcelManager.h"

ExcelManager::ExcelManager(bool closeExcelOnExit, const QString& fileName, int numSheet)
{
	m_closeExcelOnExit = closeExcelOnExit;
	m_excelApplication = nullptr;
	m_rows = nullptr;
	m_columns = nullptr;
	m_usedrange = nullptr;
	m_sheet = nullptr;
	m_sheets = nullptr;
	m_workbook = nullptr;
	m_workbooks = nullptr;
	m_excelApplication = nullptr;

	system("taskkill /fi \"WINDOWTITLE eq palmprint_registration_log_file.xlsx - Excel\" /f");
	//WinExec("taskkill /fi \"WINDOWTITLE eq palmprint_registration_log_file.xlsx - Excel\" /f", SW_HIDE);
	m_excelApplication = new QAxObject("Excel.Application", 0);//{00024500-0000-0000-C000-000000000046}

	if (m_excelApplication == nullptr)
		throw std::invalid_argument("Failed to initialize interop with Excel (probably Excel is not installed)");

	m_excelApplication->dynamicCall("SetVisible(bool)", false); // display excel
	m_excelApplication->setProperty("DisplayAlerts", 0); // disable alerts

	m_workbooks = m_excelApplication->querySubObject("Workbooks");
	if (!QFile::exists(fileName))
	{

		m_workbook = m_workbooks->querySubObject("Add");
		m_sheets = m_workbook->querySubObject("Worksheets");

		m_sheet_custom = m_sheets->querySubObject("Item(int)", 1);
		m_sheet_custom->setProperty("Name", "Custom");

		SetNewCellValueFirst(m_sheet_custom);
		SetNewCellValue(m_sheet_custom, 8, 1, "Segmentation");
		SetNewCellValue(m_sheet_custom, 9, 1, "Parameters of Segmentation");
		SetNewCellValue(m_sheet_custom, 10, 1, "Detector");
		SetNewCellValue(m_sheet_custom, 11, 5, "Parameters of Detector");
		SetNewCellValue(m_sheet_custom, 16, 1, "Descriptor");
		SetNewCellValue(m_sheet_custom, 17, 5, "Parameters of Descriptor");
		SetNewCellValue(m_sheet_custom, 22, 1, "Matcher");
		SetNewCellValue(m_sheet_custom, 23, 8, "Parameters of Matcher");
		SetNewCellValue(m_sheet_custom, 31, 1, "Opponent Color");
		SetNewCellValueLast(m_sheet_custom, 32);

		m_sheet_brisk = AddNewSheet("BRISK");

		SetNewCellValueFirst(m_sheet_brisk);
		SetNewCellValue(m_sheet_brisk, 8, 1, "Pattern Scale");
		SetNewCellValue(m_sheet_brisk, 9, 1, "Number of Octaves");
		SetNewCellValue(m_sheet_brisk, 10, 1, "Threshold");
		SetNewCellValueLast(m_sheet_brisk, 11);

		m_sheet_orb = AddNewSheet("ORB");

		SetNewCellValueFirst(m_sheet_orb);
		SetNewCellValue(m_sheet_orb, 8, 1, "Number of Features");
		SetNewCellValue(m_sheet_orb, 9, 1, "Scale Factor");
		SetNewCellValue(m_sheet_orb, 10, 1, "Number of Levels");
		SetNewCellValue(m_sheet_orb, 11, 1, "Edge Threshold");
		SetNewCellValue(m_sheet_orb, 12, 1, "First Level");
		SetNewCellValue(m_sheet_orb, 13, 1, "WTA K");
		SetNewCellValue(m_sheet_orb, 14, 1, "Score Type");
		SetNewCellValue(m_sheet_orb, 15, 1, "Patch Size");
		SetNewCellValueLast(m_sheet_orb, 16);

		m_sheet_surf = AddNewSheet("SURF");

		SetNewCellValueFirst(m_sheet_surf);
		SetNewCellValue(m_sheet_surf, 8, 1, "Hessian Threshold");
		SetNewCellValue(m_sheet_surf, 9, 1, "Number of Octaves");
		SetNewCellValue(m_sheet_surf, 10, 1, "Number of Octave Layers");
		SetNewCellValue(m_sheet_surf, 11, 1, "Extended");
		SetNewCellValue(m_sheet_surf, 12, 1, "Features Orientation");
		SetNewCellValue(m_sheet_surf, 13, 1, "Brute Force Matching");
		SetNewCellValueLast(m_sheet_surf, 14);

		m_sheet_sift = AddNewSheet("SIFT");

		SetNewCellValueFirst(m_sheet_sift);
		SetNewCellValue(m_sheet_sift, 8, 1, "Contrast Threshold");
		SetNewCellValue(m_sheet_sift, 9, 1, "Edge Threshold");
		SetNewCellValue(m_sheet_sift, 10, 1, "Number of Features");
		SetNewCellValue(m_sheet_sift, 11, 1, "Number of Octave Layers");
		SetNewCellValue(m_sheet_sift, 12, 1, "Sigma");
		SetNewCellValue(m_sheet_sift, 13, 1, "Brute Force Matching");
		SetNewCellValueLast(m_sheet_sift, 14);

		m_workbook->dynamicCall("SaveAs (const QString&)", fileName);
	}
	else
	{
		m_workbook = m_workbooks->querySubObject("Open(const QString&)", fileName);
		m_sheets = m_workbook->querySubObject("Worksheets");
	}

	if (numSheet != 0) {
		GetIntRows(numSheet);
	}
}

void ExcelManager::GetIntRows(int numSheet)
{
	m_sheet = m_sheets->querySubObject("Item(int)", numSheet);
	m_usedrange = m_sheet->querySubObject("UsedRange");
	m_rows = m_usedrange->querySubObject("Rows");
	intRows = m_rows->property("Count").toInt();
}

QVariant ExcelManager::GetCellValue(int rowIndex, int columnIndex)
{
	QVariant value;
	QAxObject* cell = m_sheet->querySubObject("Cells(Int, Int)", rowIndex, columnIndex);
	value = cell->dynamicCall("Value()");
	delete cell;
	return value;
}

QString ExcelManager::GetSheetName()
{
	return m_sheet->dynamicCall("Name()").toString();
}

void ExcelManager::SetCellValue(int columnIndex, int type, const QString& value)
{
	if (type == 1) mergeRowsCells(columnIndex);
	QAxObject *cell = m_sheet->querySubObject("Cells(int,int)", intRows + 1, columnIndex);
	cell->setProperty("Value", value);
	cell->setProperty("HorizontalAlignment", -4108);
	cell->setProperty("VerticalAlignment", -4108);
	if (type == 2) cell->setProperty("ColumnWidth", value.size() + 4);
	if (columnIndex == 1) {
		setCellBackgroundColored(cell, 255, 235, 156);
		setCellBordersColored(cell, 255, 0, 0);
		if (type == 1)
		{
			QAxObject *nextCell = m_sheet->querySubObject("Cells(int,int)", intRows + 2, columnIndex);
			setCellBordersColored(nextCell, 255, 0, 0);
			delete nextCell;
		}
		cell->setProperty("RowHeight", 18);
	}
	delete cell;
}

void ExcelManager::SetCellValueSecondRow(int columnIndex, const QString& value)
{
	QAxObject *cell = m_sheet->querySubObject("Cells(int,int)", intRows + 2, columnIndex);
	cell->setProperty("Value", value);
	cell->setProperty("HorizontalAlignment", -4108);
	delete cell;
}

int ExcelManager::getSheetCount()
{
	return intRows;
}

int ExcelManager::getColumnsCount()
{
	m_columns = m_usedrange->querySubObject("Columns");
	intColumns = m_columns->property("Count").toInt();
	return intColumns;
}

QString ExcelManager::IndexesToRange(int rowIndex, int columnIndex, int length)
{
	QString cellRange;

	if (columnIndex <= 26) {
		cellRange.append(QChar(columnIndex - 1 + 'A'));
		cellRange.append(QString::number(rowIndex));
		if ((columnIndex + length) <= 26) {
			cellRange.append(":");
			cellRange.append(QChar(columnIndex + length - 2 + 'A'));
			cellRange.append(QString::number(rowIndex));
		}
		else {
			cellRange.append(":A");
			cellRange.append(QChar(columnIndex + length - 28 + 'A'));
			cellRange.append(QString::number(rowIndex));
		}
	}
	else {
		cellRange.append("A");
		cellRange.append(QChar(columnIndex - 27 + 'A'));
		cellRange.append(QString::number(rowIndex));
		cellRange.append(":A");
		cellRange.append(QChar(columnIndex + length - 28 + 'A'));
		cellRange.append(QString::number(rowIndex));
	}

	return cellRange;
}

void ExcelManager::mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn)
{
	QString cell;
	cell.append(QChar(topLeftColumn - 1 + 'A'));
	cell.append(QString::number(topLeftRow));
	cell.append(":");
	cell.append(QChar(bottomRightColumn - 1 + 'A'));
	cell.append(QString::number(bottomRightRow));

	QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("VerticalAlignment", -4108);//ylCenter    
	range->setProperty("WrapText", true);
	range->setProperty("MergeCells", true);
}

void ExcelManager::mergeCellsCustom(int topLeftColumn, int bottomRightColumn)
{
	QString cell;
	cell.append(QChar(topLeftColumn - 1 + 'A'));
	cell.append(QString::number(intRows + 1));
	cell.append(":");
	cell.append(QChar(bottomRightColumn - 1 + 'A'));
	cell.append(QString::number(intRows + 2));

	QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("VerticalAlignment", -4108);//ylCenter    
	range->setProperty("WrapText", true);
	range->setProperty("MergeCells", true);
}

void ExcelManager::mergeRowsCells(int columnIndex)
{
	QString cell;
	if (columnIndex > 26) {
		cell.append('A');
		columnIndex -= 26;
		cell.append(QChar(columnIndex - 1 + 'A'));
		cell.append(QString::number(intRows + 1));
		cell.append(":A");
	}
	else {
		cell.append(QChar(columnIndex - 1 + 'A'));
		cell.append(QString::number(intRows + 1));
		cell.append(":");
	}
	cell.append(QChar(columnIndex - 1 + 'A'));
	cell.append(QString::number(intRows + 2));

	QAxObject *range = m_sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("VerticalAlignment", -4108);//ylCenter    
	range->setProperty("WrapText", true);
	range->setProperty("MergeCells", true);
}

QString ExcelManager::IndexToRange(int rowIndex, int columnIndex)
{
	QString cellRange;
	if (columnIndex > 26) {
		cellRange.append('A');
		columnIndex -= 26;
	}
	cellRange.append(QChar(columnIndex - 1 + 'A'));
	cellRange.append(QString::number(rowIndex));
	return cellRange;
}

void ExcelManager::setCellTextCenter(QAxObject* sheet, int rowIndex, int columnIndex)
{
	QString cell = IndexToRange(rowIndex, columnIndex);
	QAxObject *range = sheet->querySubObject("Range(const QString&)", cell);
	range->setProperty("HorizontalAlignment", -4108);//xlCenter    
}

void ExcelManager::SetNewCellValueFirst(QAxObject* sheet)
{
	SetNewCellValue(sheet, 1, 1, "Identifier");
	SetNewCellValue(sheet, 2, 1, "Current Time");
	SetNewCellValue(sheet, 3, 1, "First Image");
	SetNewCellValue(sheet, 4, 1, "Second Image(s)");
	SetNewCellValue(sheet, 5, 1, "1 to N images");
	SetNewCellValue(sheet, 6, 1, "First Image Exists in Bdd");
	SetNewCellValue(sheet, 7, 1, "Requested Image");
}
void ExcelManager::SetNewCellValueLast(QAxObject* sheet, int startColumnIndex)
{
	SetNewCellValue(sheet, startColumnIndex, 1, "Threshold Score");
	SetNewCellValue(sheet, startColumnIndex + 1, 1, "key Points 1");
	SetNewCellValue(sheet, startColumnIndex + 2, 1, "key Points 2");
	SetNewCellValue(sheet, startColumnIndex + 3, 1, "Detection Time");
	SetNewCellValue(sheet, startColumnIndex + 4, 1, "Description Time");
	SetNewCellValue(sheet, startColumnIndex + 5, 1, "Clustering Time");
	SetNewCellValue(sheet, startColumnIndex + 6, 1, "Matching Time");
	SetNewCellValue(sheet, startColumnIndex + 7, 1, "Total Time");
	SetNewCellValue(sheet, startColumnIndex + 8, 1, "Accepted Matches");
	SetNewCellValue(sheet, startColumnIndex + 9, 1, "Rejected Matches");
	SetNewCellValue(sheet, startColumnIndex + 10, 1, "Best Image Average");
	SetNewCellValue(sheet, startColumnIndex + 11, 1, "Best Image Score");
	SetNewCellValue(sheet, startColumnIndex + 12, 1, "Requested Image Score");
	SetNewCellValue(sheet, startColumnIndex + 13, 1, "Best Image");
	SetNewCellValue(sheet, startColumnIndex + 14, 1, "Rank");
}

void ExcelManager::setCellFontBold(QAxObject* cell, int size) {
	QAxObject* chars = cell->querySubObject("Characters(int, int)", 1, size);
	QAxObject* font = chars->querySubObject("Font");
	font->setProperty("Bold", true);
}

void ExcelManager::setCellFontColored(QAxObject* cell, int size) {
	QAxObject* chars = cell->querySubObject("Characters(int, int)", 1, size);
	QAxObject* font = chars->querySubObject("Font");
	font->setProperty("Color", QColor(255, 255, 255));
}

void ExcelManager::setCellBackgroundColored(QAxObject* cell, int r, int g, int b) {
	QAxObject* Interior = cell->querySubObject("Interior");
	Interior->setProperty("Color", QColor(r, g, b));
	QAxObject* borders = cell->querySubObject("Borders");
	borders->setProperty("Color", QColor(255, 255, 255));
}

void ExcelManager::setCellBordersColored(QAxObject* cell, int r, int g, int b) {
	QAxObject* borders = cell->querySubObject("Borders");
	borders->setProperty("Color", QColor(r, g, b));
}

void ExcelManager::SetNewCellValue(QAxObject* sheet, int columnIndex, int intHorizontallyRange, const QString& value)
{
	if (intHorizontallyRange == 1) {
		QAxObject *cell = sheet->querySubObject("Cells(int,int)", 1, columnIndex);
		cell->setProperty("Value", value);
		cell->setProperty("VerticalAlignment", -4108);
		cell->setProperty("HorizontalAlignment", -4108);
		cell->setProperty("ColumnWidth", value.size() + 4);
		cell->setProperty("RowHeight", 25);
		setCellFontBold(cell, value.size());
		setCellFontColored(cell, value.size());
		setCellBackgroundColored(cell, 0, 0, 80);
		setCellBordersColored(cell, 255, 255, 255);
		delete cell;
	}
	else {
		QString cellRange = IndexesToRange(1, columnIndex, intHorizontallyRange);
		QAxObject *range = sheet->querySubObject("Range(const QString&)", cellRange);
		range->setProperty("VerticalAlignment", -4108);
		range->setProperty("HorizontalAlignment", -4108); //xlCenter 
		range->setProperty("WrapText", true);
		range->setProperty("MergeCells", true);
		range->setProperty("Value", value);
		setCellFontBold(range, value.size());
		setCellFontColored(range, value.size());
		setCellBackgroundColored(range, 0, 0, 80);
		setCellBordersColored(range, 255, 255, 255);
		delete range;
	}
}

QAxObject* ExcelManager::AddNewSheet(const QString& value)
{
	m_sheets->querySubObject("Add");
	QAxObject * sheet = m_sheets->querySubObject("Item(int)", 1);
	sheet->setProperty("Name", value);
	return sheet;
}

ExcelManager::~ExcelManager()
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
}