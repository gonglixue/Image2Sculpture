#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    btnGroup_renderMode  = new QButtonGroup(this);
    btnGroup_zmapMode = new QButtonGroup(this);

    ui->setupUi(this);
    connect(ui->actionOpen_Image, SIGNAL(triggered(bool)), this, SLOT(LoadImageAction()));
    connect(ui->actionSave_Mesh, SIGNAL(triggered(bool)), this, SLOT(SaveMeshAction()));
    connect(ui->checkBox_Reverse, SIGNAL(stateChanged(int)), this, SLOT(OnChangeCheckboxReverse()));
    connect(ui->horizontalSlider_Contra, SIGNAL(valueChanged(int)), this, SLOT(OnChangeContraSlider(int)));
    connect(ui->horizontalSlider_MorphKernel, SIGNAL(valueChanged(int)), this, SLOT(OnChangeMorphKernelSlider(int)));
    connect(ui->horizontalSlider_GKernel, SIGNAL(valueChanged(int)), this, SLOT(OnChangeGKernSlider(int)));
    connect(ui->horizontalSlider_GSigma, SIGNAL(valueChanged(int)), this, SLOT(OnChangeSigmaSlider(int)));
    connect(ui->horizontalSlider_Blenda, SIGNAL(valueChanged(int)), this, SLOT(OnChangeBlendA(int)));

    connect(ui->pushButton_DilaEro, SIGNAL(clicked(bool)), ui->openGLWidget, SLOT(Morph_DilaEro()));
    connect(ui->pushButton_EroDila, SIGNAL(clicked(bool)), ui->openGLWidget, SLOT(Morph_EroDila()));

    connect(ui->horizontalSlider_Zfactor, SIGNAL(valueChanged(int)), this, SLOT(OnChangeZFactorSlider(int)));

    btnGroup_renderMode->addButton(ui->radioButton_RenderTexture, 0);
    btnGroup_renderMode->addButton(ui->radioButton_RenderSmooth, 1);
    ui->radioButton_RenderSmooth->setChecked(true);
    connect(ui->radioButton_RenderSmooth, SIGNAL(clicked(bool)), this, SLOT(OnChangeRenderMode()));
    connect(ui->radioButton_RenderTexture,SIGNAL(clicked(bool)), this, SLOT(OnChangeRenderMode()));

    connect(ui->openGLWidget, SIGNAL(GLWidgetDensityChanged(int,int)), this, SLOT(OnDensityChanged(int,int)));

    btnGroup_zmapMode->addButton(ui->radioButton_Zmap1, 1);
    btnGroup_zmapMode->addButton(ui->radioButton_Zmap2, 2);
    btnGroup_zmapMode->addButton(ui->radioButton_Zmap3, 3);
    ui->radioButton_Zmap1->setChecked(true);
    connect(ui->radioButton_Zmap1, SIGNAL(clicked(bool)), this, SLOT(OnChangeZMapMode()));
    connect(ui->radioButton_Zmap2, SIGNAL(clicked(bool)), this, SLOT(OnChangeZMapMode()));
    connect(ui->radioButton_Zmap3, SIGNAL(clicked(bool)), this, SLOT(OnChangeZMapMode()));

    connect(ui->doubleSpinBox_Contra, SIGNAL(valueChanged(double)), this, SLOT(OnChangeContraBox(double)));
    connect(ui->spinBox_MorphKernel, SIGNAL(valueChanged(int)), this, SLOT(OnChangeMorphKernelBox(int)));
    connect(ui->spinBox_GKernel, SIGNAL(valueChanged(int)), this, SLOT(OnChangeGKernelBox(int)));
    connect(ui->doubleSpinBox_GSigma, SIGNAL(valueChanged(double)), this, SLOT(OnChangeSigmaBox(double)));
    connect(ui->doubleSpinBox_Blenda, SIGNAL(valueChanged(double)), this, SLOT(OnChangeBlendBox(double)));
    connect(ui->doubleSpinBox_Zfactor, SIGNAL(valueChanged(double)), this, SLOT(OnChangeZFactorBox(double)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadImageAction()
{
    qDebug() << "load image action slot in mainwindow\n";
    // Open File
    QString image_fn = QFileDialog::getOpenFileName(
                this,
                tr("Open An Image"),
                QString(),
                tr("Image Files(*.bmp, *.jpg)")
                );
    if(!image_fn.isEmpty()){
        ui->openGLWidget->SetTexture(image_fn);
        // reset ui
        ResetUI();
    }
}

void MainWindow::SaveMeshAction()
{
    if(CheckEmpty())
        return;
    QString mesh_fn = QFileDialog::getSaveFileName(
                this,
                tr("Save Mesh"),
                QString(),
                tr("Mesh File(*.obj)")
                );
    if(!mesh_fn.isEmpty()){
        ui->openGLWidget->SaveMesh(mesh_fn);
        QMessageBox::information(this,
                             tr("Image2Scuplture"),
                             tr("Success to save mesh.\n"),
                             QMessageBox::Ok);
    }
}

void MainWindow::OnChangeCheckboxReverse()
{
    if(CheckEmpty())
        return;

    ui->openGLWidget->Reverse();
}

void MainWindow::OnChangeContraSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    float contra_value = slider_value / 10.0f;
    ui->openGLWidget->ChangeContraValue(contra_value);
    ui->doubleSpinBox_Contra->setValue(contra_value);
}

void MainWindow::OnChangeContraBox(double value)
{
    if(CheckEmpty())
        return;
    ui->openGLWidget->ChangeContraValue(value);
    ui->horizontalSlider_Contra->setValue(value * 10);
}

void MainWindow::OnChangeMorphKernelSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    ui->spinBox_MorphKernel->setValue(2 * slider_value + 1);
    ui->openGLWidget->ChangeMorphKernelSize(2 * slider_value + 1);
}

