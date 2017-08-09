#ifndef GRIDMESH_H
#define GRIDMESH_H

#include <vector>
#include <qopengl.h>
#include <QVector3D>
#include <QVector2D>
#include <QString>
#include <QOpenGLFunctions>

using namespace std;

struct Vertex{
    QVector3D position;
    QVector3D normal;
    QVector2D texcoord;
};

class GridMesh
{
public:
    GridMesh();
    GridMesh::GridMesh(QVector2D left_bottom_corner, QVector2D righut_up_corner, int grid_density);
    const Vertex *Data() const {return vertices.data(); }
    const int *Index() const {return indices.data(); }
    int GetVertexNum() const {return vertex_num_; }
    int GetIndexNum() const {return indices.size(); }
private:
    QVector2D left_bottom_corner_;
    QVector2D right_up_corner_;
    QVector2D left_up_corner_;
    QVector2D right_bottom_corner_;
    vector<Vertex> vertices;
    vector<int> indices;
    int index_num_;
    int vertex_num_;
    int density_;

};

#endif // GRIDMESH_H
