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

    ExcelExportHelper(bool closeExcelOnExit = false);
    void SetCellValue(int lineIndex, int columnIndex, const QString& value);
    void Open(const QString& fileName);

    ~ExcelExportHelper();

private:
    QAxObject* m_excelApplication;
    QAxObject* m_workbooks;
    QAxObject* m_workbook;
    QAxObject* m_sheets;
    QAxObject* m_sheet;
    bool m_closeExcelOnExit;
};

#endif // EXCELHELPER_H
