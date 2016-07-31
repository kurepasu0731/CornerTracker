#pragma once

#define NOMINMAX
#include <Windows.h>
#include <opencv2\opencv.hpp>

int main(int argc, char** argv)
{
    cv::VideoCapture cap(0);

    const int cycle = 30;

	bool refresh = true;


    cv::Mat prevFrame;
    cv::Size frameSize = prevFrame.size();
    cap >> prevFrame;

	std::vector<cv::Point2f> prevCorners;

    cv::waitKey(cycle);

    while (1) {
        cv::Mat frame;
        cap >> frame;
		cv::Mat drawframe = frame.clone();

        cv::Mat prevFrameGray;
        cv::Mat currFrameGray;

        cv::cvtColor(prevFrame, prevFrameGray, CV_RGB2GRAY);
        cv::cvtColor(frame, currFrameGray, CV_RGB2GRAY);

        // 特徴点抽出
        std::vector<cv::Point2f> currCorners;

        std::vector<uchar> featuresFound;
        std::vector<float> featuresErrors;

		if(refresh){
			cv::goodFeaturesToTrack(currFrameGray, currCorners, 100, 0.01, 10.0);
			cv::cornerSubPix(currFrameGray, currCorners, cv::Size(21, 21), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, 0.01));
		}else{
			cv::calcOpticalFlowPyrLK(
				prevFrameGray,
				currFrameGray,
				prevCorners,
				currCorners,
				featuresFound,
				featuresErrors);
			cv::cornerSubPix(currFrameGray, currCorners, cv::Size(21, 21), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, 0.01));
		}
        for (int i = 0; i < currCorners.size(); i++) {
            cv::Point p2 = cv::Point((int) currCorners[i].x, (int) currCorners[i].y);
			if(refresh)	{
				cv::putText(drawframe,std::to_string(i), cv::Point((int) currCorners[i].x, (int) currCorners[i].y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,0,255));
				cv::circle(drawframe, p2, 3, cv::Scalar(0,0,255), 2);
			}else{
				if(featuresErrors[i] <= 30.0f) //画面外に飛び出てる
				{
				cv::putText(drawframe,std::to_string(i), cv::Point((int) currCorners[i].x, (int) currCorners[i].y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255,0,0));
				cv::circle(drawframe, p2, 3, cv::Scalar(255,0,0), 2);
				}
			}
        }

        cv::imshow("preview", drawframe);
        prevFrame = frame;
		prevCorners = currCorners;

		int key = cv::waitKey(cycle);

		if (key == 27) break;
		else if(key == 32){
			refresh = !refresh;
		}
    }
    return 0;
}