#pragma once

#if !defined(_ABSTRACT_EDGE_LOCATOR_H_)
#define _ABSTRACT_EDGE_LOCATOR_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include "Edge.h"
#include "json.hpp"
#include <math.h>
#include <map>

using json = nlohmann::json;

namespace EdgeLocator {

	class AbstractEdgeLocator {

	protected:
		cv::Mat IMAGE;
		std::vector<Edge> EDGES_LIST;

	public:

		cv::Mat getImageWithEdges(int zoom, int x, int y, int width, int height, bool view_norm = false) {
			cv::Mat image_with_edges;
			if (view_norm) {
				image_with_edges = visEdgesWithNormal(zoom, x, y, width, height);
			}
			else {
				image_with_edges = visEdges(zoom, x, y, width, height);
			}
			image_with_edges.convertTo(image_with_edges, CV_8UC3);
			return image_with_edges;
		}

		std::vector<Edge> getEdgesList() {
			return EDGES_LIST;
		}

		void saveEdges(std::string fileName) {
			json edges_list;

			for (EdgeLocator::Edge e : EDGES_LIST) {
				std::map<std::string, float> param_edge;
				std::map<std::string, float>::iterator it;
				
				param_edge.insert(std::pair<std::string, float>("position", (float)e.getPosition()));
				param_edge.insert(std::pair<std::string, float>("x", (float)e.getX()));
				param_edge.insert(std::pair<std::string, float>("y", (float)e.getY()));
				param_edge.insert(std::pair<std::string, float>("nx", (float)e.getNx()));
				param_edge.insert(std::pair<std::string, float>("ny", (float)e.getNy()));
				param_edge.insert(std::pair<std::string, float>("curv", (float)e.getCurv()));
				param_edge.insert(std::pair<std::string, float>("i0", (float)e.getI0()));
				param_edge.insert(std::pair<std::string, float>("i1", (float)e.getI1()));
					
				json edge_object(param_edge);
				edges_list.push_back(edge_object);
			}
			
			std::ofstream oStr(fileName);
			oStr << edges_list << std::endl;
		}

