#include "finalproject.h"
#include "ui_finalproject.h"
#include <fstream>
#include <iostream>
using namespace std;

bool turned_on = false;
bool to_left = true;
ofstream gpio5_file;
ofstream gpio6_file;
ifstream left_isr_sts_file;
ifstream right_isr_sts_file;
char data_file_left[100];
char data_file_right[100];

FinalProject::FinalProject(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FinalProject)
{
    ui->setupUi(this);
    gpio5_file.open("/sys/class/gpio/gpio5/value", ios::out);
    gpio6_file.open("/sys/class/gpio/gpio6/value", ios::out);
    left_isr_sts_file.open("/sys/module/motor/parameters/left_events", ios::in);
    right_isr_sts_file.open("/sys/module/motor/parameters/right_events", ios::in);
    ui->left_label->setText("");
    ui->right_label->setText("");
}

FinalProject::~FinalProject()
{
    gpio5_file.close();
    gpio6_file.close();
    left_isr_sts_file.close();
    right_isr_sts_file.close();
    delete ui;
}

void FinalProject::on_checkBox_clicked()
{
    if(!turned_on) { // Turning onn
        ui->checkBox->setText("Off");
        if(to_left){
            turn_left();
        } else {
            turn_right();
        }
    } else { // Turining off
        ui->checkBox->setText("On");
        turn_off();
    }
    turned_on = !turned_on;

}


void FinalProject::on_leftButton_clicked()
{
    to_left = true;
    if(turned_on) {
        turn_left();
    }
    left_isr_sts_file.seekg(0);
    left_isr_sts_file >> data_file_left;
    ui->left_label->setText(data_file_left);

}

void FinalProject::on_rightButton_clicked()
{
    to_left = false;
    if(turned_on) {
        turn_right();
    }
    right_isr_sts_file.seekg(0);
    right_isr_sts_file >> data_file_right;
    ui->right_label->setText(data_file_right);
}

void FinalProject::turn_left(){
    gpio5_file << 0 << endl;
    gpio6_file << 1 << endl;
}

void FinalProject::turn_right(){
    gpio5_file << 1 << endl;
    gpio6_file << 0 << endl;
}

void FinalProject::turn_off(){
    gpio5_file << 0 << endl;
    gpio6_file << 0 << endl;
}
