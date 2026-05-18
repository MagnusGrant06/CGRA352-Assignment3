#include "lightfield.hpp"

LightField::LightField(std::string filepath, int rows, int cols) : lf_cams(rows*cols, LFCam()), rows(rows), cols(cols){
	
	// parse all images
	std::cout << "Loading light field ..." << std::endl;
	std::vector<cv::String> lf_imgs;
	cv::glob(filepath, lf_imgs);
	for (cv::String cv_str : lf_imgs) {
		// get the filepath
		std::string filepath(cv_str);
		size_t pos = filepath.find_last_of("/\\");

		if (pos != std::string::npos) {
			// replace "_" with " "
			std::string filename = filepath.substr(pos + 1);
			pos = 0;

			while ((pos = filename.find("_", pos)) != std::string::npos) {
				filename.replace(pos, 1, " ");
				pos++;
			}

			// parse for values
			std::istringstream ss(filename);
			std::string name;
			int row, col;
			float v, u;
			ss >> name >> row >> col >> v >> u;

			if (ss.good()) {
				LFCam* current_cam = get_lf_cam(row, col);
				current_cam->image = cv::imread(filepath);
				current_cam->uv = cv::Vec2f(u, v);
				// TODO something with the image file "filepath"
				// TODO something with the coordinates: row, col, v, u
				
			}
			else {
				// throw error otherwise
				std::cerr << "Filepath error with : " << filepath << std::endl;
				std::cerr << "Expected in the form : [prefix]/[name]_[row]_[col]_[v]_[u][suffix]";
				abort();
			}
		}
		
	}

	std::cout << "Finished loading light field" << std::endl;
}

cv::Mat LightField::reconstruct_raw_data(Aperture aperture, cv::Range row_range, cv::Range col_range) {
	int st_rows = row_range.size() * this->rows;
	int st_cols = col_range.size() * this->cols;

	cv::Mat raw_data(st_rows, st_cols, CV_8UC3, cv::Scalar(0, 0, 0));

	for (int t = 0; t < row_range.size(); t++) {
		for (int s = 0; s < col_range.size(); s++) {

			for (int v = 0; v < this->rows; v++) {
				for (int u = 0; u < this->cols; u++) {

					LFCam* current_cam = get_lf_cam(v, u);
					if (!aperture.transparent(current_cam->uv)) continue;
					raw_data.at<cv::Vec3b>(t * this->rows + v, s * this->cols + u) = current_cam->image.at<cv::Vec3b>(row_range.start+t, col_range.start + s);

				}
			}

		}
	}

	return raw_data;
}

cv::Mat LightField::reconstruct(Aperture aperture, float focal_distance) {

	LFCam* center_cam = get_lf_cam(rows / 2, cols / 2);
	cv::Vec2f center_uv = center_cam->uv;

	int st_rows = center_cam->image.rows;
	int st_cols = center_cam->image.cols;

	cv::Mat result(st_rows, st_cols, CV_32FC3, cv::Scalar(0, 0, 0));
	int count = 0;
	for (LFCam& cam : lf_cams) {

		if (!aperture.transparent(cam.uv)) continue;

		cv::Vec2f uv = center_uv - cam.uv;
		cv::Mat indices(st_rows, st_cols, CV_32FC2);

		for (int t = 0; t < st_rows; t++) {
			for (int s = 0; s < st_cols; s++) {

				indices.at<cv::Vec2f>(t, s) = cv::Vec2f(s, t) + uv * focal_distance;

			}
		}


		cv::Mat out;
		cv::remap(cam.image, out, indices, cv::Mat(), cv::INTER_LINEAR);

		result += out;
		count++;
	}

	result /= count;

	cv::Mat result_u8;
	result.convertTo(result_u8, CV_8UC3);
	return result_u8;
}

float LightField::calculate_focal_cost(cv::Rect rect, Aperture aperture, float focal_distance) {

	LFCam* center_cam = get_lf_cam(rows / 2, cols / 2);
	cv::Vec2f center_uv = center_cam->uv;

	int st_rows = center_cam->image.rows;
	int st_cols = center_cam->image.cols;

	cv::Mat center_sample(rect.height, rect.width, CV_8UC3);
	cv::Mat temp_sample(rect.height, rect.width, CV_8UC3);

	center_sample = center_cam->image(rect);
	 //copy portion of image to rect
	//build up center sample
	//build index

	//remap(center_cam->image, center_sample, index)

	float cost = 0;

	for (LFCam& cam : lf_cams) {

		if (!aperture.transparent(cam.uv)) continue;
		cv::Mat img = cam.image;

		cv::Vec2f uv = center_uv - cam.uv;
		cv::Mat indices(center_sample.rows, center_sample.cols, CV_32FC2);

		for (int t = 0; t < center_sample.rows; t++) {
			for (int s = 0; s < center_sample.cols; s++) {

				indices.at<cv::Vec2f>(t, s) = cv::Vec2f(rect.x + s, rect.y + t) + uv * focal_distance;
				

			}
		}
		//build up index mapping of the current focal distance
		//in the range of the rectangle
		cv::remap(img, temp_sample, indices,cv::Mat(), cv::INTER_LINEAR);
		//std::cout << indices << std::endl;
		cost += cv::norm(center_sample, temp_sample);

	}

	return cost;
}
