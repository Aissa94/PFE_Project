#ifndef EXCELMANAGER_H
#define EXCELMANAGER_H

#include <ActiveQt/qaxobject.h>

//Expected in .pro file: QT += axcontainer
//Application must be of UI type for ActiveX work.
class ExcelManager
{
public:
    ExcelManager(const ExcelManager& other) = delete;
    ExcelManager& operator=(const ExcelManager& other) = delete;
	ExcelManager(bool closeExcelOnExit, const QString& fileName, int numSheet);

	void SetCellValue(int columnIndex, int type, const QString& value, bool typeTest = true);
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
	void SetNewCellValueFirst(QAxObject* sheet, int type);
	void SetNewCellValueLast(QAxObject* sheet, int startColumnIndex, int type);
	QVariant GetCellValue(int rowIndex, int columnIndex);
	void GetIntRows(int numSheet);
	void DeleteRow();
	int getSheetCount();
	int getColumnsCount();

	~ExcelManager();

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

#endif // EXCELMANAGER_H
