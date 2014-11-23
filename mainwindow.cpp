#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>
#include <QTextCursor>
#include <QDebug>
#include "clplatform.h"
#include "cldevice.h"
#include "log.h"
#include "settings.h"
#include "oclsettingsdialog.h"
#include "nbodywidget.h"




#define LOG_WHO "Main"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&Log::instance(), SIGNAL(log(Log::MsgType,QString,QString)),
            this, SLOT(addlog(Log::MsgType,QString,QString)));

    nbodyWidget = new NBodyWidget(this);
    setCentralWidget(nbodyWidget);

    oclSettingsDlg = nullptr;

    refreshUi();
}

MainWindow::~MainWindow()
{
    delete oclSettingsDlg;
    delete nbodyWidget;
    delete ui;
}

void MainWindow::addlog(Log::MsgType type_, const QString& who_, const QString &msg_)
{
    static const char* msg_types_str[]={
        "",
        "(<font color=\"blue\">DD</font>)",
        "(<font color=\"green\">II</font>)",
        "(<font color=\"yellow\">WW</font>)",
        "(<font color=\"red\">EE</font>)"
    };

    if(type_ >= (sizeof(msg_types_str) / sizeof(msg_types_str[0]))){
        type_ = Log::NONE;
    }

    QDateTime curdtime = QDateTime::currentDateTime();
    QString strdtime = curdtime.toString("hh:mm:ss.zzz");//dd.MM.yyyy

    ui->teLog->append(QString("[%1]%2<b>%3:</b> %4")
                                .arg(strdtime)
                                .arg(tr(msg_types_str[type_]))
                                .arg(who_)
                                .arg(msg_));
    ui->teLog->textCursor().movePosition(QTextCursor::End,
                                         QTextCursor::MoveAnchor, 1);
}

void MainWindow::on_actExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actSettingsOCL_triggered()
{
    if(!oclSettingsDlg) oclSettingsDlg = new OCLSettingsDialog(this);

    try{

        CLPlatform platform = CLPlatform::byName(Settings::get().clPlatformName());
        oclSettingsDlg->setCurrentPlatform(platform);

        CLDevice device = platform.deviceByName(Settings::get().clDeviceName());
        oclSettingsDlg->setCurrentDevice(device);

    }catch(CLException& e){
        log(Log::ERROR, LOG_WHO, e.what());
        QMessageBox::critical(this, tr("Ошибка!"), e.what());
    }

    oclSettingsDlg->setBodiesCount(Settings::get().bodiesCount());

    if(oclSettingsDlg->exec() == QDialog::Accepted){

        try{

            Settings::get().setClPlatformName(oclSettingsDlg->currentPlatform().name());
            Settings::get().setClDeviceName(oclSettingsDlg->currentDevice().name());
            Settings::get().setBodiesCount(oclSettingsDlg->bodiesCount());

            nbodyWidget->recreateNBody();

        }catch(CLException& e){
            log(Log::ERROR, LOG_WHO, e.what());
            QMessageBox::critical(this, tr("Ошибка!"), e.what());
        }
    }
}

void MainWindow::on_actSimStart_triggered()
{
}

void MainWindow::on_actSimStop_triggered()
{
}

void MainWindow::refreshUi()
{
}
