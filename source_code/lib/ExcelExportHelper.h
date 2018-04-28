#ifndef EXCELHELPER_H
#define EXCELHELPER_H

#include <ActiveQt/qaxobject.h>

//Expected in .pro file: QT += axcontainer
//Application must be of UI type for ActiveX work.
class ExcelExportHelper
{
public:
    ExcelExportHelper(const ExcelExportHelper& other) = delete;
    ExcelExportHelper& operator=(const ExcelExportHelper& other) = delete;
	ExcelExportHelper(bool closeExcelOnExit, const QString& fileName, int numSheet);

	void SetCellValue(int columnIndex, int type, const QString& value);
	void SetCellValueSecondRow(int columnIndex, const QString& value);
	void SetNewCellValue(QAxObject* sheet, int columnIndex, int intHorizontallyRange, const QString& value);
	QAxObject *AddNewSheet(const QString& value);
	QString IndexesToRange(int rowIndex, int index, int length);
	QString IndexToRange(int rowIndex, int index);
	QString GetSheetName();
	void setCellTextCenter(QAxObject* sheet, int rowIndex, int index);
	void setCellBackgroundColored(QAxObject* cell, int r, int g, int b);
	void setCellBordersColored(QAxObject* cell, int r, int g, int b);
	void setCellFontBold(QAxObject* cell, int size);
	void setCellFontColored(QAxObject* cell, int size);
	void mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn);
	void mergeCellsCustom(int topLeftColumn, int bottomRightColumn);
	void mergeRowsCells(int columnIndex);
	void SetNewCellValueFirst(QAxObject* sheet);
	void SetNewCellValueLast(QAxObject* sheet, int startColumnIndex);
	QVariant GetCellValue(int rowIndex, int columnIndex);
	void GetIntRows(int numSheet);
	int getSheetCount();
	int getColumnsCount();
	void killProcessByName(const char *filename);

	~ExcelExportHelper();

private:
    QAxObject* m_excelApplication;
    QAxObject* m_workbooks;
    QAxObject* m_workbook;
    QAxObject* m_sheets;
    QAxObject* m_sheet;
	QAxObject* m_sheet_custom;
	QAxObject* m_sheet_brisk;
	QAxObject* m_sheet_orb;
	QAxObject* m_sheet_surf;
	QAxObject* m_sheet_sift;
	QAxObject* m_usedrange;
	QAxObject* m_rows;
	QAxObject* m_columns;
	int intRows;
	int intColumns;
    bool m_closeExcelOnExit;
};

#endif // EXCELHELPER_H
