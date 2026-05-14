#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Startwert SpinBox: 1 Minute, Minimum 1
    ui->spinBoxMinutes->setMinimum(1);
    ui->spinBoxMinutes->setValue(1);

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Öffnet eine Auswahlliste der aktuell offenen Fenster
void MainWindow::on_pushButtonSearchWindow_clicked()
{
    auto windows = WindowManager::getOpenWindows();
    if (windows.isEmpty()) {
        return;
    }

    QStringList titles = windows.keys();
    bool ok;
    QString selected = QInputDialog::getItem(
        this, "Choose window", "Opened Window:", titles, 0, false, &ok);

    if (ok && !selected.isEmpty()) {
        m_selectedWindow = windows[selected];
        ui->lineEditWindowTitle->setText(selected);
    }
}

// Stellt den Intervall in Minuten ein; läuft der Timer bereits, wird er aktualisiert
void MainWindow::on_spinBoxMinutes_valueChanged(int value)
{
    if (m_timer.isActive()) {
        m_timer.setInterval(value * 60 * 1000);
    }
}

// Löscht den gewählten Fenstertitel und stoppt den Timer
void MainWindow::on_pushButtonClear_clicked()
{
    m_timer.stop();
    m_selectedWindow = nullptr;
    ui->lineEditWindowTitle->clear();
    ui->pushButtonStartStop->setText("Start");
}

// Startet oder stoppt die Schleife (Toggle)
void MainWindow::on_pushButtonStartStop_clicked()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        ui->pushButtonStartStop->setText("Start");
    } else {
        if (!m_selectedWindow) return; // kein Fenster gewählt
        int ms = ui->spinBoxMinutes->value() * 60 * 1000;
        m_timer.start(ms);
        ui->pushButtonStartStop->setText("Stop");
    }
}

// Wird vom Timer aufgerufen – holt das Fenster in den Vordergrund
void MainWindow::onTimerTick()
{
    if (m_selectedWindow)
        WindowManager::bringToFront(m_selectedWindow);
}
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox about(this);
    about.setWindowTitle("About Keleust");
    about.setTextFormat(Qt::RichText);
    about.setTextInteractionFlags(Qt::TextBrowserInteraction);
    about.setText(
        "<b>Keleust</b> v" PRG_VERSION "<br><br>"
        "Select a window, and Keleust will periodically bring it to the foreground.<br><br>"
        "<a href='https://github.com/Alsweider/Keleust'>"
        "github.com/Alsweider/Keleust</a>"
        );
    about.setIcon(QMessageBox::Information);
    about.exec();
}

