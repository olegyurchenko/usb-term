#include "connectiondialog.h"
#include "ui_connectiondialog.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ConnectionDialog)
{
  ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog()
{
  delete ui;
}

void ConnectionDialog::setVid(uint16_t id)
{

}
void ConnectionDialog::setPid(uint16_t id)
{

}
uint16_t ConnectionDialog::vid() const
{

}
uint16_t ConnectionDialog::pid() const
{

}
