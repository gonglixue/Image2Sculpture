#include "gridmesh.h"

GridMesh::GridMesh()
{

}

GridMesh::GridMesh(QVector2D left_bottom_corner, QVector2D righut_up_corner, int grid_density)
{
    ResetParams();

    density_x_ = density_y_ = density_ = grid_density;
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
    ResetParams();

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
            int current_left_up_corner_id = row * density_x_ + col;

            indices.push_back(current_left_up_corner_id);
            indices.push_back(current_left_up_corner_id + density_x_);
            indices.push_back(current_left_up_corner_id + 1);


            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + density_x_);
            indices.push_back(current_left_up_corner_id + 1 + density_x_);

        }
    }

    // denoise
    DenoiseImage(this->contra_value_, this->morph_mode_);

    // blur
    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();


}

void GridMesh::DenoiseImage(float contra_value, bool white_noise)
{
    contra_value_ = contra_value;
    // 增强对比度
    cv::Mat contra_img;
    origin_.convertTo(contra_img, -1, contra_value, 0);

    //morph_mode_ = white_noise;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(morph_kernel_size_, morph_kernel_size_));
    if(white_noise){
        cv::erode(contra_img, contra_img, kernel);
        cv::dilate(contra_img, contra_img, kernel);
    }else{
        cv::dilate(contra_img, contra_img, kernel);
        cv::erode(contra_img, contra_img, kernel);
    }

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
    EstimateVertexNormal();

    vertex_num_ = vertices.size();
}

