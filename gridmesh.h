#ifndef GRIDMESH_H
#define GRIDMESH_H

#include <vector>
#include <qopengl.h>
#include <QVector3D>
#include <QVector2D>
#include <QString>
#include <QOpenGLFunctions>
#include <opencv2/opencv.hpp>

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

    // initialize origin image(the texture)
    void InitImage(cv::Mat& origin);
    // adjust z factor
    void AdjustZfactor(float z_factor_delta);
    // adjust the denoise_image_
    void ChangeDenoiseImage();
    // adjust the blur image
    void BlurImage();
    // adjust the density


    cv::Mat origin_;
    cv::Mat denoise_image_;
    cv::Mat blur_image_;
    cv::Mat final_blend_;
private:
    void GenZFromFinal();
    QVector2D left_bottom_corner_;
    QVector2D right_up_corner_;
    QVector2D left_up_corner_;
    QVector2D right_bottom_corner_;
    vector<Vertex> vertices;
    vector<int> indices;
    int index_num_;
    int vertex_num_;
    int density_;
    int density_x_;
    int density_y_;

    float grid_width_;
    float grid_height_;
    int image_width_;
    int image_height_;

    int gaussian_kernel_size_;
    float z_factor_;

};

#endif // GRIDMESH_H
