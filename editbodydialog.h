#ifndef EDITBODYDIALOG_H
#define EDITBODYDIALOG_H

#include <QDialog>

namespace Ui {
class EditBodyDialog;
}

class NBodyWidget;
class QShowEvent;
class QAbstractButton;


class EditBodyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditBodyDialog(NBodyWidget* nbodywgt, QWidget *parent = 0);
    ~EditBodyDialog();

private slots:
    void buttonBoxClicked(QAbstractButton* button);
    void on_sbIndex_valueChanged(int);
private:
    Ui::EditBodyDialog *ui;
    NBodyWidget* nbodyWidget;

    void showEvent(QShowEvent*);

    void refreshUi();
    void saveBody();
};

#endif // EDITBODYDIALOG_H
