#ifndef FINALPROJECT_H
#define FINALPROJECT_H

#include <QMainWindow>

namespace Ui {
class FinalProject;
}

class FinalProject : public QMainWindow
{
    Q_OBJECT

public:
    explicit FinalProject(QWidget *parent = 0);
    ~FinalProject();

private slots:
    void on_checkBox_clicked();

    void on_radioButton_clicked();

    void on_leftButton_clicked();

    void on_rightButton_clicked();

    void turn_left();

    void turn_right();

    void turn_off();

private:
    Ui::FinalProject *ui;
};

#endif // FINALPROJECT_H
