#pragma once

#if !defined(_EDGE_LOCATOR_FLOATING_WINDOW_SMOOTHED_H_)
#define _EDGE_LOCATOR_FLOATING_WINDOW_SMOOTHED_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <cmath>
#include "Edge.h"
#include "AbstractEdgeLocator.h"
#include "GaussianFilter.h"

namespace EdgeLocator {

	class EdgeLocatorFloatingWindowsSmoothed : public AbstractEdgeLocator {

	public:
		EdgeLocatorFloatingWindowsSmoothed(cv::Mat& image, float threshold = 20, int order = 2) {
			IMAGE = process_image(image);
			cv::Mat imageFiltered = GaussianFilter::applyGaussianFilter(IMAGE);
			EDGES_LIST = detectEdges(imageFiltered, threshold, order);
		}

	private:

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

			for (int i = 5; i < rows - 5; i++) {
				for (int j = 2; j < cols - 2; j++) {
					if (modGradData[i * cols + j] > threshold
						&& absFyData[i * cols + j] >= absFxData[i * cols + j]
						&& absFyData[i * cols + j] > absFyData[(i - 1) * cols + j]
						&& absFyData[i * cols + j] >= absFyData[(i + 1) * cols + j]) {
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

			// compute all horizontal edges
			int l1, l2, r1, r2, minl1, maxl2, minr1, maxr2;
			int k = 0;
			for (int edge : edges) {

				//compute window floating limits
				int m1 = -1;
				int m2 = 1;

				if (FxData[edge] * FyData[edge] >= 0) {
					l1 = 0;
					r2 = 0;
					minl1 = -2;
					maxr2 = 2;
					l2 = 1;
					r1 = -1;
					maxl2 = 4;
					minr1 = -4;
				}
				else {
					l1 = -1;
					r2 = 1;
					minl1 = -4;
					maxr2 = 4;
					l2 = 0;
					r1 = 0;
					maxl2 = 2;
					minr1 = -2;
				}

				if (absFxData[edge] < 1) {
					l1 = -1;
					l2 = 1;
					r1 = -1;
					r2 = 1;
				}

				while (l1 > minl1 && absFyData[edge - 1 + l1 * cols] >= absFyData[edge - 1 + (l1 - 1) * cols]) {
					l1--;
				}
				while (l2 < maxl2 && absFyData[edge - 1 + l2 * cols] >= absFyData[edge - 1 + (l2 + 1) * cols]) {
					l2++;
				}
				while (m1 > -3 && absFyData[edge + m1 * cols] >= absFyData[edge + (m1 - 1) * cols]) {
					m1--;
				}
				while (m2 < 3 && absFyData[edge + m2 * cols] >= absFyData[edge + (m2 + 1) * cols]) {
					m2++;
				}
				while (r1 > minr1&& absFyData[edge + 1 + r1 * cols] >= absFyData[edge + 1 + (r1 - 1) * cols]) {
					r1--;
				}
				while (r2 < maxr2 && absFyData[edge + 1 + r2 * cols] >= absFyData[edge + 1 + (r2 + 1) * cols]) {
					r2++;
				}

				// compute intensities
				float AA, BB;

				AA = imageData[edge + m2 * cols];
				BB = imageData[edge + m1 * cols];
				
				// sum columns
				float SL = 0, SM = 0, SR = 0;
				for (int n = l1; n <= l2; n++) {
					SL = SL + imageData[edge - 1 + n * cols];
				}

				for (int n = m1; n <= m2; n++) {
					SM = SM + imageData[edge + n * cols];
				}

				for (int n = r1; n <= r2; n++) {
					SR = SR + imageData[edge + 1 + n * cols];
				}

				// compute edge features
				float den = 2 * (AA - BB);
				if (order == 2) {
					c[k] = (SL + SR - 2 * SM + AA * (2 * m2 - l2 - r2) - BB * (2 * m1 - l1 - r1)) / den;
				}
				else {
					c[k] = 0;
				}
				b[k] = (SR - SL + AA * (l2 - r2) - BB * (l1 - r1)) / den;
				a[k] = (2 * SM - AA * (1 + 2 * m2) - BB * (1 - 2 * m1)) / den - c[k] * (1 + 24 * GaussianFilter::A01 + GaussianFilter::A11) / 12;
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
				float sub_x = x[pos] + 0.5f;
				float sub_y = y[pos] - a[i] + 0.5f;
				float nx = (std::signbit(A[i] - B[i]) ? -1 : 1) / (float)std::sqrt(1 + b[i] * b[i]) * b[i];
				float ny = (std::signbit(A[i] - B[i]) ? -1 : 1) / (float)std::sqrt(1 + b[i] * b[i]);
				float curv = 2 * c[i] * n[i] / (float)std::pow((1 + b[i] * b[i]), 1.5);
				float i0 = std::min(A[i], B[i]);
				float i1 = std::max(A[i], B[i]);

				//std::cout << "Curvatura Horizontal: " << curv << std::endl;

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

			for (int i = 2; i < rows - 2; i++) {
				for (int j = 5; j < cols - 5; j++) {
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

			// compute all vertical edges
			int l1, l2, r1, r2, minl1, maxl2, minr1, maxr2;
			int k = 0;
			for (int edge : edges) {

				//compute window floating limits
				int m1 = -1;
				int m2 = 1;

				if (FxData[edge] * FyData[edge] >= 0) {
					l1 = 0;
					r2 = 0;
					minl1 = -2;
					maxr2 = 2;
					l2 = 1;
					r1 = -1;
					maxl2 = 4;
					minr1 = -4;
				}
				else {
					l1 = -1;
					r2 = 1;
					minl1 = -4;
					maxr2 = 4;
					l2 = 0;
					r1 = 0;
					maxl2 = 2;
					minr1 = -2;
				}

				if (absFyData[edge] < 1) {
					l1 = -1;
					l2 = 1;
					r1 = -1;
					r2 = 1;
				}

				while (l1 > minl1&& absFxData[edge - cols + l1] >= absFxData[edge - cols + l1 - 1]) {
					l1--;
				}
				while (l2 < maxl2 && absFxData[edge - cols + l2] >= absFxData[edge - cols + l2 + 1]) {
					l2++;
				}
				while (m1 > -3 && absFxData[edge + m1] >= absFxData[edge + m1 - 1]) {
					m1--;
				}
				while (m2 < 3 && absFxData[edge + m2] >= absFxData[edge + m2 + 1]) {
					m2++;
				}
				while (r1 > minr1&& absFxData[edge + cols + r1] >= absFxData[edge + cols + r1 - 1]) {
					r1--;
				}
				while (r2 < maxr2 && absFxData[edge + cols + r2] >= absFxData[edge + cols + r2 + 1]) {
					r2 = r2 + 1;
				}

				// compute intensities
				float AA, BB;

				AA = imageData[edge + m2];
				BB = imageData[edge + m1];

				// sum columns
				float SL = 0, SM = 0, SR = 0;
				for (int n = l1; n <= l2; n++) {
					SL = SL + imageData[edge - cols + n];
				}

				for (int n = m1; n <= m2; n++) {
					SM = SM + imageData[edge + n];
				}

				for (int n = r1; n <= r2; n++) {
					SR = SR + imageData[edge + cols + n];
				}

				// compute edge features
				float den = 2 * (AA - BB);
				if (order == 2) {
					c[k] = (SL + SR - 2 * SM + AA * (2 * m2 - l2 - r2) - BB * (2 * m1 - l1 - r1)) / den;
				}
				else {
					c[k] = 0;
				}
				b[k] = (SR - SL + AA * (l2 - r2) - BB * (l1 - r1)) / den;
				a[k] = (2 * SM - AA * (1 + 2 * m2) - BB * (1 - 2 * m1)) / den - c[k] * (1 + 24 * GaussianFilter::A01 + GaussianFilter::A11) / 12;
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
				float sub_x = x[pos] - a[i] + 0.5f;
				float sub_y = y[pos] + 0.5f;
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

#endif // _EDGE_LOCATOR_2_H_