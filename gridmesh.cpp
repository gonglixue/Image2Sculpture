#include "gridmesh.h"

GridMesh::GridMesh()
{

}

GridMesh::GridMesh(QVector2D left_bottom_corner, QVector2D righut_up_corner, int grid_density)
{
    density_x_ = density_y_ = density_ = grid_density;
    gaussian_kernel_size_ = 3;
    gaussian_sigma_ = 5;
    mid_kernel_size_ = 3;
    z_factor_ = 0.06;
    contra_value_ = 1.5;
    blend_factor_a_ = 0.6;
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
            indices.push_back(current_left_up_corner_id + grid_density);
            indices.push_back(current_left_up_corner_id + 1);


            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + grid_density);
            indices.push_back(current_left_up_corner_id + 1 + grid_density);

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

void GridMesh::InitImage(cv::Mat& origin, bool reverse)
{
    if(reverse)
    {
        cv::Mat FullMat(origin.rows, origin.cols, CV_8UC1, cv::Scalar(255));
        origin = FullMat - origin;
    }

    this->origin_ = origin;
    this->reverse_ = reverse;
    density_x_ = 512;
    density_y_ = 512 * (1.0 * origin.rows / origin.cols);

    grid_width_ = 1.0f * (right_up_corner_.x() - left_bottom_corner_.x()) / (density_x_ - 1);
    grid_height_ = 1.0f * (right_up_corner_.y() - left_bottom_corner_.y()) / (density_y_ - 1);
    image_width_ = origin.cols;
    image_height_ = origin.rows;

    // initialize indices
    indices.clear();
    for (int row = 0; row < density_y_-1; row++)
    {
        for (int col = 0; col < density_x_-1; col++) {
            int current_left_up_corner_id = row * density_y_ + col;

            indices.push_back(current_left_up_corner_id);
            indices.push_back(current_left_up_corner_id + density_x_);
            indices.push_back(current_left_up_corner_id + 1);


            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + density_x_);
            indices.push_back(current_left_up_corner_id + 1 + density_x_);

        }
    }

    // denoise
    ChangeDenoiseImage();

    // blur
    BlurImage();

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();


}

void GridMesh::ChangeDenoiseImage(float contra_value)
{
    contra_value_ = contra_value;
    // 增强对比度
    cv::Mat contra_img;
    origin_.convertTo(contra_img, -1, contra_value, 0);

    // 中值滤波
    cv::medianBlur(contra_img, this->denoise_image_, mid_kernel_size_);

    cv::imshow("denoise image", denoise_image_);
}

void GridMesh::BlurImage(int kernel_size, float sigma)
{
    gaussian_kernel_size_ = kernel_size;
    gaussian_sigma_ = sigma;

    cv::GaussianBlur(denoise_image_, blur_image_, cv::Size(kernel_size, kernel_size), sigma, sigma);
    cv::imshow("gaussian blur", blur_image_);
}

void GridMesh::GenMeshData()
{
    vertices.clear();
    for(int row=0; row<density_y_; row++)
    {
        for(int col=0; col<density_x_; col++)
        {
            QVector2D texcoord = QVector2D(
                        1.0*col/(density_x_ - 1),
                        1.0*row/(density_y_ - 1)
                        );

            float posZ = this->MapGrey2Z(final_blend_.at<uchar>(texcoord.y()*(image_height_ - 1),
                                                                texcoord.x()*(image_width_ - 1))/255.0);
            QVector3D position = QVector3D(
                        left_up_corner_.x() + col*grid_width_,
                        left_up_corner_.y() - row*grid_height_,
                        posZ * this->z_factor_
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
}

float GridMesh::MapGrey2Z(float grey)
{
    // x
    //float result = grey;

    // -（x-1)^2 + 1
    float result = -1 * (grey - 1)*(grey - 1) + 1;

    // -(x-1)^4 + 1
    //float result = -1 * pow((grey - 1), 4) + 1;

    // -(|x-1|)^3 + 1
    //float result = -1 * pow(abs(grey - 1), 3) + 1;

    return result;
}

void GridMesh::GenZFromFinal()
{

}

void GridMesh::AdjustZfactor(float z_factor)
{
    float z_factor_ratio = z_factor / z_factor_;
    this->z_factor_ = z_factor;

    for (int i = 0; i < vertices.size(); i++)
    {
        this->vertices[i].position.setZ(
                    vertices[i].position.z() * z_factor_ratio
                    );
    }
}

void GridMesh::SaveMeshToFile(QString file_name)
{
    std::cout<<"save mesh...\n";

    ofstream out_file(file_name.toStdString());
    out_file << "#density:" <<density_x_<<","<<density_y_<<endl;
    out_file << "#gaussian kernel:" <<"size="<<gaussian_kernel_size_<<",sigma="<<gaussian_sigma_<<endl;
    out_file << "#z_factor:"<<z_factor_<<endl;

    for(int i=0; i<vertices.size(); i++){
        out_file << "v ";
        out_file << vertices[i].position.x() << " "
            << vertices[i].position.y() << " "
            << vertices[i].position.z() << endl;
    }
    for (int j = 0; j < vertices.size(); j++)
    {
        out_file << "vt ";
        out_file << vertices[j].texcoord.x() << " "
            << vertices[j].texcoord.y() << endl;
    }
    for (int i = 0; i < indices.size()/3; i++)
    {
        out_file << "f ";
        out_file << indices[3 * i + 0]+1 << " "
            << indices[3 * i + 1] +1 << " "
            << indices[3 * i + 2] +1 << endl;
    }

    out_file.close();
    std::cout << "finish save obj.\n";
}
