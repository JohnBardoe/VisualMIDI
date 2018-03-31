// VisualMIDI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lines.h"

int insNum = 7;
int step = 30;
instrument* ins;
Mat m, res;
CvFont font;
void loadGui();
int resolution[] = { GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) };

Mat imageProc(Mat input, int color, int eps) {
	Mat canny;
	blur(input, canny, cv::Size(2, 2));
	cvtColor(canny, canny, CV_BGR2HSV);
	inRange(canny, Scalar(abs(color - eps), 50, 70), Scalar(color + eps, 255, 255), canny); //max hue = 179
	Canny(canny, canny, 50, 200, 3);
	return canny;
}

void correctCanny(int, void*) {
	res = imageProc(m, ins[insNum - 1].color, ins[insNum - 1].eps);
	resize(res, res, Size(round(resolution[0] * 0.5), round(resolution[1] * 0.5)));
	imshow("result", res);
	moveWindow("result", 0, 0);
	waitKey(1);
}

void loadGui() {
	Mat resized;
	resize(m, resized, Size(round(resolution[0] * 0.5), round(resolution[1] * 0.5)));;
	string name = ins[insNum - 1].name;
	namedWindow("VisualMIDI", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO);
	namedWindow("result");
	imshow("result", resized);
	createTrackbar("Instrument", "VisualMIDI", &insNum, 7, correctCanny);
	setTrackbarMin("Instrument", "VisualMIDI", 1);
	createTrackbar("Eps", "VisualMIDI", &ins[insNum - 1].eps, 30, correctCanny);
	createTrackbar("Hue", "VisualMIDI", &ins[insNum - 1].color, 179, correctCanny);
	setTrackbarPos("Hue", "VisualMIDI", ins[insNum - 1].color);
	createTrackbar("Freq step", "VisualMIDI", &step, 150);
	moveWindow("VisualMIDI", round(resolution[0] * 0.5), 0);
	waitKey();
}

int countLines(Mat input) {
	int cntr = 0;
	Mat src = input.col(round(input.cols / 2));
	for (int i = 0; i < input.rows; ++i)
		if (src.at<uchar>(i, 0) == 255) {
			++i;
			while (src.at<uchar>(i, 0) == 255)
				++i;
			while (src.at<uchar>(i, 0) != 255)
				++i;
			while (src.at<uchar>(i, 0) == 255)
				++i;
			++cntr;
		}
	return cntr;
}

curve* detectLines(Mat input, int color, int eps, int& len) {

	Mat canny;
	curve* curves;
	int lCounter = 0;
	canny = imageProc(input, color, eps);

	len = countLines(canny);
	curves = new curve[len];
	if (len == 0) return curves;
	for (int j = 0; j < canny.cols; ++j) {
		lCounter = 0;
		for (int i = 0; i < canny.rows; ++i) {
			if (lCounter == len)
				break;
			if (canny.at<uchar>(i, j) == 255 && i < canny.rows - 1 && j < canny.cols - 1) {
				++i;
				while (canny.at<uchar>(i, j) == 255 && i < canny.rows - 1 && j < canny.cols - 1)
					++i;
				while (canny.at<uchar>(i, j) != 255 && i < canny.rows - 1 && j < canny.cols - 1)
					++i;
				while (canny.at<uchar>(i, j) == 255 && i < canny.rows - 1 && j < canny.cols - 1)
					++i;
				curves[lCounter].points.push_back(Point(i, j - 1));
				++lCounter;
			}

		}
	}
	return curves;
}

void initBass() {
	ins = new instrument[7];
	for (int i = 0; i < 7; ++i) {
		ins[i] = instrument(i);
	}
}

void releaseBass(curve** detected) {
	for (int i = 0; i < 7; ++i) {
		ins[i].~instrument();
	}
}

void playSequence(curve** detected, int* len) {
	HCHANNEL chnls[7];
	int max = m.rows;
	destroyAllWindows();
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < len[i]; j++) {
			if (len != 0)
				for (int stp = 0; stp < detected[i][j].points.size(); stp += step) {
					for (int k = 0; k < 7; k++) {
						BASS_ChannelSetAttribute(ins[k].smp, BASS_ATTRIB_FREQ, detected[i][j].points[stp].y / max * 80000);
						BASS_ChannelPlay(ins[k].smp, FALSE);
					}
					if (GetAsyncKeyState(VK_ESCAPE))
						return;
					Sleep(5 * step);
				}
		}
	}
}
int main(int argc, char** argv)
{
	//init stuff///////////////////
	int len[7];
	curve* detected[7];
	BASS_Init(-1, 44100, 0, 0, NULL);
	BASS_SetVolume(0.05);
	initBass();
	m = imread("image.png", CV_LOAD_IMAGE_COLOR);
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0, 0, 1, CV_AA);
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	//end of init stuff//////////////////
	loadGui();
	Beep(700, 100);
	Beep(1000, 100);
	for (int i = 0; i < 7; i++) {
		detected[i] = detectLines(m, ins[i].color, ins[i].eps, len[i]);
	}
	playSequence(detected, len);
	releaseBass(detected);
	BASS_Free();
	return 0;
}
