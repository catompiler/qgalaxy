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
     * @brief Обработчик инициализации системы моделирования.
     */
    void nbodyWidget_onInitialized();

    /**
     * @brief Обработчик окончания очередного шага моделирования.
     */
    void nbodyWidget_onSimulated();

    /**
     * @brief Обработчик действия выхода.
     */
    void on_actExit_triggered();

    /**
     * @brief Обработчик действия открытия настроек.
     */
    void on_actSettingsOCL_triggered();

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

    //! Текущий каталог.
    QString cur_dir;

    //! Файл лога.
    QFile* logFile;

    //! Интерфейс пользователя.
    Ui::MainWindow *ui;

    //! Элемент визуализации и симуляции.
    NBodyWidget* nbodyWidget;

    //! Диалог настроек.
    OCLSettingsDialog* oclSettingsDlg;

    //! Диалог редактирования.
    EditBodyDialog* editBodyDlg;
};

#endif // MAINWINDOW_H
