#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "log.h"
#include <QString>

class NBodyWidget;
class OCLSettingsDialog;
class EditBodyDialog;
class GenSettingsDialog;
class QFile;
class QTimer;
class QCloseEvent;

namespace Ui {
class MainWindow;
}

/**
 * @brief Класс главного окна приложения.
 * @class MainWindow.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    /**
     * @brief Конструктор.
     * @param Объект-родитель.
     */
    explicit MainWindow(QWidget *parent = 0);

    /**
     * @brief Деструктор.
     */
    ~MainWindow();

public slots:

    /**
     * @brief Слот логирования событий.
     * @param msg_type Тип сообщения.
     * @param who Отправитель сообщения.
     * @param msg Сообщение.
     */
    void addlog(Log::MsgType msg_type, const QString& who, const QString& msg);

private slots:
    /**
     * @brief Обработчик изменения состояния системы моделирования.
     */
    void nbodyWidget_onStatusChanged();

    /**
     * @brief Обработчик окончания очередного шага моделирования.
     */
    void nbodyWidget_onSimulated();

    /**
     * @brief Обработчик события таймера FPS.
     */
    void fpsTimer_onTimeout();

    /**
     * @brief Обработчик действия выхода.
     */
    void on_actExit_triggered();

    /**
     * @brief Обработчик действия открытия настроек.
     */
    void on_actSettingsOCL_triggered();

    /**
     * @brief Обработчик действия открытия настроек генерации.
     */
    void on_actGenSettings_triggered();

    /**
     * @brief Обработчик действия запуска симуляции.
     */
    void on_actSimStart_triggered();

    /**
     * @brief Обработчик действия прекращения симуляции.
     */
    void on_actSimStop_triggered();

    /**
     * @brief Обработчик действия генерации галактики.
     */
    void on_actGenSGalaxy_triggered();

    /**
     * @brief Обработчик действия генерации столкновения галактик.
     */
    void on_actGenGalaxyCollision_triggered();

    /**
     * @brief Обработчик действия редактирования тел.
     */
    void on_actSimEdit_triggered();

    /**
     * @brief Обработчик действия сохранения данных в файл.
     */
    void on_actSaveFile_triggered();

    /**
     * @brief Обработчик действия загрузки данных из файла.
     */
    void on_actOpenFile_triggered();

    /**
     * @brief Обработчик действия сброса симуляции.
     */
    void on_actSimReset_triggered();

    /**
     * @brief Обработчик действия отображени / скрытия окна лога.
     */
    void on_actShowHideLog_triggered();

    /**
     * @brief Обработчик действия сохранения скриншота.
     */
    void on_actScreenShot_triggered();

    /**
     * @brief Обработчик действия о программе.
     */
    void on_actAbout_triggered();

    /**
     * @brief Обработчик действия о Qt.
     */
    void on_actAboutQt_triggered();

protected:

    /**
     * @brief Обработчик закрытия окна.
     * @param event Событие закрытия окна.
     */
    void closeEvent(QCloseEvent*);

private:

    /**
     * @brief обновление интерфейса.
     */
    void refreshUi();

    /**
     * @brief сброс данных симуляции.
     */
    void resetSimData();

    //! Счётчик времени симуляции.
    qreal simulated_years;

    //! Счётчик кадров в секунду.
    quint32 cur_fps;

    //! Счётчик кадров.
    quint32 cur_frames;

    //! Текущий каталог.
    QString cur_dir;

    //! Файл лога.
    QFile* logFile;

    //! Таймер FPS.
    QTimer* fpsTimer;

    //! Интерфейс пользователя.
    Ui::MainWindow *ui;

    //! Элемент визуализации и симуляции.
    NBodyWidget* nbodyWidget;

    //! Диалог настроек.
    OCLSettingsDialog* oclSettingsDlg;

    //! Диалог редактирования.
    EditBodyDialog* editBodyDlg;

    //! Диалог настроек генерации.
    GenSettingsDialog* genSettingsDlg;
};

#endif // MAINWINDOW_H
