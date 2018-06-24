#include "TaskThread.h"

TaskThread::TaskThread(QObject *parent)
	: QThread(parent)
{
}

TaskThread::TaskThread(int nbTasks)
{
	this->nbTasks = nbTasks;
}

TaskThread::~TaskThread()
{
}

void TaskThread::terminateThread(){
	threadIsCanceled = true;
}

void TaskThread::run(){

	int nbTested = 0, progressPercentage = 1;

	while (!threadIsCanceled && nbTested < nbTasks){
		// Sending progress updates to the UI
		// do traitment
		nbTested++;
		emit taskPercentageComplete(progressPercentage * 100 / nbTasks);
		progressPercentage++;
	}
}