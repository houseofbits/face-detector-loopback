#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>
#include "ConfigReader.hpp"
#include "CaptureDevice.hpp"
#include "FaceDetector.hpp"
#include "CaffeFaceDetector.hpp"

using namespace cv;
using namespace std;

void drawDetectedFaces(Mat &frame, vector<Rect2d> &faces);

int main(int argc, char **argv)
{
    ConfigReader configReader;
    CaptureDevice captureDevice;
    FaceDetector faceDetector;
    CaffeFaceDetector caffeDetector;

    vector<Rect2d> faces;
    clock_t start;
    clock_t end;
    double fps;

    configReader.read("config.conf");
    int cameraIndex = configReader.getIntValue("openCVCameraIndex", 0);

    string cascadeFilename = configReader.getStringValue("cascadeFilename", "");
    faceDetector.loadCascade(cascadeFilename);
    faceDetector.setDownscale(configReader.getFloatValue("downscale", 0.7));
    faceDetector.setMinFaceSize(configReader.getIntSizeValue("minFaceSize", {60, 60}));
    faceDetector.setMaxFaceSize(configReader.getIntSizeValue("maxFaceSize", {120, 120}));

    captureDevice.open(cameraIndex);

    caffeDetector.load(
        configReader.getStringValue("caffePrototxtFilename"),
        configReader.getStringValue("caffeModelFilename"));

    while (captureDevice.captureFrame())
    {
        end = clock();

        Mat frame = captureDevice.getFrame();

        // faces = faceDetector.detect(frame);
        faces = caffeDetector.detect(frame);

        drawDetectedFaces(frame, faces);

        char c = (char)waitKey(10);
        // Press q to exit from window
        if (c == 27 || c == 'q' || c == 'Q')
        {
            break;
        }

        double seconds = (double(end) - double(start)) / double(CLOCKS_PER_SEC);
        fps = 1.0 / double(seconds);

        putText(frame, "FPS: " + to_string(int(fps)), {10, 20}, FONT_HERSHEY_SIMPLEX, 0.7, {255, 0, 0}, 2);
        imshow("Display Image", frame);

        start = clock();
    }

    return 0;
}

void drawDetectedFaces(Mat &frame, vector<Rect2d> &faces)
{
    Mat faceImg;
    int offset = 0;
    for (size_t i = 0; i < faces.size(); i++)
    {
        Rect r;
        r.x = int((float)frame.size().width * faces[i].x);
        r.y = int((float)frame.size().height * faces[i].y);
        r.width = int((float)frame.size().width * faces[i].width);
        r.height = int((float)frame.size().height * faces[i].height);

        rectangle(frame, r, {255, 0, 0});

        faceImg = frame(r);

        Rect rFace({0, offset}, faceImg.size());

        faceImg.copyTo(frame(rFace));

        offset += faceImg.size().height;
    }
}