#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>

/**
 * Cudos for the dnn model to:
 * https://www.youtube.com/watch?v=piaEXzNkowY&t=974s
*/
class CaffeFaceDetector
{
protected:
    cv::dnn::Net net;

public:
    void load(std::string protoFileName, std::string caffemodelFileName)
    {
        net = cv::dnn::readNetFromCaffe(protoFileName.c_str(), caffemodelFileName.c_str());
        if (net.empty())
        {
            std::cerr << "Can't load network by using the following files: " << std::endl;
            std::cerr << "prototxt:   " << protoFileName << std::endl;
            std::cerr << "caffemodel: " << caffemodelFileName << std::endl;
        }
    }

    std::vector<cv::Rect2d> detect(cv::Mat &frame)
    {
        std::vector<cv::Rect2d> unitFaces;

        cv::Mat img2;
        cv::resize(frame, img2, cv::Size(300, 300));
        cv::Mat inputBlob = cv::dnn::blobFromImage(img2, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));

        net.setInput(inputBlob);
        cv::Mat detection = net.forward();
        cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

        float confidenceThreshold = 0.2;
        for (int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2);

            if (confidence > confidenceThreshold)
            {
                int idx = static_cast<int>(detectionMat.at<float>(i, 1));
                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                cv::Rect r((int)xLeftBottom, (int)yLeftBottom,
                           (int)(xRightTop - xLeftBottom),
                           (int)(yRightTop - yLeftBottom));

                cv::Rect2d unitRect;
                unitRect.x = (float)r.x / (float)frame.size().width;
                unitRect.y = (float)r.y / (float)frame.size().height;
                unitRect.width = (float)r.width / (float)frame.size().width;
                unitRect.height = (float)r.height / (float)frame.size().height;

                unitFaces.push_back(unitRect);
            }
        }

        return unitFaces;
    }
};