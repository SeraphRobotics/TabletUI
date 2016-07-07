Qt.include("scene_helper.js")

var camera, scene, renderer, light;
var material;

function initializeGL(canvas, filename, scanWidth, scanHeight) {
    console.log("canvas.width=", canvas.width, "canvas.height=", canvas.height);
    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(30, canvas.width / canvas.height, 1, 1000);
    camera.up.set(0, 1, 0);
    scene.add(camera);

    renderer = new THREE.Canvas3DRenderer({canvas: canvas,
                                           devicePixelRatio: canvas.devicePixelRatio,
                                           antialias: true,
                                           alpha: true});
    renderer.setSize(canvas.width, canvas.height);

    light = new THREE.DirectionalLight(0xffffff, 0.9);
    camera.add(light);

    material = new THREE.MeshLambertMaterial({color: 0x909090,
                                              verdraw: 1,
                                              wireframe: false,
                                              shading: THREE.FlatShading,
                                              vertexColors: THREE.FaceColors,
                                              side: THREE.DoubleSide});

    showStlFile(canvas, filename, scanWidth, scanHeight, camera, scene, renderer, material, light);
}

function paintGL(canvas) {
    renderer.render(scene, camera);
}

function resizeGL(canvas) {
    if (camera === undefined)
        return;

    camera.aspect = canvas.width / canvas.height;
    camera.updateProjectionMatrix();

    renderer.setSize(canvas.width, canvas.height);
}

function setNewStlFile(canvas, filename, scanWidth, scanHeight) {
    if (camera === undefined)
        return;

    showNewStlFile(canvas, filename, scanWidth, scanHeight, camera, scene, renderer, material, light)
}
