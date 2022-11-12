#include <opencv2/opencv.hpp>
#include <stdio.h>

class FaceDetector
{
protected:
    cv::CascadeClassifier cascade;
    cv::Size minFaceSize;
    cv::Size maxFaceSize;
    float downscale;

public:
    FaceDetector()
    {
        downscale = 0.7;
        minFaceSize = {60, 60};
        maxFaceSize = {120, 120};
    }

    void loadCascade(std::string cascadeFilename)
    {
        cascade.load(cascadeFilename.c_str());
    }

    void setMinFaceSize(cv::Size size)
    {
        minFaceSize = size;
    }

    void setMaxFaceSize(cv::Size size)
    {
        maxFaceSize = size;
    }

    void setDownscale(float scale)
    {
        downscale = scale;
    }

    std::vector<cv::Rect2d> detect(cv::Mat &frame)
    {
        std::vector<cv::Rect> faces;
        std::vector<cv::Rect2d> unitFaces;
        cv::Mat gray, smallImg;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::resize(gray, smallImg, cv::Size(), downscale, downscale, cv::INTER_LINEAR);
        cv::equalizeHist(smallImg, smallImg);

        cascade.detectMultiScale(smallImg, faces, 1.1,
                                 2, 0 | cv::CASCADE_SCALE_IMAGE, minFaceSize, maxFaceSize);

        for (size_t i = 0; i < faces.size(); i++)
        {
            cv::Rect r = faces[i];

            cv::Rect2d ur;
            ur.x = r.x / (downscale * frame.size().width);
            ur.width = r.width / (downscale * frame.size().width);
            ur.y = r.y / (downscale * frame.size().height);
            ur.height = r.height / (downscale * frame.size().height);

            unitFaces.push_back(ur);
        }

        return unitFaces;
    }
};