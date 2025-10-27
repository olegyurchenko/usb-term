#include "inputform.h"
#include "ui_inputform.h"
#include "hex_dump.h"
#include <QTime>
#include <QScrollBar>

InputForm::InputForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::InputForm)
{
  ui->setupUi(this);
}

InputForm::~InputForm()
{
  delete ui;
}

void InputForm::onClear()
{
  ui->label->setText(QString());
}

void InputForm::addLogText(Cathegory cathegory, const QString& label, const QByteArray& data) {
  int elapsed = m_timer.restart();
  if(elapsed < 0) {
    elapsed = 0;
  }
  auto time = QTime::currentTime();

  auto title = QString(
    "<p style=\"color: %1;\">%2 %3 %4</p>"
    ).arg(
        cathegory == Error ? "red" : cathegory == Warning ? "while" : "navy",
        time.toString("hh:mm:ss"),
        QString::number(elapsed),
        label
        );
  auto text = ui->label->text();
  text += title;
  text += "\n";
  if(!data.isEmpty()) {
    text += "<pre>";
    text += hexDump(data);
    text += "</pre>\n";
  }
  ui->label->setText(text);
  ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}
