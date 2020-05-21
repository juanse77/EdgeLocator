#pragma once

#if !defined(_BASIC_EDGE_LOCATOR_ITER1_H_)
#define _BASIC_EDGE_LOCATOR_ITER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Edge.h"
#include "AbstractEdgeLocator.h"

namespace EdgeLocator {

	class BasicEdgeLocatorIter1 : public AbstractEdgeLocator {

	public:

		BasicEdgeLocatorIter1(cv::Mat& image) {
			IMAGE = process_image(image);
			cv::Mat imageFiltered = applyGaussianFilter(IMAGE);
			EDGES_LIST = detectEdges(imageFiltered);
		}

	private:

		float a11 = 1.f / 20.f;
		float a01 = 3.f / 40.f;
		float a00 = 1.f / 2.f;

		cv::Mat applyGaussianFilter(cv::Mat& image) {

			float gaussianFilter[9] = {
				a11, a01, a11,
				a01, a00, a01,
				a11, a01, a11
			};

			int rows = image.rows;
			int cols = image.cols;

			float* copy = new float[rows*cols];
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

		std::vector<Edge> detectHorizontalEdges(cv::Mat& image, float threshold, int order) {
			
			int rows = image.rows;
			int cols = image.cols;

			//compute partial derivatives and gradient
			cv::Mat Fx;
			cv::Mat Fy;

			cv::Sobel(image, Fx, CV_32FC1, 1, 0, 1);
			cv::Sobel(image, Fy, CV_32FC1, 0, 1, 1);

			cv::Mat modGrad;
			cv::Mat angGrad;

			cv::cartToPolar(Fx, Fy, modGrad, angGrad);

			cv::Mat absFx;
			cv::absdiff(Fx, cv::Scalar(0), absFx);

			cv::Mat absFy;
			cv::absdiff(Fy, cv::Scalar(0), absFy);

			int size = (int)(modGrad.total() * modGrad.channels());

			float* modGradData = (float*)modGrad.data;

			float* absFxData = (float*)absFx.data;
			float* absFyData = (float*)absFy.data;

			//detecte edge pixels with maximum Gy (not including margins)
			std::vector<int> edges;

			for (int i = 4; i < rows - 4; i++) {
				for (int j = 1; j < cols - 1; j++) {
					if (modGradData[i * cols + j] > threshold
						&& absFyData[i * cols + j] >= absFxData[i * cols + j]
						&& absFyData[i * cols + j] >= absFyData[(i - 1) * cols + j]
						&& absFyData[i * cols + j] > absFyData[(i + 1) * cols + j]) {
						edges.push_back(i * cols + j);
					}
				}
			}

			float* FxData = (float*)Fx.data;
			float* FyData = (float*)Fy.data;

			float* imageData = (float*)image.data;

			float* A = new float[edges.size()];
			float* B = new float[edges.size()];
			float* a = new float[edges.size()];
			float* b = new float[edges.size()];
			float* c = new float[edges.size()];

			int k = 0;

			// compute all horizontal edges
			for (int edge : edges) {

				// compute intensities
				float AA, BB;

				int m;
				if (FxData[edge] * FyData[edge] >= 0) {
					m = 1;
				}
				else {
					m = -1;
				}

				AA = (imageData[edge + 4 * cols]
					+ imageData[edge + 4 * cols - m]
					+ imageData[edge + 3 * cols - m]) / 3;

				BB = (imageData[edge - 3 * cols + m]
					+ imageData[edge - 4 * cols + m]
					+ imageData[edge - 4 * cols]) / 3;

				// sum columns
				float SL = 0, SM = 0, SR = 0;

				for (int n = 0; n < 7; n++) {
					SL += imageData[edge - (3-m) * cols + n * cols - 1];
					SM += imageData[edge - 3 * cols + n * cols];
					SR += imageData[edge - (3+m) * cols + n * cols + 1];
				}

				// compute edge features
				float den = 2 * (AA - BB);

				if (order == 2) {
					c[k] = (SL + SR - 2 * SM) / den;
				}
				else {
					c[k] = 0;
				}

				b[k] = m + (SR - SL) / den;
				a[k] = (2 * SM - 7 * (AA + BB)) / den - (1+ 24*a01 + 48*a11) * c[k] / 12;

				A[k] = AA;
				B[k] = BB;

				k++;
			}

			// save edges info
			int* n = new int[edges.size()];

			for (int i = 0; i < edges.size(); i++) {
				n[i] = 1;
			}

			for (int i = 0; i < edges.size(); i++) {
				if (FyData[edges.at(i)] < 0) {
					n[i] = -1;
				}
			}

			std::vector<Edge> subpixelList;

			float* x = new float[rows * cols];
			float* y = new float[rows * cols];

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					x[cols * i + j] = (float)j;
					y[cols * i + j] = (float)i;
				}
			}

			for (int i = 0; i < edges.size(); i++) {
				int pos = edges.at(i);
				float sub_x = x[pos] + 0.5;
				float sub_y = y[pos] - a[i] + 0.5;
				float nx = (std::signbit(A[i] - B[i]) ? -1 : 1) / (float)std::sqrt(1 + b[i] * b[i]) * b[i];
				float ny = (std::signbit(A[i] - B[i]) ? -1 : 1) / (float)std::sqrt(1 + b[i] * b[i]);
				float curv = 2 * c[i] * n[i] / (float)std::pow((1 + b[i] * b[i]), 1.5);
				float i0 = std::min(A[i], B[i]);
				float i1 = std::max(A[i], B[i]);

				Edge auxEdge(pos, sub_x, sub_y, nx, ny, curv, i0, i1);
				subpixelList.push_back(auxEdge);

			}

