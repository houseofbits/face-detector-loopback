    cmake_minimum_required(VERSION 2.8)
    project( OpenCVTest )
    find_package( OpenCV REQUIRED )
    include_directories( ${OpenCV_INCLUDE_DIRS} )

    add_executable( OpenCVTest src/opencv-test.cpp )
    target_link_libraries( OpenCVTest ${OpenCV_LIBS} )

    add_executable( FaceTest src/face-detector-test.cpp )
    target_link_libraries( FaceTest ${OpenCV_LIBS} )

    add_executable( LoopbackCameraTest src/loopback-camera-test.cpp )
    target_link_libraries( LoopbackCameraTest ${OpenCV_LIBS} )

    add_executable( FaceDetectorLoopback src/face-detector-loopback.cpp )
    target_link_libraries( FaceDetectorLoopback ${OpenCV_LIBS} )

