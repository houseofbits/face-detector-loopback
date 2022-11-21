import * as BABYLON from 'babylonjs';

const mediaDevices = navigator.mediaDevices;

const canvas = document.getElementById('renderCanvas');

const engine = new BABYLON.Engine(canvas, true, {preserveDrawingBuffer: true, stencil: true});

function createVirtualCameraTexture(name, material, scene) {
    mediaDevices.enumerateDevices().then((result) => {
        for (const mediaDeviceInfo of result) {
            if (mediaDeviceInfo.label === name) {
                mediaDevices.getUserMedia({
                    video: {deviceId: mediaDeviceInfo.deviceId}
                }).then((stream) => {
                    BABYLON.VideoTexture.CreateFromStreamAsync(scene, stream).then((videoTexture) => {
                        material.diffuseTexture = videoTexture;

                        videoTexture.video.addEventListener('progress', (evt) => {
                            console.log("is active: " + videoTexture.video.srcObject.active);
                        });
                    });
                });

                return;
            }
        }
    });
}

function createScene() {
    const scene = new BABYLON.Scene(engine);

    const camera = new BABYLON.FreeCamera('camera1', new BABYLON.Vector3(0, 5, -10), scene);
    camera.setTarget(BABYLON.Vector3.Zero());
    camera.attachControl(canvas, false);
    const light = new BABYLON.HemisphericLight('light1', new BABYLON.Vector3(0, 1, 0), scene);

    const ground = BABYLON.Mesh.CreateGround('ground1', 6, 6, 2, scene, false);
    const material = new BABYLON.StandardMaterial("mat", scene);
    ground.material = material;

    createVirtualCameraTexture('Virtual Webcam', material, scene);

    return scene;
}

export default function run() {
    const scene = createScene();
    engine.runRenderLoop(function () {
        scene.render();
    });
    window.addEventListener('resize', function () {
        engine.resize();
    });
}