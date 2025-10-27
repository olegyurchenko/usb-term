#ifndef INPUTFORM_H
#define INPUTFORM_H

#include <QWidget>
#include <QElapsedTimer>

namespace Ui {
class InputForm;
}

class InputForm : public QWidget
{
  Q_OBJECT
  QElapsedTimer m_timer;
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

private:
  Ui::InputForm *ui;
};

#endif // INPUTFORM_H
