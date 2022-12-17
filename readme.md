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

Install `linux linux-headers v4l2loopback-dkms v4l2loopback-utils` packages

Load module at boot

> `/etc/modules-load.d/v4l2loopback.conf`

add 
    
    v4l2loopback

> `/etc/modprobe.d/v4l2loopback.conf`

add

    options v4l2loopback video_nr=21
    options v4l2loopback card_label="Virtual Webcam"   
    options v4l2loopback exclusive_caps=1

> `sudo mkinitcpio -P`    



v4l2-ctl -d /dev/video21 --list-formats-ext

v4l2loopback-ctl set-caps /dev/video21 "YU12:640x480"

sudo rmmod v4l2loopback

v4l2-ctl -d /dev/video0 -c timeout=3000

v4l2loopback-ctl set-timeout-image -t 3000 /dev/video0 service-unavailable.png

For reference: https://github.com/umlaeute/v4l2loopback


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

## 4. Example system configuration for Arch Linux based deployment

### 1. Install dependencies (OpenCV, v4l2loopback)

> `sudo pacman -S opencv qt5-base`

> `sudo pacman -S linux linux-headers v4l2loopback-dkms v4l2loopback-utils`

### 2. Create virtual device
> `sudo modprobe v4l2loopback devices=1 video_nr=21 exclusive_caps=1 card_label="Virtual Webcam"`

Remove device, if necessary
> `sudo modprobe -r v4l2loopback`

### 3. Create a systemd service to run `./FaceDetectorLoopback` and restart on failure

### 4. Make sure that the service depends on availablity of virtual video device
 
> `sudo nano /etc/udev/rules.d/98-virtual-video.rules`

add

    KERNEL=="video21", SYMLINK="video21", TAG+="systemd"

Update service to depend on the device

```
[Unit]
BindsTo=dev-video21.device
After=dev-video21.device
...
```

### 5. Example of front-end application

> /test/


## Useful tools

> `v4l2-ctl --list-devices` 

Comandline webcam playback   
> `xawtv -c /dev/video21` 

## 6. Reference 
 * https://www.scs.stanford.edu/~dm/blog/hide-webcam.html
 * https://stackoverflow.com/questions/68433415/using-v4l2loopback-virtual-cam-with-google-chrome-or-chromium-on-linux-while-hav
 * https://github.com/umlaeute/v4l2loopback/issues/274