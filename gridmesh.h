#ifndef GRIDMESH_H
#define GRIDMESH_H

#include <vector>
#include <qopengl.h>
#include <QVector3D>
#include <QVector2D>
#include <QString>
#include <QOpenGLFunctions>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <string>
#include "constant.h"

using namespace std;

struct Vertex{
    QVector3D position;
    QVector3D normal;
    QVector2D texcoord;

};

enum InterImageType{
    CONTRA_IMAGE,
    DENOISE_IMAGE,
    BLUR_IMAGE,
    FINAL_BLEND_IAMGE
};

class GridMesh
{
public:
    GridMesh();
    GridMesh::GridMesh(QVector2D left_bottom_corner, QVector2D righut_up_corner, int grid_density);
    const Vertex *Data() const {return vertices.data(); }
    const int *Index() const {return indices.data(); }
    int GetVertexNum() const {return vertices.size(); }
    int GetIndexNum() const {return indices.size(); }
    int GetDensityX() const {return density_x_;}
    int GetDensityY() const {return density_y_;}

    // initialize origin image(the texture)
    void InitImage(cv::Mat& origin, bool reverse=false);
    // adjust z factor
    void AdjustZfactor(float new_z_factor);
    // adjust the denoise_image_
    void DenoiseImage(float contra_value=1.5f, bool white_noise=true);
    // adjust the blur image
    void BlurImage(int kernel_size=3, float sigma=5.0f);
    // adjust the density

    // save mesh to file
    void SaveMeshToFile(QString file_name);

    // Reverse interface
    void Reverse();
    // change contra_value interface
    void ChangeContraValue(float contra_value);
    // change morph_kernel_size interface
    void ChangeMorphKernelSize(int kernel_size);
    // erode and dilate
    void ErodeAndDilate();
    void DilateAndErode();
    // change gaussian kenel size
    void ChangeGKernelSize(int kernel_size);
    // change gaussian sigma
    void ChangeGSigma(float sigma);
    // change blend_a
    void ChangeBlend_a(float a);
    // change zmap mode
    void ChangeZMapMode(int mode);
    // change blend_b
    void ChangeBlend_b(float b);

    // Show intermediate result in a image window
    void ShowInterImage(InterImageType inter);

    cv::Mat origin_;
    cv::Mat denoise_image_;
    cv::Mat blur_image_;
    cv::Mat final_blend_;
    cv::Mat contra_image_;

private:
    void GenMeshData();
    float MapGrey2Z(float grey);
    void ResetParams();
    void EstimateVertexNormal();

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

    // image parameters
    bool reverse_;
    int gaussian_kernel_size_;
    float gaussian_sigma_;
    int mid_kernel_size_;
    int morph_kernel_size_;  //形态学处理核
    float z_factor_;
    float bright_value_;
    float contra_value_;   //对比度 0~3.0
    float blend_factor_a_;
    float blend_factor_high_;
    bool morph_mode_;       // true先腐蚀后膨胀
    int zmap_mode_;
    float blend_factor_b_;




};

#endif // GRIDMESH_H
