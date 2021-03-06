#include "RCube/Core/Graphics/MeshGen/Plane.h"
#include "glm/gtx/string_cast.hpp"

namespace rcube
{

TriangleMeshData plane(float width, float height, unsigned int width_segments, unsigned int height_segments,
               Orientation ort)
{
    TriangleMeshData mesh_data;
    mesh_data.indexed = true;

    float half_width = width / 2.f;
    float half_height = height / 2.f;

    unsigned int width_vertices = width_segments + 1;
    unsigned int height_vertices = height_segments + 1;

    float edge_width = width / width_segments;
    float edge_height = height / height_segments;

    // Rotate vertices and normals according to plane orientation
    glm::quat rotation(1, 0, 0, 0);
    glm::vec3 plane_normal(0, 0, 1);
    if (ort == Orientation::NegativeZ)
    {
        plane_normal = glm::vec3(0, 0, -1);
    }
    else if (ort == Orientation::PositiveX)
    {
        rotation = glm::angleAxis(glm::half_pi<float>(), glm::vec3(0, 1, 0));
        plane_normal = glm::vec3(1, 0, 0);
    }
    else if (ort == Orientation::NegativeX)
    {
        rotation = glm::angleAxis(glm::half_pi<float>(), glm::vec3(0, 1, 0));
        plane_normal = glm::vec3(-1, 0, 0);
    }
    else if (ort == Orientation::PositiveY)
    {
        rotation = glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0));
        plane_normal = glm::vec3(0, 1, 0);
    }
    else if (ort == Orientation::NegativeY)
    {
        rotation = glm::angleAxis(glm::half_pi<float>(), glm::vec3(1, 0, 0));
        plane_normal = glm::vec3(0, -1, 0);
    }
    // Generate vertices, normals and texcoords
    for (unsigned int i = 0; i < height_vertices; ++i)
    {
        float y = static_cast<float>(i) * edge_height - half_height;
        for (unsigned int j = 0; j < width_vertices; ++j)
        {
            float x = j * edge_width - half_width;
            mesh_data.vertices.push_back(rotation * glm::vec3(x, -y, 0));
            mesh_data.normals.push_back(plane_normal);
            mesh_data.texcoords.push_back(
                glm::vec2(static_cast<float>(j) / static_cast<float>(height_segments),
                          1.f - (static_cast<float>(i) / static_cast<float>(width_segments))));
        }
    }
    // Indices
    for (unsigned int i = 0; i < height_segments; ++i)
    {
        for (unsigned int j = 0; j < width_segments; ++j)
        {
            unsigned int a = j + width_vertices * i;
            unsigned int b = j + width_vertices * (i + 1);
            unsigned int c = (j + 1) + width_vertices * (i + 1);
            unsigned int d = (j + 1) + width_vertices * i;
            mesh_data.indices.push_back({a, b, d});
            mesh_data.indices.push_back({b, c, d});
        }
    }
    return mesh_data;
}

} // namespace rcube