			delete[] A;
			delete[] B;
			delete[] a;
			delete[] b;
			delete[] c;

			return subpixelList;

		}

		std::vector<Edge> detectVerticalEdges(cv::Mat& image, float threshold, int order) {
			int rows = image.rows;
			int cols = image.cols;

			//compute partial derivatives and gradient
			cv::Mat Fx;
			cv::Mat Fy;

			cv::Sobel(image, Fx, CV_32FC1, 1, 0, 1);
			cv::Sobel(image, Fy, CV_32FC1, 0, 1, 1);

			cv::Mat modGrad;
			cv::Mat angGrad;

			cv::cartToPolar(Fx, Fy, modGrad, angGrad);

			cv::Mat absFx;
			cv::absdiff(Fx, cv::Scalar(0), absFx);

			cv::Mat absFy;
			cv::absdiff(Fy, cv::Scalar(0), absFy);

			int size = (int)(modGrad.total() * modGrad.channels());

			float* modGradData = (float*)modGrad.data;

			float* absFxData = (float*)absFx.data;
			float* absFyData = (float*)absFy.data;

			//detecte edge pixels with maximum Gy (not including margins)
			std::vector<int> edges;

			for (int i = 1; i < rows - 1; i++) {
				for (int j = 4; j < cols - 4; j++) {
					if (modGradData[i * cols + j] > threshold
						&& absFxData[i * cols + j] > absFyData[i * cols + j]
						&& absFxData[i * cols + j] >= absFxData[i * cols + j - 1]
						&& absFxData[i * cols + j] > absFxData[i * cols + j + 1]) {
						edges.push_back(i * cols + j);
					}
				}
			}

			float* FxData = (float*)Fx.data;
			float* FyData = (float*)Fy.data;

			float* imageData = (float*)image.data;

			float* A = new float[edges.size()];
			float* B = new float[edges.size()];
			float* a = new float[edges.size()];
			float* b = new float[edges.size()];
			float* c = new float[edges.size()];

			int k = 0;

			// compute all vertical edges
			for (int edge : edges) {

				// compute intensities
				float AA, BB;

				int m;
				if (FxData[edge] * FyData[edge] >= 0) {
					m = 1;
				}
				else {
					m = -1;
				}

				AA = (imageData[edge - m * cols + 3]
					+ imageData[edge - m * cols + 4]
					+ imageData[edge + 4]) / 3;

				BB = (imageData[edge - 4]
					+ imageData[edge + m * cols - 4]
					+ imageData[edge + m * cols - 3]) / 3;

				// sum columns
				float SL = 0, SM = 0, SR = 0;

				for (int n = 0; n < 7; n++) {
					SL += imageData[edge - cols - 3 - m + n];
					SM += imageData[edge - 3 + n];
					SR += imageData[edge + cols - 3 + m + n];
				}

				// compute edge features
				float den = 2 * (AA - BB);

				if (order == 2) {
					c[k] = (SL + SR - 2 * SM) / den;
				}
				else {
					c[k] = 0;
				}

				b[k] = -m + (SR - SL) / den;
				a[k] = (2 * SM - 7 * (AA + BB)) / den - (1 + 24 * a01 + 48 * a11) * c[k] / 12;

				A[k] = AA;
				B[k] = BB;

				k++;
			}

			// save edges info
			int* n = new int[edges.size()];

			for (int i = 0; i < edges.size(); i++) {
				n[i] = 1;
			}

			for (int i = 0; i < edges.size(); i++) {
				if (FxData[edges.at(i)] < 0) {
					n[i] = -1;
				}
			}

			std::vector<Edge> subpixelList;

			float* x = new float[rows * cols];
			float* y = new float[rows * cols];

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					x[cols * i + j] = (float)j;
					y[cols * i + j] = (float)i;
				}
			}

			for (int i = 0; i < edges.size(); i++) {
				int pos = edges.at(i);
				float sub_x = x[pos] - a[i] + 0.5;
				float sub_y = y[pos] + 0.5;
				float nx = (std::signbit(A[i] - B[i]) ? -1 : 1) / (float)std::sqrt(1 + b[i] * b[i]);
				float ny = (std::signbit(A[i] - B[i]) ? -1 : 1) / (float)std::sqrt(1 + b[i] * b[i]) * b[i];
				float curv = 2 * c[i] * n[i] / (float)std::pow((1 + b[i] * b[i]), 1.5);
				float i0 = std::min(A[i], B[i]);
				float i1 = std::max(A[i], B[i]);

				Edge auxEdge(pos, sub_x, sub_y, nx, ny, curv, i0, i1);
				subpixelList.push_back(auxEdge);

			}

			delete[] A;
			delete[] B;
			delete[] a;
			delete[] b;
			delete[] c;

			return subpixelList;

		}

	};

}

#endif // _BASIC_EDGE_LOCATOR_ITER1_H_
