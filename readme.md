# 1. Install OpenCV (https://www.geeksforgeeks.org/how-to-install-opencv-in-c-on-linux/)

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

# 2. Install v4l2loopback


# 3. Compile

Create build files
> `cmake .`

Build

> `make`

Executables

* **OpenCVTest** - Minimal code to test if OpenCV capture device is up and running
* **FaceTest** - Minimal code to test OpenCV face detection
* **LoopbackCameraTest** - Test to feed captured video data into the virtual video device

# Useful tools

https://www.mankier.com/1/v4l2-ctl
> `v4l2-ctl --list-devices` 

Comandline webcam playback and recording     
> `xawtv -c /dev/video1` 

> `sudo modprobe v4l2loopback`

