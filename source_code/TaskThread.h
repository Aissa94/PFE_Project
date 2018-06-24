#pragma once

#include <QThread>

class TaskThread : public QThread
{
	Q_OBJECT

public:
	TaskThread(QObject *parent);
	TaskThread(int nbTasks);
	~TaskThread();
	void run();

signals:
	void taskPercentageComplete(int);

public slots:
	void terminateThread();

private:
	int nbTasks;
	bool threadIsCanceled = false;
};
