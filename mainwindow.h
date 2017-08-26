#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QButtonGroup>
#include <QLabel>

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

    void OnChangeContraBox(double value);
    void OnChangeMorphKernelBox(int value);
    void OnChangeGKernelBox(int value);
    void OnChangeSigmaBox(double value);
    void OnChangeBlendBox(double value);
    void OnChangeZFactorBox(double value);
    void OnChangeThicknessSlider(int slider_value);
    void OnChangeThicknessBox(double value);
private:
    Ui::MainWindow *ui;
    bool CheckEmpty();
    void ResetUI();
    QButtonGroup *btnGroup_renderMode;
    QButtonGroup *btnGroup_zmapMode;
    QLabel* HintLabel;
};

#endif // MAINWINDOW_H
