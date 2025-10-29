#include "outputform.h"
#include "ui_outputform.h"
#include <QMessageBox>
#include <QFile>

OutputForm::OutputForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::OutputForm)
{
  ui->setupUi(this);
  connect(ui->edit, &QPlainTextEdit::textChanged, this, &OutputForm::fileChanged);
}

OutputForm::~OutputForm()
{
  delete ui;
}

bool OutputForm::isModified() const {
  return ui->edit->document()->isModified();
}

QString OutputForm::text() {
  return ui->edit->toPlainText();
}

void OutputForm::loadFile(const QString& f)
{
  QFile file(f);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Error open file"), file.errorString());
    return;
  }

  auto text = file.readAll();
  if(file.error() != QFileDevice::NoError) {
    QMessageBox::critical(this, tr("Error read file"), file.errorString());
    return;
  }
  ui->edit->document()->setPlainText(QString::fromUtf8(text));
  ui->edit->document()->setModified(false);
  setFileName(f);
}

void OutputForm::saveFile(const QString& f)
{
  QFile file(f);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("Error open file"), file.errorString());
    return;
  }
  file.write(ui->edit->toPlainText().toUtf8());
  if(file.error() != QFileDevice::NoError) {
    QMessageBox::critical(this, tr("Error read file"), file.errorString());
    return;
  }
  setFileName(f);
  ui->edit->document()->setModified(false);
}

