#pragma once

#if !defined(_EDGE_LOCATOR_FACTORY_)
#define _EDGE_LOCATOR_FACTORY_H_

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "AbstractEdgeLocator.h"
#include "BasicEdgeLocator.h"
#include "BasicEdgeLocatorSmoothed.h"
#include "EdgeLocatorFloatingWindows.h"
#include "EdgeLocatorFloatingWindowsSmoothed.h"

namespace EdgeLocator {
	static class EdgeLocatorFactory {
	
	public:

		std::string method;

		AbstractEdgeLocator* getEdgeLocator(int type, cv::Mat &src, float threshold = 20, int order = 2) {
			switch (type) {
				case 0:
					method = "EdgeLocatorFloatingWindowsSmoothed";
					return new EdgeLocatorFloatingWindowsSmoothed(src, threshold, order);

				case 1:
					method = "EdgeLocatorFloatingWindows";
					return new EdgeLocatorFloatingWindows(src, threshold, order);

				case 2:
					method = "BasicEdgeLocatorSmoothed";
					return new BasicEdgeLocatorSmoothed(src, threshold, order);

				case 3:
					method = "BasicEdgeLocator";
					return new BasicEdgeLocator(src, threshold, order);
			}
		}
	};
}

#endif // _EDGE_LOCATOR_FACTORY_H_