#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

void MainWindow::OnChangeMorphKernelSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    ui->spinBox_MorphKernel->setValue(2 * slider_value + 1);
    ui->openGLWidget->ChangeMorphKernelSize(2 * slider_value + 1);
}

void MainWindow::OnChangeGKernSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    ui->spinBox_GKernel->setValue(2 * slider_value + 1);
    ui->openGLWidget->ChangeGKernelSize(2 * slider_value + 1);
}

void MainWindow::OnChangeSigmaSlider(int slider_value)
{
    if(CheckEmpty())
        return;
    float sigma = slider_value / 10.0f;
    ui->openGLWidget->ChangeGSigma(sigma);
    ui->doubleSpinBox_GSigma->setValue(sigma);
}

void MainWindow::OnChangeBlendA(int slider_value)
{
    if(CheckEmpty())
        return;
    float a = slider_value / 10.0f;
    ui->openGLWidget->ChangeBlendA(a);
    ui->doubleSpinBox_Blenda->setValue(a);
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
}
