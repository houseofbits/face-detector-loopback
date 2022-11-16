# Face detector loopback

Goal of this project is to create a Linux service which does the following
 
 * Detects a single face in the Web-camera feed using OpenCV
 * Creates a virtual video output device and streams detected face image to it.
 * Ensures the necesarry configuration so that the virtual video stream can be picked up by the browser (Chromium)

This project is a proof-of-concept for use in web kiosk application, where face detection on the frontend side (java script) proved to be expensive and slow. Thus, the need for a BE solution arise.

## 1. Install OpenCV 

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

For reference: https://www.geeksforgeeks.org/how-to-install-opencv-in-c-on-linux/

## 2. Install v4l2loopback

https://github.com/umlaeute/v4l2loopback

...


> `sudo modprobe -r v4l2loopback`

> `sudo modprobe v4l2loopback devices=1 video_nr=21 exclusive_caps=1 card_label="Virtual Webcam"`

Note: To open this media device in browser it has to have data streamed to it.

## 3. Compile

Create build files
> `cmake .`

Build

> `make`

Executables

* **OpenCVTest** - Minimal code to test if OpenCV capture device is up and running
* **FaceTest** - Minimal code to test OpenCV face detection with various algorithms
* **LoopbackCameraTest** - Test to feed captured video data into the virtual video device
* **FaceDetectorLoopback** - Full face detection with streaming to loopback device

## 4. System configuration (Arch)

1. Create a service to run `./FaceDetectorLoopback` and restart on failure
2. Make sure that it depends on availablity of `/dev/video21`
 
> `sudo nano /etc/udev/rules.d/98-virtual-video.rules`

add

    KERNEL=="video21", SYMLINK="video21", TAG+="systemd"

Service

```
[Unit]
BindsTo=dev-video21.device
After=dev-video21.device
...
```

## Useful tools

> `v4l2-ctl --list-devices` 

Comandline webcam playback   
> `xawtv -c /dev/video21` 

## Links for reference 
 * https://www.scs.stanford.edu/~dm/blog/hide-webcam.html
 * https://stackoverflow.com/questions/68433415/using-v4l2loopback-virtual-cam-with-google-chrome-or-chromium-on-linux-while-hav
 * https://github.com/umlaeute/v4l2loopback/issues/274