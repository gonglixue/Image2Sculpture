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

    // initialize front-face indices
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

    int front_face_vert_num = indices.size();
    // initialize back-face indices
    for(int row=0; row<grid_density-1; row++)
    {
        for(int col=0; col<grid_density-1; col++)
        {
            int current_left_up_corner_id = row*grid_density + col + density_x_*density_y_;

            indices.push_back(current_left_up_corner_id);
            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + grid_density);


            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + 1 + grid_density);
            indices.push_back(current_left_up_corner_id + grid_density);

        }
    }

    //左侧
    for(int row=0; row<density_y_-1; row++)
    {
        int ru_id = row * density_x_;

        indices.push_back(ru_id);
        indices.push_back(ru_id + density_x_ + density_x_*density_y_);
        indices.push_back(ru_id + density_x_);

        indices.push_back(ru_id);
        indices.push_back(ru_id + density_x_*density_y_);
        indices.push_back(ru_id + density_x_ + density_x_*density_y_);
    }
    //上侧
    for(int col=0; col<density_x_-1; col++)
    {
        int ld_id = col;

        indices.push_back(ld_id);
        indices.push_back(ld_id+1);
        indices.push_back(ld_id+1 + density_x_*density_y_);

        indices.push_back(ld_id);
        indices.push_back(ld_id + 1 + density_x_*density_y_);
        indices.push_back(ld_id + density_x_*density_y_);
    }
    // 右侧
    for(int row=0; row<density_y_-1; row++)
    {
        int lu_id = row*density_x_ + (density_x_ -1);

        indices.push_back(lu_id);
        indices.push_back(lu_id + density_x_);
        indices.push_back(lu_id + density_x_*density_y_);

        indices.push_back(lu_id + density_x_*density_y_);
        indices.push_back(lu_id + density_x_);
        indices.push_back(lu_id + density_x_ + density_x_*density_y_);
    }
    //下侧
    for(int col=0; col<density_x_-1; col++)
    {
        int lu_id = (density_y_-1)*density_x_ + col;

        indices.push_back(lu_id);
        indices.push_back(lu_id + density_x_*density_y_);
        indices.push_back(lu_id + 1);

        indices.push_back(lu_id+1);
        indices.push_back(lu_id + density_x_*density_y_);
        indices.push_back(lu_id + 1 + density_x_*density_y_);
    }

    // initialize front-face mesh data
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

    // initialize back-face mesh data
    for(int row=0; row<grid_density; row++)
    {
        for(int col=0; col<grid_density; col++)
        {
            QVector3D position = QVector3D(
                        left_up_corner_.x() + col*grid_width,
                        left_up_corner_.y() - row*grid_height,
                        -0.3f
            );
            QVector2D texcoord = QVector2D(
                        1.0 * col / (grid_density-1),
                        1.0 * row / (grid_density-1)
                        );
            QVector3D normal = QVector3D(0, 0, -1);

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
    //density_x_ = 512;
    //density_y_ = 512 * (1.0 * origin.rows / origin.cols);
    if(origin.rows <= DEFAULT_MAXIMUM_DENSITY && origin.cols <=DEFAULT_MAXIMUM_DENSITY){
        density_x_ = origin.cols;
        density_y_ = origin.rows;
    }
    else{
        //density_x_ = 2048;
        //density_y_ = 2048 * (1.0 * origin.rows / origin.cols);
        if(origin.cols > origin.rows)
        {
            density_x_ = DEFAULT_MAXIMUM_DENSITY;
            density_y_ = DEFAULT_MAXIMUM_DENSITY * (1.0*origin.rows/origin.cols);
        }
        else{
            density_y_ = DEFAULT_MAXIMUM_DENSITY;
            density_x_ = DEFAULT_MAXIMUM_DENSITY * (1.0*origin.cols/origin.rows);
        }
    }

    grid_width_ = 1.0f * (right_up_corner_.x() - left_bottom_corner_.x()) / (density_x_ - 1);
    grid_height_ = 1.0f * (right_up_corner_.y() - left_bottom_corner_.y()) / (density_y_ - 1);
    image_width_ = origin.cols;
    image_height_ = origin.rows;

    // initialize indices
    indices.clear();
    // front
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
    // back
    for(int row=0; row<density_y_-1; row++)
    {
        for(int col=0; col<density_x_-1; col++)
        {
            int current_left_up_corner_id = row*density_x_ + col + density_x_*density_y_;

            indices.push_back(current_left_up_corner_id);
            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + density_x_);

            indices.push_back(current_left_up_corner_id + 1);
            indices.push_back(current_left_up_corner_id + 1 + density_x_);
            indices.push_back(current_left_up_corner_id + density_x_);
        }
    }
    //左侧
    for(int row=0; row<density_y_-1; row++)
    {
        int ru_id = row * density_x_;

        indices.push_back(ru_id);
        indices.push_back(ru_id + density_x_ + density_x_*density_y_);
        indices.push_back(ru_id + density_x_);

        indices.push_back(ru_id);
        indices.push_back(ru_id + density_x_*density_y_);
        indices.push_back(ru_id + density_x_ + density_x_*density_y_);
    }
    //上侧
    for(int col=0; col<density_x_-1; col++)
    {
        int ld_id = col;

        indices.push_back(ld_id);
        indices.push_back(ld_id+1);
        indices.push_back(ld_id+1 + density_x_*density_y_);

        indices.push_back(ld_id);
        indices.push_back(ld_id + 1 + density_x_*density_y_);
        indices.push_back(ld_id + density_x_*density_y_);
    }
    // 右侧
    for(int row=0; row<density_y_-1; row++)
    {
        int lu_id = row*density_x_ + (density_x_ -1);

        indices.push_back(lu_id);
        indices.push_back(lu_id + density_x_);
        indices.push_back(lu_id + density_x_*density_y_);

        indices.push_back(lu_id + density_x_*density_y_);
        indices.push_back(lu_id + density_x_);
        indices.push_back(lu_id + density_x_ + density_x_*density_y_);
    }
    //下侧
    for(int col=0; col<density_x_-1; col++)
    {
        int lu_id = (density_y_-1)*density_x_ + col;

        indices.push_back(lu_id);
        indices.push_back(lu_id + density_x_*density_y_);
        indices.push_back(lu_id + 1);

        indices.push_back(lu_id+1);
        indices.push_back(lu_id + density_x_*density_y_);
        indices.push_back(lu_id + 1 + density_x_*density_y_);
    }


    // denoise
    DenoiseImage(this->contra_value_, this->morph_mode_);

    // blur
    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    // blend origin and dist_field
    GenFinalBlendImage();
    //cv::imshow("final", final_blend_);

    // use final blend image to generate z
    GenMeshData();


}

