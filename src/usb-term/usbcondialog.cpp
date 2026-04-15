#include "usbcondialog.h"
#include "ui_usbcondialog.h"
#include "usbcon.h"

UsbConnDialog::UsbConnDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::UsbConnDialog)
{
  ui->setupUi(this);
  deviceVector = usbDeviceList();

  for(const auto& item : deviceVector) {
    QString s = QString("%1:%2 %3,%4").arg(
          QString::number(item.idVendor, 16),
          QString::number(item.idProduct, 16),
          QString::fromStdString(item.vendor),
          QString::fromStdString(item.product));
    ui->comboBox->addItem(s);
  }

}

UsbConnDialog::~UsbConnDialog()
{
  delete ui;
}

void UsbConnDialog::onDeviceChanged(int i)
{
  if(i >= 0 && i < (int) deviceVector.size()) {
    ui->vidLineEdit->setText(QString::number(deviceVector[i].idVendor, 16));
    ui->pidLineEdit->setText(QString::number(deviceVector[i].idProduct, 16));
  }
}

void UsbConnDialog::setVid(uint16_t id)
{
  ui->vidLineEdit->setText(QString::number(id, 16));
}

void UsbConnDialog::setPid(uint16_t id)
{
  ui->pidLineEdit->setText(QString::number(id, 16));
}

uint16_t UsbConnDialog::vid() const
{
  return ui->vidLineEdit->text().toUShort(nullptr, 16);
}
uint16_t UsbConnDialog::pid() const
{
  return ui->pidLineEdit->text().toUShort(nullptr, 16);
}
