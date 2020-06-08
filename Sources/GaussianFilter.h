#pragma once

#if !defined(_GAUSSIAN_FILTER_H_)
#define _GAUSSIAN_FILTER_H_

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace EdgeLocator {

	class GaussianFilter {

	public:
		static float A11;
		static float A01;
		static float A00;

		static cv::Mat applyGaussianFilter(cv::Mat& image) {

			float gaussianFilter[9] = {
				A11, A01, A11,
				A01, A00, A01,
				A11, A01, A11
			};

			int rows = image.rows;
			int cols = image.cols;

			float* copy = new float[rows * cols];
			float* imageData = (float*)image.data;

			for (int i = 1; i < rows - 1; i++) {
				for (int j = 1; j < cols - 1; j++) {

					float new_val = 0;

					for (int l = 0; l < 3; l++) {
						for (int m = 0; m < 3; m++) {
							new_val += gaussianFilter[l * 3 + m] * imageData[(i - 1 + l) * cols + (j - 1 + m)];
						}
					}

					copy[i * cols + j] = new_val;
				}
			}

			return cv::Mat(rows, cols, CV_32FC1, copy);
		}

	};
}








#endif // _GAUSSIAN_FILTER_H_