#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <vector>
namespace Ui {
class ConnectionDialog;
}

struct UsbDeviceInfo;
class ConnectionDialog : public QDialog
{
  Q_OBJECT
  std::vector<UsbDeviceInfo> deviceVector;
public:
  explicit ConnectionDialog(QWidget *parent = nullptr);
  ~ConnectionDialog();

  void setVid(uint16_t);
  void setPid(uint16_t);

  uint16_t vid() const;
  uint16_t pid() const;

protected slots:
  void onDeviceChanged(int i);
private:
  Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
