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

void ConnectionDialog::onChangeType() {
  if(ui->usbRadio->isChecked()) {
    m_type = UsbConnection;
  } else if(ui->lpRradio->isChecked()) {
    m_type = LpConnection;
  }
}
