# Face detector loopback

Goal with this project is to create a Linux service that
 
 * Detects faces in the Web-camera feed using OpenCV
 * Creates a virtual video output device and pipes detected faces to it.

This project is a proof-of-concept for web kiosk application, where face detection on the frontend side (java script) proved to be expensive and slow.

## 1. Install OpenCV 

For reference: https://www.geeksforgeeks.org/how-to-install-opencv-in-c-on-linux/

Packages and tools:
> `sudo apt install -y g++ cmake make git libgtk2.0-dev pkg-config`

> `git clone https://github.com/opencv/opencv`    

> `mkdir -p build && cd build`

Generate the build scripts using cmake

> `cmake ../opencv`

Build the source using make

> `make -j4`

Install the OpenCV package

> `sudo make install`

## 2. Install v4l2loopback

...

## 3. Compile

Create build files
> `cmake .`

Build

> `make`

Executables

* **OpenCVTest** - Minimal code to test if OpenCV capture device is up and running
* **FaceTest** - Minimal code to test OpenCV face detection
* **LoopbackCameraTest** - Test to feed captured video data into the virtual video device

## Useful tools

For reference: https://www.mankier.com/1/v4l2-ctl
> `v4l2-ctl --list-devices` 

Comandline webcam playback and recording     
> `xawtv -c /dev/video1` 

> `sudo modprobe v4l2loopback`

