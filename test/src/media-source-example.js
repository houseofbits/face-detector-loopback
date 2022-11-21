export default function run() {
    const mediaDevices = navigator.mediaDevices;

    let isStreaming = false;

    deviceEnumerationProcess();

    function deviceEnumerationProcess() {
        if (!isStreaming) {
            detectDevice('Virtual Webcam');
        }
        setTimeout(deviceEnumerationProcess, 1000);
    }

    function detectDevice(name) {
        mediaDevices.enumerateDevices().then((result) => {
            for (const mediaDeviceInfo of result) {
                if (mediaDeviceInfo.label === name) {
                    navigator.mediaDevices.getUserMedia({
                        video: {
                            deviceId: mediaDeviceInfo.deviceId
                        }
                    }).then((stream) => createSource(stream));
                    return;
                }
            }
        });
    }

    function stopStream(mediaSource) {
        isStreaming = false;
        mediaSource.endOfStream();
    }

    function createSource(stream) {
        const video = document.querySelector('video');
        const mediaSource = new MediaSource();
        video.src = URL.createObjectURL(mediaSource);
        mediaSource.onsourceopen = () => {
            const mediaRecorder = new MediaRecorder(stream)
            const videoBuffer = mediaSource.addSourceBuffer('video/webm;codecs=vp8');
            processStream(mediaSource, mediaRecorder, videoBuffer);
        }
    }

    function processStream(mediaSource, mediaRecorder, videoBuffer) {
        mediaRecorder.ondataavailable = (data) => {
            let fileReader = new FileReader();
            let arrayBuffer;

            fileReader.onloadend = () => {
                if (isStreaming) {
                    arrayBuffer = fileReader.result;
                    videoBuffer.appendBuffer(arrayBuffer);
                }
            }
            fileReader.readAsArrayBuffer(data.data);
        }

        mediaRecorder.onstop = () => {
            stopStream(mediaSource);
        }

        mediaRecorder.onerror = (error) => {
            stopStream(mediaSource);
        }

        mediaRecorder.start();
        isStreaming = true;

        function requestDataProcess() {
            if (mediaRecorder.state === "recording") {
                mediaRecorder.requestData();
                setTimeout(requestDataProcess, 16);
            }
        }

        requestDataProcess();
    }
}