Qt.include("three.js")
Qt.include("parser.js")

// TODO: need to get rid of such large number of parameters
function showStlFile(canvas, filename, scanWidth, scanHeight, camera, scene, renderer, material, light) {
    var vf_data;

    function loadAndParseStlFile() {
        var request = new XMLHttpRequest()
        request.open('GET', filename)
        request.onreadystatechange = function(event) {
            if (request.readyState === XMLHttpRequest.DONE) {
                if (request.responseText.length === 0) {
                    console.warn("Cannot get stl file content");
                    return;
                }

                try
                {
                    vf_data = parse_3d_file(filename, request.responseText);
                    showStlModel();
                }
                catch(err)
                {
                    console.warn("Cannot parse stl file", err.message);
                }
            }
        }
        request.send();
    }

    function showStlModel() {
        var geo = new THREE.Geometry;
        geo.vertices = vf_data.vertices;
        geo.faces = vf_data.faces;
        geo.computeBoundingBox();
        geo.computeFaceNormals();
        geo.computeVertexNormals();

        console.log("BB center(" + geo.boundingBox.center().x +", "+ geo.boundingBox.center().y +", "+ geo.boundingBox.center().z+")");
        var cz = 0.5 * scanHeight / Math.tan(camera.fov * Math.PI / 360);
        console.log("camera center.z =", cz);
        camera.position.set(scanHeight / 2, scanWidth / 2, cz);
        camera.lookAt(new THREE.Vector3(scanHeight / 2, scanWidth / 2, 0));
        camera.farPlane = cz;
        camera.nearPlane = cz - 100;

        var mesh = new THREE.Mesh(geo, material);
        mesh.name = "mesh";
        scene.add(mesh);

        light.position.set( 0, 0, cz);
        light.position.normalize();

        if (typeof canvas.componentInitialized != "undefined") {
            canvas.componentInitialized();
        }
    }

    loadAndParseStlFile();
}

function showNewStlFile(canvas, filename, scanWidth, scanHeight, camera, scene, renderer, material, light) {
    var oldMesh = scene.getObjectByName("mesh");
    scene.remove(oldMesh);
    showStlFile(canvas, filename, scanWidth, scanHeight, camera, scene, renderer, material, light);
}
