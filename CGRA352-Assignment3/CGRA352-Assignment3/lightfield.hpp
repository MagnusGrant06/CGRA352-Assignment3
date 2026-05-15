#include <string>
#include <opencv2/opencv.hpp>
#include "aperture.hpp"
struct LFCam {
	cv::Mat image;
	cv::Vec2f uv;
};

class LightField {
private:
	std::vector<LFCam> lf_cams;

public:
	int rows;
	int cols;

	LightField(std::string filepath, int rows, int cols);

	LFCam* get_lf_cam(int row, int col) {
		return &(lf_cams[row * this->cols + col]);
	}

	void print_core_part1() {
		//get point at [7,10,384,768]
		LFCam* cam = get_lf_cam(7, 10);
		cv::Vec3b pix_color = cam->image.at<cv::Vec3b>(384, 768);
		std::cout << pix_color << std::endl;
	}

	cv::Mat reconstruct_raw_data(Aperture aperture, cv::Range row_range, cv::Range col_range);

	cv::Mat reconstruct(Aperture aperture, float focal_distance);

	float calculate_focal_cost(cv::Rect rect, Aperture aperture, float focal_distance);
};