		void generateAccuracyResults(cv::Size tam, float radius_in, float radius_out, cv::Point2f center, std::string fileName) {

			float MSE_curv_in = 0.0f;
			float MSE_curv_out = 0.0f;

			float MSE_edge_pos_in = 0.f;
			float MSE_edge_pos_out = 0.f;
			
			float mean_normal_error = 0.f;

			float mean_real_distances_in = 0.f;
			float mean_real_distances_out = 0.f;

			float mean_real_curv_in = 0.0f;
			float mean_real_curv_out = 0.0f;

			int count_in = 0;

			for (Edge e : EDGES_LIST) {

				cv::Vec2f radial_edge_vector(e.getX() - center.x, e.getY() - center.y);
				
				float real_distance = cv::norm(radial_edge_vector);

				float edge_pos_in = (real_distance - radius_in) * (real_distance - radius_in);
				float edge_pos_out = (real_distance - radius_out) * (real_distance - radius_out);

				float r_curv = 1.f / e.getCurv();
				float curv = e.getCurv();

				//std::cout << r_curv << std::endl;

				if (edge_pos_in < edge_pos_out) {
					count_in++;

					MSE_edge_pos_in += edge_pos_in;
					mean_real_distances_in += real_distance;
		
					MSE_curv_in += (curv - 1.f/radius_in) * (curv - 1.f/radius_in);
					mean_real_curv_in += curv;
				}
				else {
					MSE_edge_pos_out += edge_pos_out;
					mean_real_distances_out += real_distance;

					MSE_curv_out += (curv - 1.f / radius_out) * (curv - 1.f / radius_out);
					mean_real_curv_out += curv;
				}
				
				cv::Vec2f radial_normalize_edge_vector = radial_edge_vector / cv::norm(radial_edge_vector);

				cv::Vec2f edge_normal_vector(e.getNx(), e.getNy());
				mean_normal_error += 1.f - std::abs(radial_normalize_edge_vector.dot(edge_normal_vector));
			}

			MSE_edge_pos_in /= (float)count_in;
			MSE_edge_pos_out /= (float)(EDGES_LIST.size() - count_in);

			MSE_curv_in /= (float)count_in;
			MSE_curv_out /= (float)(EDGES_LIST.size() - count_in);

			mean_normal_error /= EDGES_LIST.size();

			mean_real_distances_in /= (float)count_in;
			mean_real_distances_out /= (float)(EDGES_LIST.size() - count_in);

			mean_real_curv_in /= (float)count_in;
			mean_real_curv_out /= (float)(EDGES_LIST.size() - count_in);

			float sd_distances_in = 0.f;
			float sd_distances_out = 0.f;

			float sd_curvs_in = 0.f;
			float sd_curvs_out = 0.f;

			for (Edge e : EDGES_LIST) {
				cv::Vec2f radial_edge_vector(e.getX() - center.x, e.getY() - center.y);

				float real_distance = cv::norm(radial_edge_vector);

				float edge_pos_in = (real_distance - radius_in) * (real_distance - radius_in);
				float edge_pos_out = (real_distance - radius_out) * (real_distance - radius_out);

				float r_curv = 1.f / e.getCurv();
				float curv = e.getCurv();

				if (edge_pos_in < edge_pos_out) {
					sd_distances_in += (real_distance - mean_real_distances_in) * (real_distance - mean_real_distances_in);
					sd_curvs_in += (curv - mean_real_curv_in) * (curv - mean_real_curv_in);
				}
				else {
					sd_distances_out += (real_distance - mean_real_distances_out) * (real_distance - mean_real_distances_out);
					sd_curvs_out += (curv - mean_real_curv_out) * (curv - mean_real_curv_out);
				}

			}

			sd_distances_in = std::sqrtf(sd_distances_in / ((float)count_in - 1.f));
			sd_distances_out = std::sqrtf(sd_distances_out / (float)(EDGES_LIST.size() - (float)count_in - 1.f));

			sd_curvs_in = std::sqrtf(sd_curvs_in / ((float)count_in - 1.f));
			sd_curvs_out = std::sqrtf(sd_curvs_out / (float)(EDGES_LIST.size() - (float)count_in - 1.f));

			std::ofstream results;
			results.open(fileName);
			
			results << "External Edge Position Radius (Mean Squared Error): " << MSE_edge_pos_out << "\n";
			results << "External Real Radius: " << radius_out << "\nMean Radius: " << mean_real_distances_out << "\nRadius Standard Deviation: " << sd_distances_out << "\n\n";

			results << "Internal Edge Position Radius (Mean Squared Error): " << MSE_edge_pos_in << "\n";
			results << "Internal Real Radius: " << radius_in << "\nMean Radius: " << mean_real_distances_in << "\nRadius Standard Deviation: " << sd_distances_in << "\n\n";
			
			results << "External Curvature (Mean Squared Error): " << MSE_curv_out << "\n";
			results << "Real External Radius: " << radius_out << "\nMean Radius of Curvature: " << std::abs(1.f/mean_real_curv_out) << "\nCurvature Standard Deviation: " << sd_curvs_out << "\n\n";

			results << "Internal Curvature (Mean Squared Error): " << MSE_curv_in << "\n";
			results << "Real Internal Radius: " << radius_in << "\nMean Radius of Curvature: " << std::abs(1.f/mean_real_curv_in) << "\nCurvature Standard Deviation: " << sd_curvs_in << "\n\n";
			
			results << "Normal Vector (Mean Projection Error): " << mean_normal_error << "\n";
			
			results.close();
			
		}

	protected:

		cv::Mat process_image(cv::Mat& image) {

			if (image.channels() > 1) {
				cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
			}

			image.convertTo(image, CV_32FC1);
			return image;
		}

		std::vector<Edge> detectEdges(cv::Mat& image, float threshold = 20, int order = 2) {
			std::vector<Edge> edges = detectHorizontalEdges(image, threshold, order);
			std::vector<Edge> v_edges = detectVerticalEdges(image, threshold, order);
			edges.insert(std::end(edges), std::begin(v_edges), std::end(v_edges));

			return edges;
		}

		virtual std::vector<Edge> detectHorizontalEdges(cv::Mat& image, float threshold, int order) = 0;
		virtual std::vector<Edge> detectVerticalEdges(cv::Mat& image, float threshold, int order) = 0;

