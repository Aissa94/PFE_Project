#ifndef _INCLUDE_FILES_H
#define _INCLUDE_FILES_H

// Important :
// add lib and classes directory to the Include directory in Project properties...  

// Include C++
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <iomanip>
#include <string>
#include <ctime>
#include <functional>

// Include Qt
#include <QMainWindow>
#include <QtWidgets>

// Include Opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Include lib
// http://www.codelooker.com/id/217/1100103.html
#include "binarisation\AdaptiveLocalThreshold.h"
#include "extraction\CrossingNumber.h"
#include "extraction\Filter.h"
#include "extraction\Minutiae.h"
#include "gap-optimisation\GapOptimisation.h"
#include "optimisation\Ideka.h"
#include "thinning\GuoHall.h"
#include "thinning\ZhangSuen.h"
#include "file-reader\tinydir.h"
#include "dualMod\DualModeI.h"
#include "ExcelExportHelper.h"
#include "graph\qcustomplot.h"

// Dr :
#include "image-processing\image_processing.h"

// Our
#include "image-processing\MinutiaeToKeyPointAdapter.h"

// Include classes



#endif