void MainWindow::OnChangeMorphKernelBox(int value)
{
    if(CheckEmpty())
        return;
    ui->openGLWidget->ChangeMorphKernelSize(value);
    ui->horizontalSlider_MorphKernel->setValue((value-1)/2);
}

void MainWindow::OnChangeGKernSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    ui->spinBox_GKernel->setValue(2 * slider_value + 1);
    ui->openGLWidget->ChangeGKernelSize(2 * slider_value + 1);
}

void MainWindow::OnChangeGKernelBox(int value)
{
    if(CheckEmpty())
        return;
    ui->openGLWidget->ChangeGKernelSize(value);
    ui->horizontalSlider_GKernel->setValue((value-1)/2);
}

void MainWindow::OnChangeSigmaSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    float sigma = slider_value / 10.0f;
    ui->openGLWidget->ChangeGSigma(sigma);
    ui->doubleSpinBox_GSigma->setValue(sigma);
}

void MainWindow::OnChangeSigmaBox(double value)
{
    if(CheckEmpty())
        return;
    ui->openGLWidget->ChangeGSigma(value);
    ui->horizontalSlider_GSigma->setValue(value * 10);
}

void MainWindow::OnChangeBlendA(int slider_value)
{
    if(CheckEmpty())
        return;
    float a = slider_value / 10.0f;
    ui->openGLWidget->ChangeBlendA(a);
    ui->doubleSpinBox_Blenda->setValue(a);
}

void MainWindow::OnChangeBlendBox(double value)
{
    if(CheckEmpty())
        return;
    ui->openGLWidget->ChangeBlendA(value);
    ui->horizontalSlider_Blenda->setValue(value * 10);
}

void MainWindow::OnChangeBlendB(int slider_value)
{
    if(CheckEmpty())
        return;
    float b = slider_value / 10.0f;
    ui->openGLWidget->ChangeBlendB(b);
}

void MainWindow::OnChangeZFactorSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    float factor = slider_value / 1000.0;
    ui->openGLWidget->ChangeZFactor(factor);
    ui->doubleSpinBox_Zfactor->setValue(factor);

}

void MainWindow::OnChangeZFactorBox(double value)
{
    if(CheckEmpty())
        return;
    ui->openGLWidget->ChangeZFactor(value);
    ui->horizontalSlider_Zfactor->setValue(value * 1000);
}

void MainWindow::OnChangeRenderMode()
{
    ui->openGLWidget->ChangeRenderMode(btnGroup_renderMode->checkedId());
}

void MainWindow::OnChangeZMapMode()
{
    ui->openGLWidget->ChangeZMapMode(btnGroup_zmapMode->checkedId());
}

void MainWindow::OnDensityChanged(int density_x, int density_y)
{
    char density_str[100];
    sprintf(density_str, "%d x %d", density_x, density_y);
    QString density_info(density_str);
    ui->lineEdit_Density->setText(density_info);
    ui->horizontalSlider_Density->setValue(density_x);
}

bool MainWindow::CheckEmpty()
{
    if(ui->openGLWidget->Empty())
    {
        QMessageBox::warning(this,
                             tr("Image2Scuplture"),
                             tr("Cannot save empty mesh.\n"
                                "Please load a new image to generate mesh."),
                             QMessageBox::Ok);

        return true;
    }
    return false;
}

void MainWindow::ResetUI()
{
    ui->checkBox_Reverse->setCheckState(Qt::Unchecked);
    ui->horizontalSlider_Contra->setValue(15);
    ui->horizontalSlider_MorphKernel->setValue(1);
    //ui->horizontalSlider_Median->setValue(2);
    ui->horizontalSlider_GKernel->setValue(1);
    ui->horizontalSlider_GSigma->setValue(50);
    ui->horizontalSlider_Blenda->setValue(6);

    ui->horizontalSlider_Zfactor->setValue(60);
    ui->radioButton_RenderSmooth->setChecked(true);
    ui->radioButton_RenderTexture->setChecked(false);

    ui->radioButton_Zmap1->setChecked(true);
    ui->radioButton_Zmap2->setChecked(false);
    ui->radioButton_Zmap3->setChecked(false);
}
