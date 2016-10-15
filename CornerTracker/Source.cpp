#pragma once

#define NOMINMAX
#include <Windows.h>
#include <opencv2\opencv.hpp>

#include <atltime.h>

#define A_THRESH_VAL -5
#define DOT_THRESH_VAL_MIN 10  // ドットノイズ弾き
#define DOT_THRESH_VAL_MAX 15 // エッジノイズ弾き


int main(int argc, char** argv)
{
    cv::VideoCapture cap(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
	cap.set(CV_CAP_PROP_FPS, 30);

    const int cycle = 10;

	bool refresh = true;

    CFileTime cTimeStart, cTimeEnd;
    CFileTimeSpan cTimeSpan;


    cv::Mat prevFrame;
    cv::Size frameSize = prevFrame.size();
    cap >> prevFrame;

	std::vector<cv::Point2f> prevCorners;

    cv::waitKey(cycle);

    while (1) {
		cTimeStart = CFileTime::GetCurrentTime();           // 現在時刻

        cv::Mat frame;
        cap >> frame;
		cv::Mat drawframe = frame.clone();

        cv::Mat prevFrameGray;
        cv::Mat currFrameGray;

        cv::cvtColor(prevFrame, prevFrameGray, CV_RGB2GRAY);
        cv::cvtColor(frame, currFrameGray, CV_BGR2GRAY);

//適応的閾値処理のテスト
#if 0
		//適応的閾値処理の確認//
		cv::Mat adapBinImg, binImg;
		cv::adaptiveThreshold(currFrameGray, adapBinImg, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 7, A_THRESH_VAL);
		cv::threshold(currFrameGray, binImg, 150, 255, cv::THRESH_BINARY);

		cv::Mat resizeAdap, resizeBin;
		cv::resize(adapBinImg, resizeAdap, cv::Size(adapBinImg.cols * 0.5, adapBinImg.rows * 0.5));
		cv::resize(binImg, resizeBin, cv::Size(binImg.cols * 0.5, binImg.rows * 0.5));

		cv::imshow("Threshold", resizeBin);
		cv::imshow("adaptiveThreshold", resizeAdap);
		int key = cv::waitKey(cycle);
		//適応的閾値処理の確認//
#endif

//コーナー点の検出とトラッキング
#if 1
        // 特徴点抽出
        std::vector<cv::Point2f> currCorners; //->whileの外に出してみる

        std::vector<uchar> featuresFound;
        std::vector<float> featuresErrors;

		if(refresh){
		    //cTimeStart = CFileTime::GetCurrentTime();           // 現在時刻
			cv::goodFeaturesToTrack(currFrameGray, currCorners, 200, 0.01, 50.0);
			cv::cornerSubPix(currFrameGray, currCorners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03));
			//cv::cornerSubPix(currFrameGray, currCorners, cv::Size(3, 3), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
			//cTimeEnd = CFileTime::GetCurrentTime();           // 現在時刻
			//cTimeSpan = cTimeEnd - cTimeStart;
			//std::cout<< "cv::goodFeaturesToTrack():" << cTimeSpan.GetTimeSpan()/10000 << "[ms]" << std::endl;
		}else{
		    cTimeStart = CFileTime::GetCurrentTime();           // 現在時刻
			cv::calcOpticalFlowPyrLK(
				prevFrameGray,
				currFrameGray,
				prevCorners,
				currCorners,
				featuresFound,
				featuresErrors);
			cv::cornerSubPix(currFrameGray, currCorners, cv::Size(3, 3), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03));
			cTimeEnd = CFileTime::GetCurrentTime();           // 現在時刻
			cTimeSpan = cTimeEnd - cTimeStart;
			std::cout<< "cv::calcOpticalFlowPyrLK():" << cTimeSpan.GetTimeSpan()/10000 << "[ms]" << std::endl;
		}
        for (int i = 0; i < currCorners.size(); i++) {
            cv::Point p2 = cv::Point((int) currCorners[i].x, (int) currCorners[i].y);
			if(refresh)	{
				//cv::putText(drawframe,std::to_string(i), cv::Point((int) currCorners[i].x, (int) currCorners[i].y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0,0,255));
				cv::circle(drawframe, p2, 1, cv::Scalar(0,0,255), 2);
			}else{
				if(featuresErrors[i] <= 30.0f) //画面外に飛び出てる
				{
				cv::putText(drawframe,std::to_string(i), cv::Point((int) currCorners[i].x, (int) currCorners[i].y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255,0,0));
				cv::circle(drawframe, p2, 3, cv::Scalar(255,0,0), 2);
				}
			}
        }

		//コーナー検出結果表示
		cv::Mat resize_cam;
		//cv::resize(drawframe, resize_cam, cv::Size(), 0.8, 0.8);
		cv::imshow("preview", drawframe);
        prevFrame = frame;
		prevCorners = currCorners;

		int key = cv::waitKey(cycle);

		if (key == 27) break;
		else if(key == 32){
			refresh = !refresh;
		}
#endif

#if 0

        // 特徴点抽出
        std::vector<cv::Point2f> prevCorners;
        std::vector<cv::Point2f> currCorners;

        cv::goodFeaturesToTrack(prevFrameGray, prevCorners, 20, 0.05, 5.0);
        cv::goodFeaturesToTrack(currFrameGray, currCorners, 20, 0.05, 5.0);
        cv::cornerSubPix(prevFrameGray, prevCorners, cv::Size(21, 21), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, 0.01));
        cv::cornerSubPix(currFrameGray, currCorners, cv::Size(21, 21), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, 0.01));

        std::vector<uchar> featuresFound;
        std::vector<float> featuresErrors;

        cv::calcOpticalFlowPyrLK(
            prevFrameGray,
            currFrameGray,
            prevCorners,
            currCorners,
            featuresFound,
            featuresErrors);

        for (int i = 0; i < featuresFound.size(); i++) {
            cv::Point p1 = cv::Point((int) prevCorners[i].x, (int) prevCorners[i].y);
            cv::Point p2 = cv::Point((int) currCorners[i].x, (int) currCorners[i].y);

			//cv::putText(drawframe,std::to_string(i), cv::Point((int) currCorners[i].x, (int) currCorners[i].y), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255,0,0));
			//cv::circle(drawframe, p2, 3, cv::Scalar(255,0,0), 2);
            cv::line(drawframe, p1, p2, cv::Scalar(0, 0, 255), 2);
        }

        cv::imshow("preview", drawframe);
        prevFrame = frame;
        if (cv::waitKey(cycle) == 27) { break; }
#endif
		cTimeEnd = CFileTime::GetCurrentTime();           // 現在時刻
		cTimeSpan = cTimeEnd - cTimeStart;
		std::cout<< cTimeSpan.GetTimeSpan()/10000 << "[ms]" << std::endl;
	}


    return 0;
}