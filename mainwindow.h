#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "log.h"

class QString;
class GalaxyViewWidget;
class OCLSettingsDialog;

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
    void addlog(Log::MsgType type_, const QString& who_, const QString& msg_);

private slots:
    void on_twMain_tabCloseRequested(int index);
    void on_actExit_triggered();
    void on_actSettingsOCL_triggered();
    void on_actSimNew_triggered();
    void on_actSimEnd_triggered();

private:
    void _refreshUi();

    Ui::MainWindow *ui;

    OCLSettingsDialog* _oclSettingsDlg;
};

#endif // MAINWINDOW_H
