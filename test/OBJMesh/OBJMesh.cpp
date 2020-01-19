#include "RCube/Core/Graphics/Effects/GammaCorrectionEffect.h"
#include "RCube/Core/Graphics/MeshGen/Obj.h"
#include "RCubeViewer/RCubeViewer.h"

int main()
{
    using namespace rcube;

    // Properties to configure the viewer
    viewer::RCubeViewerProps props;
    props.resolution = glm::vec2(1280 /*4096*/, 720/*2160*/); // 720p
    props.MSAA = 2; // turn on 2x multisampling

    //Create a viewer
    viewer::RCubeViewer viewer(props);

    //Load obj file 
    std::string input_obj_file = std::string(OBJ_RESOURCE_PATH) + "/" +                                 
                                 "armadillo.obj";
    MeshData mesh = rcube::loadOBJ(input_obj_file);

    //make sure the mesh is OK    
    //assert(mesh.valid());

    //Add the loaded obj to the viewer
    EntityHandle mesh_handle = viewer.addSurface("OBJMesh", rcube::loadOBJ(input_obj_file));

    //This is such a huge mesh. Gotta scale down a bit
    mesh_handle.get<Transform>()->setScale(glm::vec3(0.01, 0.01, 0.01));

    //Change its diffuse color by getting the Drawable component
    const auto &material = mesh_handle.get<Drawable>()->material;
    material->uniform("material.diffuse").set(glm::vec3(0.0, 0.3, 0.7));
    material->uniform("show_wireframe").set(true);

    //Apply gamma correction to the screen
    viewer.camera().get<Camera>()->postprocess.push_back(makeGammaCorrectionEffect());

    //Show viewer
    viewer.execute();

    return 0;    
}