
const video = document.querySelector('video');
const mediaSource = new MediaSource();
video.src = URL.createObjectURL(mediaSource);

function permittedGetUserMedia() {
    return !!(navigator.mediaDevices &&
        navigator.mediaDevices.getUserMedia);
}

const mediaDevices = navigator.mediaDevices;

mediaDevices.enumerateDevices().then((result) => {
    console.log(result);
    for (const mediaDeviceInfo of result) {
        if (mediaDeviceInfo.label === 'Virtual Webcam') {
            navigator.mediaDevices.getUserMedia({
                video: {
                    deviceId: mediaDeviceInfo.deviceId
                }
            }).then((stream) => processStream(stream, mediaSource));
        }
    }
});

function processStream(stream, mediaSource) {

    console.log(stream);

    const mediaRecorder = new MediaRecorder(stream)
    const videoBuffer = mediaSource.addSourceBuffer('video/webm;codecs=vp8');

    mediaRecorder.ondataavailable = (data) => {
        let fileReader = new FileReader();
        let arrayBuffer;

        fileReader.onloadend = () => {
            arrayBuffer = fileReader.result;
            videoBuffer.appendBuffer(arrayBuffer)
        }
        fileReader.readAsArrayBuffer(data.data);
    }
    mediaRecorder.start()

    setInterval(() => {
        mediaRecorder.requestData()
    }, 16)
}

