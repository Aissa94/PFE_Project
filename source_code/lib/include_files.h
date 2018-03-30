// Includes files from lib to use

// Important :
// add lib directory to the Include directory in Project properties...  
#ifndef _INCLUDE_FILES_H
#define _INCLUDE_FILES_H

// http://www.codelooker.com/id/217/1100103.html
#include "binarisation\AdaptiveLocalThreshold.h"
#include "extraction\CrossingNumber.h"
#include "extraction\Filter.h"
#include "extraction\Minutiae.h"
#include "gap-optimisation\GapOptimisation.h"
#include "optimisation\Ideka.h"
#include "thinning\GuoHall.h"
#include "thinning\ZhangSuen.h"
#include "QtableWidget\QtableWidget_CopyPaste.h"
#include "ExcelExportHelper.h"


// Dr :
#include "image-processing\image_processing.h"

// Our
#include "image-processing\MinutiaeToKeyPointAdapter.h"

#endif
