#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "log.h"

class QString;
class NBodyWidget;
class OCLSettingsDialog;
class QFile;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool postInit();

public slots:
    void addlog(Log::MsgType msg_type, const QString& who, const QString& msg);

private slots:
    void on_actExit_triggered();
    void on_actSettingsOCL_triggered();
    void on_actSimStart_triggered();
    void on_actSimStop_triggered();
    void on_actGenSGalaxy_triggered();

private:
    void refreshUi();

    QFile* logFile;

    Ui::MainWindow *ui;

    NBodyWidget* nbodyWidget;

    OCLSettingsDialog* oclSettingsDlg;
};

#endif // MAINWINDOW_H
