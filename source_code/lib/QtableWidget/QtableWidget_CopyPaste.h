// QTableWidget with support for copy and paste added
// Here copy and paste can copy/paste the entire grid of cells
#ifndef QTABLEWIDGET_COPYPASTE_H
#define QTABLEWIDGET_COPYPASTE_H

#include <QTableWidget>
#include <QKeyEvent>
#include <QWidget>

class QTableWidget_CopyPaste : public QTableWidget
{
    Q_OBJECT
public:
  QTableWidget_CopyPaste(int rows, int columns, QWidget *parent = 0) :
  QTableWidget(rows, columns, parent)
  {}

  QTableWidget_CopyPaste(QWidget *parent = 0) :
  QTableWidget(parent)
  {}

private:
  void copy();
  void paste();

public slots:
  void keyPressEvent(QKeyEvent * event);
};

#endif // QTABLEWIDGET_COPYPASTE_H