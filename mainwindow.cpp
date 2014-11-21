#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>
#include <QTextCursor>
#include <QDebug>
#include "galaxyviewwidget.h"
#include "log.h"
#include "settings.h"
#include "oclsettingsdialog.h"




#define LOG_WHO "Main"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&Log::instance(), SIGNAL(log(Log::MsgType,QString,QString)),
            this, SLOT(addlog(Log::MsgType,QString,QString)));

    _oclSettingsDlg = NULL;

    _refreshUi();
}

MainWindow::~MainWindow()
{
    delete _oclSettingsDlg;
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

void MainWindow::on_twMain_tabCloseRequested(int index)
{
}

void MainWindow::on_actExit_triggered()
{
    qApp->quit();
    //_refreshUi();
}

void MainWindow::on_actSettingsOCL_triggered()
{
}

void MainWindow::on_actSimNew_triggered()
{
}

void MainWindow::on_actSimEnd_triggered()
{
}

void MainWindow::_refreshUi()
{
    ui->actSimEnd->setEnabled(ui->twMain->count() != 0);
}



/*QGLFormat MainWindow::_getGLFormat()
{
    QGLFormat res_glfmt;

    res_glfmt.setRgba(true);
    res_glfmt.setRedBufferSize(8);
    res_glfmt.setGreenBufferSize(8);
    res_glfmt.setBlueBufferSize(8);
    res_glfmt.setAlpha(true);
    res_glfmt.setAlphaBufferSize(8);

    res_glfmt.setDepth(true);
    res_glfmt.setDepthBufferSize(24);

    res_glfmt.setStencil(false);
    res_glfmt.setAccum(false);
    res_glfmt.setStereo(false);

    res_glfmt.setSampleBuffers(true);
    res_glfmt.setSamples(4);
    res_glfmt.setProfile(QGLFormat::CoreProfile);
    res_glfmt.setVersion(3, 3);

    return res_glfmt;
}*/

