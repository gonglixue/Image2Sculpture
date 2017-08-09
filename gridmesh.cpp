#include "gridmesh.h"

GridMesh::GridMesh()
{

}

GridMesh::GridMesh(QVector2D left_bottom_corner, QVector2D righut_up_corner, int grid_density)
{
    density_ = grid_density;
    left_bottom_corner_ = left_bottom_corner;
    right_up_corner_ = righut_up_corner;
    left_up_corner_ = QVector2D(left_bottom_corner.x(), righut_up_corner.y());
    right_bottom_corner_ = QVector2D(righut_up_corner.x(), left_bottom_corner.y());

    float grid_width = 1.0f * (right_up_corner_.x() - left_bottom_corner.x())/(grid_density-1);
    float grid_height = 1.0f * (right_up_corner_.y() - left_bottom_corner.y())/(grid_density-1);

    // initialize indices
    for (int row = 0; row < grid_density-1; row++)
    {
        for (int col = 0; col < grid_density-1; col++) {
            int current_left_up_corner_id = row * grid_density + col;

            indices.push_back(current_left_up_corner_id);
            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + grid_density);

            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + 1 + grid_density);
            indices.push_back(current_left_up_corner_id + grid_density);
        }
    }

    // initialize mesh data
    for (int row = 0; row < grid_density; row++)
    {
        for (int col = 0; col < grid_density; col++)
        {

            QVector3D position = QVector3D(
                        left_up_corner_.x() + col*grid_width,
                        left_up_corner_.y() - row*grid_height,
                        0
            );
            QVector2D texcoord = QVector2D(
                        1.0 * col / (grid_density-1),
                        1.0 * row / (grid_density-1)

                        );
            QVector3D normal = QVector3D(0, 0, 1);


            Vertex v;
            v.position = position;
            v.texcoord = texcoord;
            v.normal = normal;

            vertices.push_back(v);
        }
    }

    vertex_num_ = vertices.size();
    index_num_ = indices.size();
}
