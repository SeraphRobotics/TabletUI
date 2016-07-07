//Dynamic object creation
//Qt3d ScenLoader doesn not handle source change properly
//In order to load different source we need to recreate qml object

//Code for View3d.qml

var view3d_component;
var view3d_source;
var view3d_parent;
var view3d_object;
var view3d_foot;

function createView3d(source, parent, foot) {
    view3d_source = source
    view3d_parent = parent
    view3d_foot = foot
    view3d_component = Qt.createComponent("../../../Components/View3d/View3d.qml");
    if (view3d_component.status === Component.Ready)
        finishView3d();
    else
        view3d_component.statusChanged.connect(finishView3d);
}

function finishView3d() {
    if (view3d_component.status === Component.Ready) {
        view3d_object = view3d_component.createObject(view3d_parent,
                                                      {"anchors.fill": view3d_parent,
                                                       "meshSource": view3d_source,
                                                       "footSide": view3d_foot});
        if (view3d_object === null) {
            // Error Handling
            console.log("Error creating View3d object");
        }
    } else if (view3d_component.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:", view3d_component.errorString());
    }
}


//Code for View3dPerspective.qml

var perspective3d_component;
var perspective3d_source_left;
var perspective3d_source_right;
var perspective3d_parent;
var perspective3d_post_function;


function createPerspective3d(sourceLeft, sourceRight, parent, post_function) {
    perspective3d_source_left = sourceLeft
    perspective3d_source_right = sourceRight
    perspective3d_parent = parent
    perspective3d_post_function = post_function
    perspective3d_component = Qt.createComponent("../../../RegularScreens/SettingsPagePrivateComponents/ReviewSubpage/View3dPerspective.qml");
    if (perspective3d_component.status === Component.Ready)
        finishPerspective3d();
    else
        perspective3d_component.statusChanged.connect(finishPerspective3d);
}

function finishPerspective3d() {
    if (perspective3d_component.status === Component.Ready) {
        var object = perspective3d_component.createObject(perspective3d_parent,
                                                          {"anchors.fill": perspective3d_parent,
                                                           "leftSource": perspective3d_source_left,
                                                           "rightSource": perspective3d_source_right});
        if (object === null) {
            // Error Handling
            console.log("Error creating object");
        }
        perspective3d_post_function( object )
    } else if (perspective3d_component.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:", perspective3d_component.errorString());
    }
}

//Code for View3dPosting.qml

var posting3d_component;
var posting3d_source;
var posting3d_parent;
var posting3d_object;

function createPosting3d(source, parent) {
    posting3d_source = source
    posting3d_parent = parent
    posting3d_component = Qt.createComponent("../../../Components/View3d/View3dPosting.qml");
    if (posting3d_component.status === Component.Ready)
        finishPosting3d();
    else
        posting3d_component.statusChanged.connect(finishPosting3d);
}

function finishPosting3d() {
    if (posting3d_component.status === Component.Ready) {
        posting3d_object = posting3d_component.createObject(posting3d_parent,
                                                            {"anchors.fill": posting3d_parent,
                                                             "meshSource": posting3d_source});
        if (posting3d_object === null) {
            // Error Handling
            console.log("Error creating posting3d object");
        }
    } else if (posting3d_component.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:", posting3d_component.errorString());
    }
}
