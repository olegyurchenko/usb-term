#ifndef INPUTFORM_H
#define INPUTFORM_H

#include <QWidget>
#include <QElapsedTimer>
#include <QTimer>

namespace Ui {
class InputForm;
}

class InputForm : public QWidget
{
  Q_OBJECT
  QTimer timer;
  QElapsedTimer elapsedTimer;
public:
  enum Cathegory {
    Info,
    Warning,
    Error
  };
public:
  explicit InputForm(QWidget *parent = nullptr);
  ~InputForm();

  void addLogText(Cathegory cathegory, const QString& label, const QByteArray& data = QByteArray());

protected slots:
  void onClear();
  void onAfterLog();

private:
  Ui::InputForm *ui;
};

#endif // INPUTFORM_H
