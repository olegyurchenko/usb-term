#ifndef OUTPUTFORM_H
#define OUTPUTFORM_H

#include <QWidget>

namespace Ui {
class OutputForm;
}

class OutputForm : public QWidget
{
  Q_OBJECT
  QString m_fileName;
public:
  explicit OutputForm(QWidget *parent = nullptr);
  ~OutputForm();

  QString fileName() const {return m_fileName;}
  void setFileName(const QString& f) {m_fileName = f; emit fileNameChanged(f);}
  bool isModified() const;
  QString text();

public slots:
  void loadFile(const QString& f);
  void saveFile(const QString& f);

signals:
  void fileNameChanged(QString);
  void fileChanged();

private:
  Ui::OutputForm *ui;
};

#endif // OUTPUTFORM_H
