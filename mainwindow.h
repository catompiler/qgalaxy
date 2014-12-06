#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "log.h"
#include <QString>

class NBodyWidget;
class OCLSettingsDialog;
class EditBodyDialog;
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

public slots:
    void addlog(Log::MsgType msg_type, const QString& who, const QString& msg);

private slots:
    void nbodyWidget_onInitialized();
    void nbodyWidget_onSimulated();

    void on_actExit_triggered();
    void on_actSettingsOCL_triggered();
    void on_actSimStart_triggered();
    void on_actSimStop_triggered();
    void on_actGenSGalaxy_triggered();
    void on_actGenGalaxyCollision_triggered();
    void on_actSimEdit_triggered();
    void on_actSaveFile_triggered();
    void on_actOpenFile_triggered();
    void on_actSimReset_triggered();
private:
    void refreshUi();
    void resetSimData();

    qreal simulated_years;

    QString cur_dir;

    QFile* logFile;

    Ui::MainWindow *ui;

    NBodyWidget* nbodyWidget;

    OCLSettingsDialog* oclSettingsDlg;

    EditBodyDialog* editBodyDlg;
};

#endif // MAINWINDOW_H
