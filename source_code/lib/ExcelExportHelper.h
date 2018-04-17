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

    ExcelExportHelper(bool closeExcelOnExit, int numSheet);
    void SetCellValue(int columnIndex, const QString& value);
	void SetNewCellValue(QAxObject* sheet, int columnIndex, int intHorizontallyRange, const QString& value);
	QAxObject *AddNewSheet(const QString& value);
	QString IndexesToRange(int rowIndex, int index, int length);
	QString IndexToRange(int rowIndex, int index);
	void setCellTextCenter(QAxObject* sheet, int rowIndex, int index);
	void setCellFontBold(QAxObject* cell, int size);
    void Create();

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
	int intRows;
    bool m_closeExcelOnExit;
};

#endif // EXCELHELPER_H
