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