cv::Mat EraseSingleNoise(const cv::Mat& input)
{
    //cv::imshow("origin beform erases single noise", input);
    cv::Mat origin = input;

    int height = origin.rows;
    int width = origin.cols;
    int count = 0;

    for(int i=1; i<height-2; i++)
    {
        for(int j=1; j<width-2; j++)
        {
            int center = origin.at<uchar>(i, j);
            int p1 = origin.at<uchar>(i-1, j-1);
            int p2 = origin.at<uchar>(i-1, j);
            int p3 = origin.at<uchar>(i-1, j+1);
            int p4 = origin.at<uchar>(i, j-1);
            int p5 = origin.at<uchar>(i, j+1);
            int p6 = origin.at<uchar>(i+1, j-1);
            int p7 = origin.at<uchar>(i+1, j);
            int p8 = origin.at<uchar>(i+1, j+1);
            if(p1==p2 && p1==p3 && p1==p4 && p1==p5 && p1==p6 && p1==p7 && p1== p8)
            {
                if(center != p1){
                    origin.at<uchar>(i, j) = p1;
                    count++;
                }
            }
        }
    }

    //std::cout << "nums of single noise:" << count << std::endl;
    //cv::imshow("erase single noise:", origin);

    return origin;
}

void GridMesh::DenoiseImage(float contra_value, bool white_noise)
{
    contra_value_ = contra_value;
    // 增强对比度
    cv::Mat contra_img;
    origin_.convertTo(contra_img, -1, contra_value, (1.0-contra_value)*80);
    //this->contra_image_ = contra_img;
    this->contra_image_ = EraseSingleNoise(contra_img);

    //morph_mode_ = white_noise;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(morph_kernel_size_, morph_kernel_size_));
    std::cout << "morph kernel size:" << morph_kernel_size_ << std::endl;

    cv::Mat morph_mat;
    if(white_noise){
        cv::erode(contra_img, morph_mat, kernel);
        cv::dilate(morph_mat, morph_mat, kernel);

        //cv::erode(morph_mat, morph_mat, kernel);
        //cv::dilate(morph_mat, morph_mat, kernel);
    }else{
        cv::dilate(contra_img, morph_mat, kernel);
        cv::erode(morph_mat, morph_mat, kernel);

        //cv::dilate(morph_mat, morph_mat, kernel);
        //cv::erode(morph_mat, morph_mat, kernel);
    }

    // 中值滤波
    cv::medianBlur(morph_mat, this->denoise_image_, mid_kernel_size_);
    //this->denoise_image_ = morph_mat;

    //cv::imshow("Denoise", denoise_image_);


}

