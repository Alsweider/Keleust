#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "windowmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pushButtonSearchWindow_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonStartStop_clicked();
    void on_spinBoxMinutes_valueChanged(int value);
    void onTimerTick();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow*  ui;
    QTimer           m_timer;
    NativeWindow     m_selectedWindow = nullptr;
};

#endif // MAINWINDOW_H