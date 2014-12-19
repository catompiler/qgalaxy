#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>
#include <QTextCursor>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>
#include "clplatform.h"
#include "cldevice.h"
#include "log.h"
#include "settings.h"
#include "oclsettingsdialog.h"
#include "editbodydialog.h"
#include "nbodywidget.h"
#include "spiralgalaxy.h"
//#include <QDebug>



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

    connect(nbodyWidget, SIGNAL(nbodyStatusChanged()),
            this, SLOT(nbodyWidget_onStatusChanged()));
    connect(nbodyWidget, SIGNAL(simulationFinished()),
            this, SLOT(nbodyWidget_onSimulated()));

    fpsTimer = new QTimer(this);
    connect(fpsTimer, SIGNAL(timeout()),
            this, SLOT(fpsTimer_onTimeout()));

    oclSettingsDlg = nullptr;

    editBodyDlg = nullptr;

    cur_dir = QDir::currentPath();

    simulated_years = 0.0;

    cur_fps = 0;
    cur_frames = 0;

    refreshUi();
}

MainWindow::~MainWindow()
{
    fpsTimer->stop();
    delete fpsTimer;

    delete editBodyDlg;
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

void MainWindow::nbodyWidget_onStatusChanged()
{
    refreshUi();
}

void MainWindow::nbodyWidget_onSimulated()
{
    cur_frames ++;

    simulated_years += nbodyWidget->timeStep();

    QString units;
    qreal years = simulated_years;

    if(years >= 1e12){
        years /= 1e12;
        units = tr("трлн. лет.");
    }else if(years >= 1e9){
        years /= 1e9;
        units = tr("млрд. лет.");
    }else if(years >= 1e6){
        years /= 1e6;
        units = tr("млн. лет.");
    }else if(years >= 1e3){
        years /= 1e3;
        units = tr("тыс. лет.");
    }else{
        units = tr("лет.");
    }

    statusBar()->showMessage(tr("%1 %2 FPS: %3").arg(years, 0, 'f', 6).arg(units).arg(cur_fps));
}

void MainWindow::fpsTimer_onTimeout()
{
    cur_fps = cur_frames;
    cur_frames = 0;
}

void MainWindow::on_actExit_triggered()
{
    nbodyWidget->setSimulationRunning(false);
    qApp->quit();
}

void MainWindow::on_actSettingsOCL_triggered()
{
    if(!oclSettingsDlg) oclSettingsDlg = new OCLSettingsDialog(this);

    try{

        CLPlatform platform = CLPlatform::byName(Settings::get().clPlatformName());
        if(platform.isValid()){

            oclSettingsDlg->setCurrentPlatform(platform);
            CLDevice device = platform.deviceByName(Settings::get().clDeviceName());

            if(device.isValid()){
                oclSettingsDlg->setCurrentDevice(device);
            }
        }

    }catch(CLException& e){
        //log(Log::ERROR, LOG_WHO, e.what());
        //QMessageBox::critical(this, tr("Ошибка!"), e.what());
    }

    oclSettingsDlg->setBodiesCount(Settings::get().bodiesCount());
    oclSettingsDlg->setTimeStep(Settings::get().timeStep());

    if(oclSettingsDlg->exec() == QDialog::Accepted){

        try{

            Settings::get().setClPlatformName(oclSettingsDlg->currentPlatform().name());
            Settings::get().setClDeviceName(oclSettingsDlg->currentDevice().name());
            Settings::get().setBodiesCount(oclSettingsDlg->bodiesCount());
            Settings::get().setTimeStep(oclSettingsDlg->timeStep());

            nbodyWidget->recreateNBody();

        }catch(CLException& e){
            log(Log::ERROR, LOG_WHO, e.what());
            //QMessageBox::critical(this, tr("Ошибка!"), e.what());
        }
    }
    refreshUi();
}

void MainWindow::on_actSimStart_triggered()
{
    nbodyWidget->setSimulationRunning(true);
    refreshUi();
    if(nbodyWidget->isSimulationRunning()) fpsTimer->start(1000);
}

void MainWindow::on_actSimStop_triggered()
{
    nbodyWidget->setSimulationRunning(false);
    refreshUi();
    fpsTimer->stop();
}

void MainWindow::on_actGenSGalaxy_triggered()
{
    SpiralGalaxy galaxy;

    galaxy.setStarsCount(Settings::get().bodiesCount());
    galaxy.setRadius(2000.0);
    galaxy.setMinStarMass(utils::getRandf(1e-1, 1e0));//5e-1
    galaxy.setMaxStarMass(utils::getRandf(1e0, 1e1));//2e0
    galaxy.setBlackHoleMass(utils::getRandf(1e6, 1e7));//1e7

    if(!galaxy.generate() || !nbodyWidget->setBodies(0, galaxy.starsMasses(), galaxy.starsPositons(), galaxy.starsVelosities())){
        log(Log::ERROR, LOG_WHO, tr("Ошибка генерации галактики!"));
        return;
    }
    nbodyWidget->setSimulatedBodiesCount(galaxy.starsCount());

    resetSimData();
}

void MainWindow::on_actGenGalaxyCollision_triggered()
{
    static size_t galaxies_count = 0;

    size_t all_stars_count = Settings::get().bodiesCount();
    size_t max_galaxies = all_stars_count / 2;

    bool ok = false;
    galaxies_count = QInputDialog::getInt(this, tr("Выбор."), tr("Выберите число галактик:"), galaxies_count, 1, max_galaxies, 1, &ok);

    if(!ok) return;

    SpiralGalaxy galaxy;

    size_t stars_per_galaxy = all_stars_count / galaxies_count;

    const qreal max_axis_pos = 1000.0 * galaxies_count;
    const qreal max_axis_vel = 1e-5;

    for(size_t i = 0; i < galaxies_count; i ++){
        if(i == galaxies_count - 1){
            galaxy.setStarsCount(all_stars_count - stars_per_galaxy * i);
        }else{
            galaxy.setStarsCount(stars_per_galaxy);
        }
        galaxy.setRadius(utils::getRandf(1000.0, 2000.0));
        galaxy.setMinStarMass(utils::getRandf(1e-1, 1e0));//5e-1
        galaxy.setMaxStarMass(utils::getRandf(1e0, 1e1));//2e0
        galaxy.setBlackHoleMass(utils::getRandf(1e6, 1e7));//1e7
        galaxy.setPosition(QVector3D(utils::getRandf(-max_axis_pos, max_axis_pos),
                                     utils::getRandf(-max_axis_pos, max_axis_pos),
                                     utils::getRandf(-max_axis_pos, max_axis_pos)));
        galaxy.setVelocity(QVector3D(utils::getRandf(-max_axis_vel, max_axis_vel),
                                     utils::getRandf(-max_axis_vel, max_axis_vel),
                                     utils::getRandf(-max_axis_vel, max_axis_vel)));
        galaxy.setOrientation(QQuaternion::fromAxisAndAngle(utils::getRandsf(),
                                                            utils::getRandsf(),
                                                            utils::getRandsf(),
                                                            rand() % 360));

        if(!galaxy.generate() || !nbodyWidget->setBodies(i * stars_per_galaxy, galaxy.starsMasses(), galaxy.starsPositons(), galaxy.starsVelosities())){
            log(Log::ERROR, LOG_WHO, tr("Ошибка генерации галактики!"));
            return;
        }
    }
    nbodyWidget->setSimulatedBodiesCount(all_stars_count);

    resetSimData();
}

void MainWindow::on_actSimEdit_triggered()
{
    if(editBodyDlg == nullptr)
        editBodyDlg = new EditBodyDialog(nbodyWidget, this);

    editBodyDlg->exec();
}

void MainWindow::on_actSaveFile_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранение файла"), cur_dir, tr("Galaxy files (*.glxy)"));

    if(filename.isEmpty()) return;

    cur_dir = QDir(filename).path();

    if(!nbodyWidget->saveNBody(filename)){
        log(Log::ERROR, LOG_WHO, tr("Ошибка сохранения файла!"));
    }

    resetSimData();
}

