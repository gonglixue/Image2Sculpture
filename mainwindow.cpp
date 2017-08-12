#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen_Image, SIGNAL(triggered(bool)), this, SLOT(LoadImageAction()));
    connect(ui->actionSave_Mesh, SIGNAL(triggered(bool)), this, SLOT(SaveMeshAction()));
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
    }
}

void MainWindow::SaveMeshAction()
{
    if(ui->openGLWidget->Empty())
    {
        QMessageBox::warning(this,
                             tr("Image2Scuplture"),
                             tr("Cannot save empty mesh.\n"
                                "Please load a new image to generate mesh."),
                             QMessageBox::Ok);

        return;
    }
    QString mesh_fn = QFileDialog::getSaveFileName(
                this,
                tr("Save Mesh"),
                QString(),
                tr("Mesh File(*.obj)")
                );
    if(!mesh_fn.isEmpty())
        ui->openGLWidget->SaveMesh(mesh_fn);
}
