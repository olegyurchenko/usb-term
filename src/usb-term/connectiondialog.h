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
  explicit ConnectionDialog(QWidget *parent = nullptr);
  ~ConnectionDialog();

  void setVid(uint16_t);
  void setPid(uint16_t);

  uint16_t vid() const;
  uint16_t pid() const;

private:
  Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONDIALOG_H