void GridMesh::BlurImage(int kernel_size, float sigma)
{
    gaussian_kernel_size_ = kernel_size;
    gaussian_sigma_ = sigma;

    cv::GaussianBlur(denoise_image_, blur_image_, cv::Size(kernel_size, kernel_size), sigma, sigma);
    //cv::imshow("Gaussian Blur After Denoise", blur_image_);

    //cv::Sobel(this->denoise_image_, contour_image_, CV_8UC1, 1, 1);
    // before detect edge, denoise a bit
    //temp = this->denoise_image_;
    //cv::blur(this->blur_image_, temp, cv::Size(3,3));
    float lower_threshould = 30;
    //cv::Canny(this->blur_image_, contour_image_, lower_threshould, 3*lower_threshould, 3 );
    //cv::imshow("contour", contour_image_);
    cv::Mat FullMat(origin_.rows, origin_.cols, CV_8UC1, cv::Scalar(255));
    temp = FullMat - blur_image_;
    // temp thresh binary
    cv::threshold(temp, temp, 200, 255, CV_THRESH_BINARY);
    //cv::imshow("temp", temp);

    cv::distanceTransform(temp, dist_field_image_, CV_DIST_L2, 3);
    cv::normalize(dist_field_image_, dist_field_image_, 0, dist_normal_range_, CV_MINMAX);
    cv::imshow("distance", dist_field_image_);
}

void GridMesh::GenMeshData()
{
    vertices.clear();
    // front-face
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

    // back-face
    for(int row=0; row<density_y_; row++)
    {
        for(int col=0; col<density_x_; col++)
        {
            Vertex front_vert = vertices[row*density_x_ + col];

            //float posZ = this->MapGrey2Z(final_blend_.at<uchar>(texcoord.y()*(image_height_ - 1),
                                                                //texcoord.x()*(image_width_ - 1))/255.0);
            QVector3D position = QVector3D(
                        left_up_corner_.x() + col*grid_width_,
                        left_up_corner_.y() - row*grid_height_,
                        front_vert.position.z() - this->thickness_
                        );

            QVector3D normal = QVector3D(0, 0, -1);

            Vertex v;
            v.position = position;
            v.texcoord = front_vert.texcoord;
            v.normal = normal;
            vertices.push_back(v);
        }
    }

    EstimateVertexNormal();

    vertex_num_ = vertices.size();
}

