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
  timer.setSingleShot(true);
  timer.setInterval(100);

  connect(&timer, &QTimer::timeout, this, &InputForm::onAfterLog);
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
  int elapsed = elapsedTimer.restart();
  if(elapsed < 0) {
    elapsed = 0;
  }
  auto time = QTime::currentTime();

  auto title = QString(
    "<pre style=\"color: %1;\">%2 %3 %4\n"
    ).arg(
        cathegory == Error ? "red" : cathegory == Warning ? "orange" : "navy",
        time.toString("hh:mm:ss"),
        QString("%1").arg(elapsed, 6),
        label
        );
  auto text = ui->label->text();
  text += title;
  if(!data.isEmpty()) {
    text += hexDump(data);
  }
  text += "</pre>\n";
  ui->label->setText(text);
  timer.start();
}

void InputForm::onAfterLog()
{
  ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}
