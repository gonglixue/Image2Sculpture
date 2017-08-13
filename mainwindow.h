#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void LoadImageAction();
    void SaveMeshAction();
    void OnChangeCheckboxReverse();
    void OnChangeContraSlider(int slider_value);
    void OnChangeMorphKernelSlider(int slider_value);
    void OnChangeGKernSlider(int slider_value);
    void OnChangeSigmaSlider(int slider_value);

private:
    Ui::MainWindow *ui;
    bool CheckEmpty();
};

#endif // MAINWINDOW_H
