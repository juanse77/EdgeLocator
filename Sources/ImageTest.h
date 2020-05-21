#pragma once

#if !defined(_IMAGE_TEST_H_)
#define _IMAGE_TEST_H_

namespace EdgeLocator {

	class ImageTest {

	public:
		ImageTest(std::string ruta, cv::Size tam, float radius_in, float radius_out, cv::Point2f center, cv::Vec3f color_in, cv::Vec3f color_out, int n) {
			cv::Mat image = generateImageTest(tam, radius_in, radius_out, center, color_in, color_out, n);
			cv::imwrite(ruta, image);
		}

	private:
		cv::Mat generateImageTest(cv::Size tam, float radius_in, float radius_out, cv::Point2f center, cv::Vec3f color_in, cv::Vec3f bg_color, int n) {
			cv::Mat image(tam, CV_32FC3, bg_color);

			int rows = image.rows;
			int cols = image.cols;

			float sq_radius_in = radius_in * radius_in;
			float sq_radius_out = radius_out * radius_out;

			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					cv::Vec3f& color = image.at<cv::Vec3f>(j, i);
					
					float sq_lelf_up = ((float)j - center.x) * ((float)j - center.x) + ((float)i - center.y) * ((float)i - center.y);
					float sq_lelf_down = ((float)j - center.x) * ((float)j - center.x) + ((float)i + 1.f - center.y) * ((float)i + 1.f - center.y);
					float sq_right_up = ((float)j + 1.f - center.x) * ((float)j + 1.f - center.x) + ((float)i - center.y) * ((float)i - center.y);
					float sq_right_down = ((float)j + 1.f - center.x) * ((float)j + 1.f - center.x) + ((float)i + 1.f - center.y) * ((float)i + 1.f - center.y);

					if ((sq_lelf_up > sq_radius_out && sq_lelf_down > sq_radius_out && sq_right_up > sq_radius_out && sq_right_down > sq_radius_out) ||
						(sq_lelf_up < sq_radius_in && sq_lelf_down < sq_radius_in && sq_right_up < sq_radius_in && sq_right_down < sq_radius_in)){
						color = bg_color;
					}
					else if ((sq_lelf_up < sq_radius_out&& sq_lelf_down < sq_radius_out&& sq_right_up < sq_radius_out&& sq_right_down < sq_radius_out) &&
						(sq_lelf_up > sq_radius_in && sq_lelf_down > sq_radius_in && sq_right_up > sq_radius_in && sq_right_down > sq_radius_in)){
						color = color_in;
					}
					else {
						float ratio = figure_out_color_ratio(radius_in, radius_out, center, cv::Point2f(j, i), n);
						color = color_in * ratio + bg_color * (1. - ratio);
					}
				}
			}

			image.convertTo(image, CV_8UC3);
			return image;
		}

		float figure_out_color_ratio(float radius_in, float radius_out, cv::Point2f center, cv::Point2f pixel, int n) {
			
			int subpixel_in = 0;
			float s = 1. / ((float)n * 2.);

			float sq_radius_in = radius_in * radius_in;
			float sq_radius_out = radius_out * radius_out;

			for (int i = 0; i < n; i++) {
				float inc_y = (float)i / (float)n;

				for (int j = 0; j < n; j++) {	
					float inc_x = (float)j / (float)n;		

					if ((((pixel.x + inc_x + s) - center.x) * ((pixel.x + inc_x + s) - center.x) +
						((pixel.y + inc_y + s) - center.y) * ((pixel.y + inc_y + s) - center.y) < sq_radius_out) &&
						(((pixel.x + inc_x + s) - center.x) * ((pixel.x + inc_x + s) - center.x) +
						((pixel.y + inc_y + s) - center.y) * ((pixel.y + inc_y + s) - center.y) > sq_radius_in)){
						subpixel_in++;
					}
				}
			}

			return (float)subpixel_in / ((float)n*(float)n);
		}
	};
}

#endif //_IMAGE_TEST_H_