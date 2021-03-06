#include "RubikState.h"

/* White - Green - Red - Blue - Orange - Yellow */
std::vector<Scalar> minColor{ Scalar(70, 20, 130), Scalar(60, 110, 110), Scalar(120, 120, 140), Scalar(80, 180, 190), Scalar(5, 150, 150), Scalar(20, 100, 100) };
std::vector<Scalar> maxColor{ Scalar(180, 110, 255), Scalar(100, 220, 250), Scalar(180, 250, 200), Scalar(120, 255, 255), Scalar(15, 235, 250), Scalar(40, 255, 255) };

void RubikState::setCameraId(int camera_id){
	this->camera_id = camera_id;
}

int RubikState::getCameraId(){
	return this->camera_id;
}

void RubikState::setSquareCount(int square_count){
	this->square_count = square_count;
}

int RubikState::getSquareCount(){
	return this->square_count;
}

void RubikState::setWindowName(String window_name){
	this->window_name = window_name;
}

String RubikState::getWindowName(){
	return this->window_name;
}
/* 60 60
	55 65 
		55 65*/
bool RubikState::filterRect(Rect rec){

	if (rec.x < 100 || rec.x > 450){
		return false;
	}
	else if (rec.y < 30 || rec.y > 300){
		return false;
	}

	if (rec.width == 78 && rec.height == 78){
		return true;
	}

	if (rec.width > 75 && rec.width < 85){
		if (rec.height > 75 && rec.height < 85){
			return true;
		}
	}

	return false;
}

String RubikState::defineColorText(int color_id){
	switch (color_id){
		case 0:
			return "W";
		case 1:
			return "G";
		case 2:
			return "R";
		case 3:
			return "B";
		case 4:
			return "O";
		case 5:
			return "Y";
		default:
			return "";
	}
}

RubikState::RubikState(){
	this->setCameraId(0);
	this->setSquareCount(0);
	this->setWindowName("Default Rubik Window");
}

RubikState::RubikState(int camera_id){
	this->setCameraId(camera_id);
	this->setSquareCount(0);
	this->setWindowName("Default Rubik Window");
}

RubikState::RubikState(int camera_id, String window_name){
	this->setCameraId(camera_id);
	this->setSquareCount(0);
	this->setWindowName(window_name);
}

void RubikState::launchCapture(){
	VideoCapture cap(camera_id);

	cap.set(CV_CAP_PROP_SETTINGS, 1);

	if (!cap.isOpened())  // if not success, exit program
	{
		std::cout << "Cannot open the video file" << std::endl;
		return;
	}

	namedWindow(this->getWindowName(), CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	std::vector<std::vector<cv::Point> > contours;
	std::vector<Vec4i> hierarchy;

	Mat frame_RGB;
	Mat filter;
	Mat frame_HSV;
	Mat frame_threshed;
	Mat imgray;
	Mat result;

	std::vector<std::vector<SquareRubik>> results;
	std::vector<std::vector<std::vector<cv::Point>>> finalContours;

	int nb_capture = 0;

	while (nb_capture < NB_CAPTURE)
	{
		std::vector<SquareRubik> points;

		this->setSquareCount(0);
		cap.read(frame_RGB); // read a new frame from video
		bilateralFilter(frame_RGB, filter, 9, 75, 75);
		cvtColor(filter, frame_HSV, cv::COLOR_BGR2HSV);

		for (int i = 0; i < minColor.size(); i++){
			inRange(frame_HSV, minColor[i], maxColor[i], frame_threshed);
			imgray = frame_threshed;

			threshold(frame_threshed, result, 127, 255, 0);

			findContours(result, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
			finalContours.push_back(contours);
		}

		for (unsigned int j = 0; j < finalContours.size(); j++){
			String text = defineColorText(j);
			
			for (unsigned int i = 0; i < finalContours[j].size(); i++){
				Rect rec = cv::boundingRect(finalContours[j][i]);

				if (filterRect(rec)){
					this->setSquareCount(this->getSquareCount() + 1);
					rectangle(frame_RGB, rec, Scalar(0, 255, 0), 2);
					putText(frame_RGB, text, cv::Point2f(rec.x + rec.width / 2, rec.y + rec.height / 2), cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0, 0));
					points.push_back(SquareRubik(cv::Point2f(rec.x, rec.y), ColorRubik(j)));
				}
			}
		}

		imshow(this->getWindowName(), frame_RGB);

		// Found all cube of a side!
		if (this->getSquareCount() == 9){
			points = sortResult(points);

			printSide(points);
			results.push_back(points);

			nb_capture++;
			cv::waitKey();
		}

		finalContours.clear();
		hierarchy.clear();
		if (cv::waitKey(20) == 'q') return;
	}


}

void RubikState::printSide(std::vector<SquareRubik> points){
	for (int i = 0; i < points.size(); i++){
		std::cout << points.at(i).color;
		if ((i + 1) % 3 == 0){
			std::cout << std::endl;
		}
	}
}

std::vector<SquareRubik> RubikState::sortResult(std::vector<SquareRubik> points){
	std::sort(points.begin(), points.end(), RubikState::sortYAxis);
	std::sort(points.begin(), points.begin() + 3, RubikState::sortXAxis);
	std::sort(points.begin() + 3, points.begin() + 6, RubikState::sortXAxis);
	std::sort(points.begin() + 6, points.end(), RubikState::sortXAxis);

	return points;
}