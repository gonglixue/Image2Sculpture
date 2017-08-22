#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QButtonGroup>

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
    void OnChangeBlendA(int slider_value);
    void OnChangeBlendB(int slider_value);
    void OnChangeZFactorSlider(int slider_value);
    void OnChangeRenderMode();
    void OnDensityChanged(int density_x, int density_y);
    void OnChangeZMapMode();
private:
    Ui::MainWindow *ui;
    bool CheckEmpty();
    void ResetUI();
    QButtonGroup *btnGroup_renderMode;
    QButtonGroup *btnGroup_zmapMode;
};

#endif // MAINWINDOW_H
