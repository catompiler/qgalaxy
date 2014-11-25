#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>
#include <QTextCursor>
#include <QFile>
#include <QDebug>
#include "clplatform.h"
#include "cldevice.h"
#include "log.h"
#include "settings.h"
#include "oclsettingsdialog.h"
#include "nbodywidget.h"
#include "spiralgalaxy.h"



#define LOG_WHO "Main"

static const char* log_file_name = "log.txt";


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    logFile = new QFile(log_file_name);
    logFile->open(QIODevice::WriteOnly);

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

    logFile->close();
    delete logFile;
}

void MainWindow::addlog(Log::MsgType msg_type, const QString& who, const QString &msg)
{
    #define MSG_TYPES_COUNT 5
    static const char* msg_types_html[MSG_TYPES_COUNT]={
        "",
        "(<font color=\"blue\">DD</font>)",
        "(<font color=\"green\">II</font>)",
        "(<font color=\"yellow\">WW</font>)",
        "(<font color=\"red\">EE</font>)"
    };
    static const char* msg_types_text[MSG_TYPES_COUNT]={
        "",
        "(DD)",
        "(II)",
        "(WW)",
        "(EE)"
    };

    if(msg_type >= MSG_TYPES_COUNT){
        msg_type = Log::NONE;
    }

    QDateTime curdtime = QDateTime::currentDateTime();
    QString strdtime = curdtime.toString("hh:mm:ss.zzz");//dd.MM.yyyy

    QString loghtml = QString("[%1]%2<b>%3:</b> %4")
                        .arg(strdtime)
                        .arg(tr(msg_types_html[msg_type]))
                        .arg(who)
                        .arg(msg);

    ui->teLog->append(loghtml);
    ui->teLog->textCursor().movePosition(QTextCursor::End,
                                         QTextCursor::MoveAnchor, 1);
    if(logFile->isOpen()){

        QString logtext = QString("[%1]%2%3: %4\n")
                            .arg(strdtime)
                            .arg(tr(msg_types_text[msg_type]))
                            .arg(who)
                            .arg(msg);

        logFile->write(logtext.toUtf8());
        logFile->flush();
    }

    if(msg_type == Log::ERROR){
        QMessageBox::critical(this, who, msg);
    }
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
        //QMessageBox::critical(this, tr("Ошибка!"), e.what());
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
            //QMessageBox::critical(this, tr("Ошибка!"), e.what());
        }
    }
}

void MainWindow::on_actSimStart_triggered()
{
    nbodyWidget->setSimulationRunning(true);
}

void MainWindow::on_actSimStop_triggered()
{
    nbodyWidget->setSimulationRunning(false);
}

void MainWindow::on_actGenSGalaxy_triggered()
{
    SpiralGalaxy galaxy;

    galaxy.setStarsCount(Settings::get().bodiesCount());
    galaxy.setRadius(2000.0);
    galaxy.setMinStarMass(1e-1);
    galaxy.setMaxStarMass(2e-1);
    galaxy.setBlackHoleMass(8e5);//8e5

    if(galaxy.generate()){
        nbodyWidget->setBodies(0, galaxy.starsMasses(), galaxy.starsPositons(), galaxy.starsVelosities());
    }
}

void MainWindow::refreshUi()
{
}
