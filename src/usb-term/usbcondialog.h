#ifndef USBCONDIALOG_H
#define USBCONDIALOG_H

#include <QDialog>
#include <vector>
namespace Ui {
class UsbConnDialog;
}

struct UsbDeviceInfo;
class UsbConnDialog : public QDialog
{
  Q_OBJECT
  std::vector<UsbDeviceInfo> deviceVector;
public:
  explicit UsbConnDialog(QWidget *parent = nullptr);
  ~UsbConnDialog();

  void setVid(uint16_t);
  void setPid(uint16_t);

  uint16_t vid() const;
  uint16_t pid() const;

protected slots:
  void onDeviceChanged(int i);
private:
  Ui::UsbConnDialog *ui;
};

#endif // USBCONDIALOG_H