		cv::Mat visEdges(int zoom, int x, int y, int width, int height) {

			cv::Mat imageColor(IMAGE.size(), CV_32FC3);

			int pos_X = round((float)x / (float)zoom);
			int pos_Y = round((float)y / (float)zoom);

			int aux_width = width / zoom;
			int aux_height = height / zoom;

			cv::Rect roi(pos_X, pos_Y, aux_width, aux_height);
			cv::Mat sub_img = IMAGE(roi);

			cv::cvtColor(sub_img, imageColor, cv::COLOR_GRAY2RGB);
			cv::resize(imageColor, imageColor, cv::Size(imageColor.cols * zoom, imageColor.rows * zoom), 0, 0, 0);

			float seg = 0.6;

			for (Edge ep : EDGES_LIST) {

				cv::Point e_pt1(round((ep.getX() - seg / 2 * ep.getNy()) * zoom * zoom), round((ep.getY() + seg / 2 * ep.getNx()) * zoom * zoom));
				cv::Point e_pt2(round(seg * ep.getNy() * zoom * zoom) + e_pt1.x, round(-seg * ep.getNx() * zoom * zoom) + e_pt1.y);

				e_pt1.x -= x * zoom;
				e_pt1.y -= y * zoom;

				e_pt2.x -= x * zoom;
				e_pt2.y -= y * zoom;

				if (e_pt1.x < 0 || e_pt1.x > width * zoom || e_pt2.x < 0 || e_pt2.x > width * zoom) {
					continue;
				}

				if (e_pt1.y < 0 || e_pt1.y > height * zoom || e_pt2.y < 0 || e_pt2.y > height * zoom) {
					continue;
				}

				cv::line(imageColor, e_pt1, e_pt2, cv::Scalar(0, 255, 255), 1, cv::LINE_8, log2(zoom));
			}

			return imageColor;
		}

		cv::Mat visEdgesWithNormal(int zoom, int x, int y, int width, int height) {

			cv::Mat imageColor(IMAGE.size(), CV_32FC3);

			int pos_X = round((float)x / (float)zoom);
			int pos_Y = round((float)y / (float)zoom);

			int aux_width = width / zoom;
			int aux_height = height / zoom;
		
			cv::Rect roi(pos_X, pos_Y, aux_width, aux_height);
			cv::Mat sub_img = IMAGE(roi);

			cv::cvtColor(sub_img, imageColor, cv::COLOR_GRAY2RGB);
			cv::resize(imageColor, imageColor, cv::Size(imageColor.cols * zoom, imageColor.rows * zoom), 0, 0, 0);

			float seg = 0.6;

			for (Edge ep : EDGES_LIST) {

				cv::Point e_pt1(round((ep.getX() - seg / 2 * ep.getNy()) * zoom * zoom), round((ep.getY() + seg / 2 * ep.getNx()) * zoom * zoom));
				cv::Point e_pt2(round(seg * ep.getNy() * zoom * zoom) + e_pt1.x, round(-seg * ep.getNx() * zoom * zoom) + e_pt1.y);

				e_pt1.x -= x * zoom;
				e_pt1.y -= y * zoom;

				e_pt2.x -= x * zoom;
				e_pt2.y -= y * zoom;

				if (e_pt1.x < 0 || e_pt1.x > width * zoom || e_pt2.x < 0 || e_pt2.x > width * zoom) {
					continue;
				}

				if (e_pt1.y < 0 || e_pt1.y > height * zoom || e_pt2.y < 0 || e_pt2.y > height * zoom) {
					continue;
				}

				cv::Point n_pt1(round(ep.getX() * zoom * zoom), round(ep.getY() * zoom * zoom));
				cv::Point n_pt2(round((ep.getX() + ep.getNx()) * zoom * zoom), round((ep.getY() + ep.getNy()) * zoom * zoom));

				n_pt1.x -= x * zoom;
				n_pt1.y -= y * zoom;

				n_pt2.x -= x * zoom;
				n_pt2.y -= y * zoom;

				if (n_pt1.x < 0 || n_pt1.x > width * zoom || n_pt2.x < 0 || n_pt2.x > width * zoom) {
					continue;
				}

				if (n_pt1.y < 0 || n_pt1.y > height * zoom || n_pt2.y < 0 || n_pt2.y > height * zoom) {
					continue;
				}

				cv::line(imageColor, e_pt1, e_pt2, cv::Scalar(255, 0, 0), 1, cv::LINE_8, log2(zoom));
				cv::line(imageColor, n_pt1, n_pt2, cv::Scalar(0, 0, 255), 1, cv::LINE_8, log2(zoom));
			}

			return imageColor;
		}

	};
}

#endif // _ABSTRACT_EDGE_LOCATOR_H_

