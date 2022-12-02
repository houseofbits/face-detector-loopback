#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>
#include "Exception.hpp"
#include "ConfigReader.hpp"
#include "CaptureDevice.hpp"
#include "FaceDetector.hpp"
#include "CaffeFaceDetector.hpp"
#include "OutputVideoDevice.hpp"
#include "DetectionResultInterpolator.hpp"

using namespace std;

cv::Mat getImageFromRect(cv::Mat &frame, cv::Rect2d rect, cv::Size2f outputSize, float fixedAspectRatio, cv::Size2f expandX, cv::Size2f expandY);

int main(int argc, char **argv)
{
    try
    {
        ConfigReader configReader;
        CaptureDevice captureDevice;
        CaffeFaceDetector caffeDetector;
        OutputVideoDevice outputVideoDevice;
        DetectionResultInterpolator detectionResultInterpolator;
        vector<cv::Rect2d> faces;
        cv::Mat faceImg;
        clock_t start;
        clock_t end;

        configReader.read("config.conf");
        cv::Size2f outputSize = configReader.getSizeValue("outputSize", {320, 240});

        cout << "Configuration file loaded" << endl;

        captureDevice.open(configReader.getIntValue("openCVCameraIndex", 0));

        cout << "Video capture device opened" << endl;

        outputVideoDevice.setSize(outputSize.width, outputSize.height);
        outputVideoDevice.open(configReader.getStringValue("loopbackDevide", ""));

        cout << "Loopback device opened" << endl;

        caffeDetector.load(
            configReader.getStringValue("caffePrototxtFilename"),
            configReader.getStringValue("caffeModelFilename"));

        cout << "Caffe face detector models loaded" << endl;

        float fixedAspectRatio = configReader.getFloatValue("fixedAspectRatio", 1.4);
        cv::Size2f expandX = configReader.getSizeValue("expandX", {0.4, 0.4});
        cv::Size2f expandY = configReader.getSizeValue("expandY", {0.1, 0.8});
        bool shouldShowWindow = configReader.getBoolValue("shouldShowWindow", true);
        int inputRotate = configReader.getIntValue("inputRotate", 0);
        int inputFlip = configReader.getIntValue("inputFlip", 0);

        cout << "Settings loaded" << endl;

        cout << "Running" << endl;

        while (true)
        {
            char c = (char)cv::waitKey(10);
            if (c == 27 || c == 'q' || c == 'Q')
            {
                break;
            }

            try
            {
                end = clock();

                captureDevice.captureFrame();

                double frameTime = (double(end) - double(start)) / double(CLOCKS_PER_SEC);

                frameTime = fmax(0.1, frameTime);

                cv::Mat frame = captureDevice.getFrame(inputRotate, inputFlip);

                faces = caffeDetector.detect(frame);

                cv::Rect2d rect = detectionResultInterpolator.getInterpolatedResult(faces, frameTime);

                faceImg = getImageFromRect(frame, rect, outputSize, fixedAspectRatio, expandX, expandY);

                if (shouldShowWindow)
                {
                    cv::imshow("Display Image", faceImg);
                }

                outputVideoDevice.writeOpenCVImage(faceImg);

                start = clock();
            }
            catch (cv::Exception e)
            {

                cout << "Exception: " << e.msg << endl;

                continue;
            }
        }
    }
    catch (Exception e)
    {
        cout << "Fatal exception: " << e.getMessage() << endl;
    }
    return 0;
}

cv::Mat getImageFromRect(cv::Mat &frame, cv::Rect2d rect, cv::Size2f outputSize, float fixedAspectRatio, cv::Size2f expandX, cv::Size2f expandY)
{
    cv::Mat faceImg, imgResized;
    cv::Rect r;
    int offset = 0;
    float frameWidth = frame.size().width;
    float frameHeight = frame.size().height;

    r.x = int(frameWidth * rect.x);
    r.y = int(frameHeight * rect.y);
    r.width = int(frameWidth * rect.width);
    r.height = int(frameHeight * rect.height);
    float expandX1 = (float)r.width * expandX.width;
    float expandX2 = (float)r.width * expandX.height;
    float expandY1 = (float)r.height * expandY.width;
    float expandY2 = (float)r.height * expandY.height;

    r.x = r.x - expandX1;
    r.y = r.y - expandY1;
    r.width = r.width + (expandX1 + expandX2);
    r.height = r.height + (expandY1 + expandY2);

    float newWidth = r.height / fixedAspectRatio;

    r.x = r.x + ((r.width - newWidth) / 2);
    r.width = newWidth;

    float offsetex = 0;
    if (r.x + r.width > frameWidth)
    {
        offsetex = fabs(frameWidth - (r.x + r.width));
        r.width = frameWidth - r.x;
    }
    float offsetey = 0;
    if (r.y + r.height > frameHeight)
    {
        offsetey = fabs(frameHeight - (r.y + r.height));
        r.height = frameHeight - r.y;
    }
    float offsetsx = 0;
    if (r.x < 0)
    {
        offsetsx = fabs(r.x);
        r.x = 0;
    }
    float offsetsy = 0;
    if (r.y < 0)
    {
        offsetsy = fabs(r.y);
        r.y = 0;
    }

    faceImg = frame(r);

    cv::Mat largerImage = cv::Mat::zeros(cv::Size(offsetsx + r.width + offsetex, offsetsy + r.height + offsetey), frame.type());
    cv::Rect rFace({(int)offsetsx, (int)offsetsy}, faceImg.size());

    faceImg.copyTo(largerImage(rFace));

    cv::resize(largerImage, imgResized, outputSize);

    return imgResized;
}