// 0~1
float GridMesh::MapGrey2Z(float grey)
{
    float result;
    int mode = this->zmap_mode_;
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

    for (int i = 0; i < vertices.size()-4; i++)
    {
        this->vertices[i].position.setZ(
                    vertices[i].position.z() * z_factor_ratio
                    );
    }

    this->EstimateVertexNormal();
}

void GridMesh::SaveMeshToFile(QString file_name)
{
    std::cout<<"save mesh...\n";
    float yx_ration = origin_.rows*1.0f/origin_.cols;

    ofstream out_file(file_name.toStdString());
    out_file << "#density:" <<density_x_<<","<<density_y_<<endl;
    out_file << "#gaussian kernel:" <<"size="<<gaussian_kernel_size_<<",sigma="<<gaussian_sigma_<<endl;
    out_file << "#z_factor:"<<z_factor_<<endl;

    for(int i=0; i<vertices.size(); i++){
        out_file << "v ";
        out_file << vertices[i].position.x() << " "
            << vertices[i].position.y()*yx_ration << " "
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

    // blend origin and dist_field
    GenFinalBlendImage();

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::ChangeContraValue(float contra_value)
{
    this->contra_value_ = contra_value;
    DenoiseImage(contra_value, this->morph_mode_);
    // blur
    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    // blend origin and dist_field
    GenFinalBlendImage();

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

    // blend origin and dist_field
    GenFinalBlendImage();

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::DilateAndErode()
{

    DenoiseImage(this->contra_value_, false);

    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);

    // blend origin and dist_field
    GenFinalBlendImage();

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::DilateAndErode2()
{
    std::cout << "dilate2" << std::endl;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(morph_kernel_size_, morph_kernel_size_));

    cv::dilate(temp, temp, kernel);
    cv::erode(temp, temp, kernel);
    cv::imshow("dilate2", temp);
    //BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);cv::addWeighted(contra_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    //cv::addWeighted(contra_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);

    // use final blend image to generate z
    //GenMeshData();
}

void GridMesh::ChangeGKernelSize(int kernel_size)
{
    this->gaussian_kernel_size_ = kernel_size;
    BlurImage(kernel_size, this->gaussian_sigma_);

    // blend origin and dist_field
    GenFinalBlendImage();

    // use final blend image to generate z
    GenMeshData();

}

void GridMesh::ChangeGSigma(float sigma)
{
    this->gaussian_sigma_ = sigma;
    BlurImage(this->gaussian_kernel_size_, sigma);

    // blend origin and dist_field
    GenFinalBlendImage();

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::ChangeDistRange(int r)
{
    this->dist_normal_range_ = r;

    BlurImage(this->gaussian_kernel_size_, this->gaussian_sigma_);
    GenFinalBlendImage();
    GenMeshData();
}

void GridMesh::ChangeBlend_a(float a)
{
    blend_factor_a_ = a;

    // blend origin and dist_field
    GenFinalBlendImage();

    // use final blend image to generate z
    GenMeshData();
}

void GridMesh::GenFinalBlendImage()
{
    cv::Mat FullMat(origin_.rows, origin_.cols, CV_8UC1, cv::Scalar(255));
    cv::Mat dist_field_8U_image_(origin_.rows, origin_.cols, CV_8UC1, cv::Scalar(255));
    dist_field_image_.convertTo(dist_field_8U_image_, CV_8U, 255);
    dist_field_8U_image_ = FullMat - dist_field_8U_image_;
    cv::addWeighted(origin_, blend_factor_a_, dist_field_8U_image_, (1-blend_factor_a_), 0, final_blend_);
}

void GridMesh::ChangeZMapMode(int mode)
{
    this->zmap_mode_ = mode;
    GenMeshData();
}

void GridMesh::ChangeBlend_b(float b)
{
    blend_factor_b_ = b;

    // blend denoise and blur_img
    //cv::addWeighted(contra_image_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    cv::addWeighted(origin_, blend_factor_a_, blur_image_, (1-blend_factor_a_), 0, final_blend_);
    //cv::imshow("final", final_blend_);

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
    zmap_mode_ = DEFAULT_ZMAPMODE;
    blend_factor_b_ = 0.3;
    thickness_ = DEFAULT_THICKNESS;
    dist_normal_range_ = DEFAULT_DIST_NORMAL_RANGE;
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

            // 左上
            if(row==0&&col==0)
                vertices[vertex_id].normal = QVector3D(-1, 1, 1);
            // 右上
            else if(row==0&&col==density_x_-1)
                vertices[vertex_id].normal = QVector3D(1, 1, 1);
            // 坐下
            else if(row==density_y_-1 && col==0)
                vertices[vertex_id].normal = QVector3D(-1, -1, 1);
            // 右下
            else if(row==density_-1 && col==density_x_-1)
                vertices[vertex_id].normal = QVector3D(1, -1, 1);
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

    // back-face
    for(int row=0; row<density_y_; row++)
    {
        for(int col=0; col<density_x_; col++)
        {
            int vertex_id = row * density_x_ + col + density_x_*density_y_;
            QVector3D front_vert = vertices[vertex_id - density_x_*density_y_].position;
            vertices[vertex_id].normal = QVector3D(front_vert.x(), front_vert.y(), front_vert.z()*-1);
        }
    }
}

void GridMesh::EstimateVertexNormal2()
{
    // front-face
    for(int row=0; row<density_y_; row++)
    {
        for(int col=0; col<density_x_; col++)
        {
            int vertex_id = row * density_x_ + col;
            Vertex v = vertices[vertex_id];
        }
    }
}

void GridMesh::ShowInterImage(InterImageType inter)
{
    cv::destroyAllWindows();

    switch(inter)
    {
    case CONTRA_IMAGE:
        // 显示对比度图片
        cv::imshow("adjust contrast", contra_image_);
        break;
    case DENOISE_IMAGE:
        // 显示降噪后的图片
        cv::imshow("denoise", denoise_image_);
        break;
    case BLUR_IMAGE:
        // 显示降噪并高斯模糊后的图片
        cv::imshow("gaussian blur", blur_image_);
        break;
    case FINAL_BLEND_IAMGE:
        // 显示最终混合的图片
        cv::imshow("final blending", final_blend_);
        break;
    default:
        cv::imshow("final blending", final_blend_);
        break;
    }
}

void GridMesh::ChangeThickness(float t)
{
    thickness_ = t;
//    int vert_num = vertices.size();
//    for(int i=0;i<4;i++)
//    {
//        vertices[vert_num-1-i].position.setZ(-1.0*thickness_);
//    }
//    vertices[0].position.setZ(-1 * thickness_);
//    vertices[density_x_-1].position.setZ(-1*thickness_);
//    vertices[density_x_ * density_y_-1].position.setZ(-1*thickness_);
//    vertices[(density_y_-1)*density_x_].position.setZ(-1*thickness_);
    GenMeshData();
    std::cout << "change thickness.\n";
}

void GridMesh::ChangeDensity(int dx)
{
    density_x_ = dx;
    density_y_ = dx * (1.0 * origin_.rows/origin_.cols);
    grid_width_ = 1.0f * (right_up_corner_.x() - left_bottom_corner_.x()) / (density_x_ - 1);
    grid_height_ = 1.0f * (right_up_corner_.y() - left_bottom_corner_.y()) / (density_y_ - 1);

    indices.clear();
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
    indices.push_back(0);  // 左上
    indices.push_back(density_x_-1);  // 右上
    indices.push_back(density_x_ * density_y_-1); // 右下
    indices.push_back(0);
    indices.push_back(density_x_ * density_y_-1);
    indices.push_back((density_y_-1)*density_x_); // 左下

    GenMeshData();
}
