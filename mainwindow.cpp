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
    //ui->spinBoxMinutes->setValue(10);

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTimerTick);
    connect(&m_countdownTimer, &QTimer::timeout, this, &MainWindow::onCountdownTick); // Zähler Statuszeile
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
        ui->pushButtonStartStop->setEnabled(true);
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
    m_countdownTimer.stop();
    ui->statusBar->clearMessage();
    m_selectedWindow = nullptr;
    ui->lineEditWindowTitle->clear();
    ui->lineEditWindowTitle->setEnabled(true);
    ui->pushButtonStartStop->setText("Start");
    ui->pushButtonStartStop->setEnabled(false);
}

// Startet oder stoppt die Schleife (Toggle)
void MainWindow::on_pushButtonStartStop_clicked()
{
    if (m_timer.isActive()) {
        m_timer.stop();
        m_countdownTimer.stop();
        ui->statusBar->clearMessage();
        ui->pushButtonStartStop->setText("Start");
        ui->lineEditWindowTitle->setEnabled(true);
    } else {
        if (!m_selectedWindow) {
            QString title = ui->lineEditWindowTitle->text().trimmed();
            if (title.isEmpty()) return;

            auto windows = WindowManager::getOpenWindows();
            for (auto it = windows.cbegin(); it != windows.cend(); ++it) {
                if (it.key().contains(title, Qt::CaseInsensitive)) {
                    m_selectedWindow = it.value();
                    break;
                }
            }

            if (!m_selectedWindow) {
                QMessageBox::warning(this, "Window not found",
                                     QString("No open window with title:\n\"%1\"").arg(title));
                return;
            }
        }

        int ms = ui->spinBoxMinutes->value() * 60 * 1000;
        m_remainingSeconds = ms / 1000;
        m_timer.start(ms);
        m_countdownTimer.start(1000);
        // onTimerTick(); // sofortiges Öffnen nach Start
        ui->pushButtonStartStop->setText("Stop");
        ui->lineEditWindowTitle->setEnabled(false);
    }
}

// Wird vom Timer aufgerufen – holt das Fenster in den Vordergrund
void MainWindow::onTimerTick()
{
    if (m_selectedWindow)
        WindowManager::bringToFront(m_selectedWindow);

    m_remainingSeconds = ui->spinBoxMinutes->value() * 60;
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


void MainWindow::on_lineEditWindowTitle_textEdited(const QString &arg1)
{
    m_selectedWindow = nullptr;
    ui->pushButtonStartStop->setEnabled(!arg1.trimmed().isEmpty());
}


void MainWindow::onCountdownTick()
{
    --m_remainingSeconds;
    int m = m_remainingSeconds / 60;
    int s = m_remainingSeconds % 60;
    ui->statusBar->showMessage(QString("Next: %1:%2")
                                   .arg(m, 2, 10, QChar('0'))
                                   .arg(s, 2, 10, QChar('0')));
}
