#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
  Q_OBJECT
public:
  enum Type {
    UsbConnection,
    LpConnection
  };
  explicit ConnectionDialog(QWidget *parent = nullptr);
  ~ConnectionDialog();
  Type type() const {return m_type;}
protected:
  Type m_type = UsbConnection;
protected slots:
  void onChangeType();
private:
  Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
