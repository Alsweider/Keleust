#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "windowmanager.h"
#include <QSettings>
#include <QStandardPaths>


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
    void onCountdownTick(); // Zähler für Statuszeile
    void on_actionAbout_triggered();
    void on_lineEditWindowTitle_textEdited(const QString &arg1);

private:
    Ui::MainWindow*  ui;
    QTimer           m_timer;
    QTimer           m_countdownTimer;
    int              m_remainingSeconds;
    NativeWindow     m_selectedWindow = nullptr;
};

#endif // MAINWINDOW_H