float GridMesh::MapGrey2Z(float grey, int mode)
{
    float result;
    switch(mode){
    case 1:
        result = grey;
        break;
    case 2:
        result = -1 * (grey - 1)*(grey - 1) + 1;
        break;
    case 3:
        result = -1 * pow(abs(grey - 1), 3) + 1;
        break;
    case 4:
        result = -1 * pow((grey - 1), 4) + 1;
        break;
    default:
        result = -1 * (grey - 1)*(grey - 1) + 1;
    }

    return result;
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

void GridMesh::Reverse()
{
    reverse_ ^=  reverse_;
    morph_mode_ ^= morph_mode_;

    cv::Mat FullMat(origin_.rows, origin_.cols, CV_8UC1, cv::Scalar(255));
    origin_ = FullMat - origin_;

    DenoiseImage(this->contra_value_, this->morph_mode_);
    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::ChangeContraValue(float contra_value)
{
    this->contra_value_ = contra_value;
    DenoiseImage(contra_value, this->morph_mode_);
    // blur
    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::ChangeMorphKernelSize(int kernel_size)
{
    this->morph_kernel_size_ = kernel_size;
}

void GridMesh::ErodeAndDilate()
{
    DenoiseImage(this->contra_value_, true);
    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::DilateAndErode()
{
    DenoiseImage(this->contra_value_, false);
    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::ChangeGKernelSize(int kernel_size)
{
    this->gaussian_kernel_size_ = kernel_size;
    BlurImage(kernel_size, this->gaussian_sigma_);

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();

}

void GridMesh::ChangeGSigma(float sigma)
{
    this->gaussian_sigma_ = sigma;
    BlurImage(this->gaussian_kernel_size_, sigma);

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::ChangeBlend_a(float a)
{
    blend_factor_a_ = a;

    // blend denoise and blur_img
    cv::addWeighted(denoise_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::ResetParams()
{
    gaussian_kernel_size_ = DEFAULT_GKERNEL;
    gaussian_sigma_ = DEFAULT_GSIGMA;
    mid_kernel_size_ = DEFAULT_MIDKERNEL;
    morph_kernel_size_ = DEFAULT_MORPHKERNEL;
    z_factor_ = DEFAULT_ZFACTOR;
    contra_value_ = DEFAULT_CONTRA;
    blend_factor_a_ = DEFAULT_BLENDA;
    morph_mode_ = true;
}

void GridMesh::EstimateVertexNormal()
{
    // 邻接面法线相加，归一
    for(int row=0; row<density_y_; row++)
    {
        for(int col=0; col<density_x_; col++)
        {
            int vertex_id = row * density_x_ + col;
            Vertex v = vertices[vertex_id];

            // 四角
            if((row==0&&col==0) || (row==0&&col==density_x_-1)
                    || (row==density_y_-1 && col==0) || (row==density_y_-1&&col==density_x_-1)){
                vertices[vertex_id].normal = QVector3D(0, 0, 1);
            }

            // 上边界
            else if(row == 0){
                Vertex v0 = vertices[vertex_id - 1];
                Vertex v1 = vertices[vertex_id + 1];
                Vertex v2 = vertices[vertex_id+density_x_-1];
                Vertex v3 = vertices[vertex_id+density_x_];
                Vertex v4 = vertices[vertex_id+density_x_+1];

                QVector3D v_v0 = v0.position - v.position;
                QVector3D v_v2 = v2.position - v.position;
                QVector3D v_v3 = v3.position - v.position;
                QVector3D v_v1 = v1.position - v.position;

                QVector3D normal1 = QVector3D::crossProduct(v_v0, v_v2);
                QVector3D normal2 = QVector3D::crossProduct(v_v2, v_v3);
                QVector3D normal3 = QVector3D::crossProduct(v_v3, v_v1);

                vertices[vertex_id].normal = (normal1 + normal2 +normal3).normalized();

            }
            // 左边界
            else if(col == 0){
                Vertex v0 = vertices[vertex_id + density_x_];
                Vertex v1 = vertices[vertex_id + 1];
                Vertex v2 = vertices[vertex_id + 1 - density_x_];
                Vertex v3 = vertices[vertex_id - density_x_];

                QVector3D v_v0 = v0.position - v.position;
                QVector3D v_v1 = v1.position - v.position;
                QVector3D v_v2 = v2.position - v.position;
                QVector3D v_v3 = v3.position - v.position;

                QVector3D normal1 = QVector3D::crossProduct(v_v0, v_v1);
                QVector3D normal2 = QVector3D::crossProduct(v_v1, v_v2);
                QVector3D normal3 = QVector3D::crossProduct(v_v2, v_v3);

                vertices[vertex_id].normal = (normal1 + normal2 + normal3).normalized();
            }

            // 右边界
            else if(col == density_x_-1){
                Vertex v0 = vertices[vertex_id - density_x_];
                Vertex v1 = vertices[vertex_id + density_x_];
                Vertex v2 = vertices[vertex_id + density_x_ - 1];
                Vertex v3 = vertices[vertex_id - 1];

                QVector3D v_v0 = v0.position - v.position;
                QVector3D v_v1 = v1.position - v.position;
                QVector3D v_v2 = v2.position - v.position;
                QVector3D v_v3 = v3.position - v.position;

                QVector3D normal1 = QVector3D::crossProduct(v_v0, v_v3);
                QVector3D normal2 = QVector3D::crossProduct(v_v3, v_v2);
                QVector3D normal3 = QVector3D::crossProduct(v_v2, v_v1);

                vertices[vertex_id].normal = (normal1 + normal2 + normal3).normalized();

            }

            // 下边界
            else if(row == density_y_ - 1)
            {
                Vertex v0 = vertices[vertex_id - 1];
                Vertex v1 = vertices[vertex_id - density_x_];
                Vertex v2 = vertices[vertex_id - density_x_ + 1];
                Vertex v3 = vertices[vertex_id + 1];
                QVector3D v_v0 = v0.position - v.position;
                QVector3D v_v1 = v1.position - v.position;
                QVector3D v_v2 = v2.position - v.position;
                QVector3D v_v3 = v3.position - v.position;

                QVector3D normal1 = QVector3D::crossProduct(v_v1, v_v0);
                QVector3D normal2 = QVector3D::crossProduct(v_v2, v_v1);
                QVector3D normal3 = QVector3D::crossProduct(v_v3, v_v2);
                vertices[vertex_id].normal = (normal1 + normal2 + normal3).normalized();
            }

            // 非边界点
            else{
                Vertex v0 = vertices[vertex_id - density_x_];
                Vertex v1 = vertices[vertex_id - 1];
                Vertex v2 = vertices[vertex_id - 1 + density_x_];
                Vertex v3 = vertices[vertex_id + density_x_];
                Vertex v4 = vertices[vertex_id + 1];
                Vertex v5 = vertices[vertex_id + 1 - density_x_];

                QVector3D v_v0 = v0.position - v.position;
                QVector3D v_v1 = v1.position - v.position;
                QVector3D v_v2 = v2.position - v.position;
                QVector3D v_v3 = v3.position - v.position;
                QVector3D v_v4 = v4.position - v.position;
                QVector3D v_v5 = v5.position - v.position;

                QVector3D normal1 = QVector3D::crossProduct(v_v0, v_v1);
                QVector3D normal2 = QVector3D::crossProduct(v_v1, v_v2);
                QVector3D normal3 = QVector3D::crossProduct(v_v2, v_v3);
                QVector3D normal4 = QVector3D::crossProduct(v_v3, v_v4);
                QVector3D normal5 = QVector3D::crossProduct(v_v4, v_v5);
                QVector3D normal6 = QVector3D::crossProduct(v_v5, v_v0);

                vertices[vertex_id].normal = (normal1 + normal2 + normal3 + normal4 + normal5 + normal6).normalized();
            }



        }
    }
}
