#include "detect.h"

using namespace std;
using namespace cv;

network *netMain;

int metaSize = 0;

vector<string> metaNames;

float colors[6][3] = { {1,0,1}, {0,0,1},{0,1,1},{0,1,0},{1,1,0},{1,0,0} };

float get_color(int c, int x, int max)
{
	float ratio = ((float)x / max) * 5;
	int i = floor(ratio);
	int j = ceil(ratio);
	ratio -= i;
	float r = (1 - ratio) * colors[i][c] + ratio * colors[j][c];
	return r;
}

char* getUCharPtrFromJs(napi_env env, napi_value originString) {
	size_t strlen, res;
	napi_get_value_string_utf8(env, originString, NULL, 0, &strlen);
	char *returnValue = new char[strlen + 1];
	napi_get_value_string_utf8(env, originString, returnValue, strlen + 1, &res);
	return returnValue;
}

napi_value DetectInit(napi_env env, napi_callback_info info) {


	char* cfgfile = "models/yolov3.cfg";
	char* weightfile = "models/yolov3.weights";
	char* metaFile = "models/coco.names";

	netMain = load_network_custom(cfgfile, weightfile, 0, 1);

	ifstream classNamesFile(metaFile);

	if (classNamesFile.is_open()) {
		metaSize = 0;
		string className = "";
		while (getline(classNamesFile, className)) {
			metaNames.push_back(className);
			metaSize++;
		}
	}

	napi_value result;

	napi_create_string_utf8(env, "Init Complete", NAPI_AUTO_LENGTH, &result);

	return result;
}


napi_value DetectImage(napi_env env, napi_callback_info info) {

	size_t argc = 1;
	napi_value argv[1];
	napi_get_cb_info(env, info, &argc, argv, NULL, NULL);

	char* filePath = getUCharPtrFromJs(env, argv[0]);

	Mat im = imread(filePath);
	Mat rgbImg;

	cvtColor(im, rgbImg, cv::COLOR_BGR2RGB);

	float* srcImg;
	size_t srcSize = rgbImg.rows * rgbImg.cols * 3 * sizeof(float);
	srcImg = (float*)malloc(srcSize);

	imgConvert(rgbImg, srcImg);

	float* resizeImg;

	size_t resizeSize = netMain->w * netMain->h * 3 * sizeof(float);

	resizeImg = (float*)malloc(resizeSize);

	imgResize(srcImg, resizeImg, im.cols, im.rows, netMain->w, netMain->h);

	network_predict_ptr(netMain, resizeImg);

	int nboxes = 0;

	float thresh = 0.5f;
	float hier_thresh = 0.5f;
	float nms = 0.35f;

	detection *dets = get_network_boxes(netMain, rgbImg.cols, rgbImg.rows, thresh, hier_thresh, nullptr, 1, &nboxes, 0);

	if (nms) {
		do_nms_sort(dets, nboxes, metaSize, nms);
	}

	vector<cv::Rect>boxes;
	boxes.clear();
	vector<int>classNames;

	for (int i = 0; i < nboxes; i++) {
		bool flag = 0;
		int className;
		for (int j = 0; j < metaSize; j++) {
			if (dets[i].prob[j] > thresh) {
				if (!flag) {
					flag = 1;
					className = j;
				}
			}
		}
		if (flag)
		{
			int left = (dets[i].bbox.x - dets[i].bbox.w / 2.)*im.cols;
			int right = (dets[i].bbox.x + dets[i].bbox.w / 2.)*im.cols;
			int top = (dets[i].bbox.y - dets[i].bbox.h / 2.)*im.rows;
			int bot = (dets[i].bbox.y + dets[i].bbox.h / 2.)*im.rows;

			if (left < 0)
				left = 0;
			if (right > im.cols - 1)
				right = im.cols - 1;
			if (top < 0)
				top = 0;
			if (bot > im.rows - 1)
				bot = im.rows - 1;

			Rect box(left, top, fabs(left - right), fabs(top - bot));
			boxes.push_back(box);
			classNames.push_back(className);
		}
	}
	free_detections(dets, nboxes);

	for (int i = 0; i < boxes.size(); i++)
	{
		int offset = classNames[i] * 123457 % 80;
		float red = 255 * get_color(2, offset, 80);
		float green = 255 * get_color(1, offset, 80);
		float blue = 255 * get_color(0, offset, 80);

		rectangle(im, boxes[i], Scalar(blue, green, red), 2);

		String label = String(metaNames[classNames[i]]);
		int baseLine = 0;
		Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
		putText(im, label, Point(boxes[i].x, boxes[i].y + labelSize.height),
			FONT_HERSHEY_SIMPLEX, 1, Scalar(red, blue, green), 2);
	}

	imshow("Image", im);

	free(srcImg);
	free(resizeImg);
	//free_network(net);

	waitKey(0);

	napi_value result;

	napi_create_string_utf8(env, "Hello World", NAPI_AUTO_LENGTH, &result);

	return result;
}