void MainWindow::on_actOpenFile_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Открытие файла"), cur_dir, tr("Galaxy files (*.glxy)"));

    if(filename.isEmpty()) return;

    cur_dir = QDir(filename).path();

    if(!nbodyWidget->openNBody(filename)){
        log(Log::ERROR, LOG_WHO, tr("Ошибка открытия файла!"));
    }

    resetSimData();
}

void MainWindow::on_actSimReset_triggered()
{
    nbodyWidget->reset();

    resetSimData();
}

void MainWindow::refreshUi()
{
    bool is_ready = nbodyWidget->isReady();
    bool is_running = nbodyWidget->isSimulationRunning();
    bool is_not_running = is_ready && !is_running;

    ui->actSettingsOCL->setEnabled(!is_running);

    ui->actSimEdit->setEnabled(is_not_running);
    ui->actSimReset->setEnabled(is_not_running);
    ui->actSimStart->setEnabled(is_not_running);
    ui->actSimStop->setEnabled(is_running);

    ui->actGenSGalaxy->setEnabled(is_not_running);
    ui->actGenGalaxyCollision->setEnabled(is_not_running);

    ui->actOpenFile->setEnabled(is_not_running);
    ui->actSaveFile->setEnabled(is_not_running);
}

void MainWindow::resetSimData()
{
    simulated_years = 0.0;
    statusBar()->clearMessage();
}
