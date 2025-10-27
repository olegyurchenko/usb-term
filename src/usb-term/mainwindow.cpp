#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "outputform.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileInfo>
#include "connectiondialog.h"
#include "usbcon.h"
#include "inputform.h"
#include "text_parser.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connection = new UsbConnection();
  onFileNew();
  ui->tabWidget->setTabsClosable(true);
  connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequest);

  timer = new QTimer();
  timer->setSingleShot(false);
  connect(timer, &QTimer::timeout, this, &MainWindow::onTimer);
  timer->setInterval(20);
  timer->start();
}

MainWindow::~MainWindow()
{
  delete timer;
  delete connection;
  delete ui;
}

OutputForm *MainWindow::activeForm()
{
  auto w = ui->tabWidget->currentWidget();
  OutputForm *form = qobject_cast<OutputForm *>(w);
  return form;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
  int count = ui->tabWidget->count();
  for(int i = 0; i < count; i++) {
    ui->tabWidget->setCurrentIndex(i);
    auto form = activeForm();
    if(form && !modifiedQuestion(form)) {
      e->ignore();
      return;
    }
  }
}

bool MainWindow::modifiedQuestion(OutputForm *form)
{
  bool modified = form->isModified();
  if(modified) {
    QString prompt =  tr("Data was modified. Save changes to file ?");
    QMessageBox::StandardButtons buttons = QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard;
    switch(QMessageBox::question(this,
                                 tr("Data modified"),
                                 prompt,
                                 buttons,
                                 QMessageBox::Cancel))
    {
      case QMessageBox::Save:
      case QMessageBox::Yes:
        onFileSave();
        return !form->isModified();
      case QMessageBox::Discard:
        return true;
      case QMessageBox::Cancel:
      default:
        return false;
    }
    return false;
  }
  return true;
}

void MainWindow::onFileNew()
{
  auto form = new OutputForm(this);
  connect(form, &OutputForm::fileChanged, this, &MainWindow::onFileChanged, Qt::QueuedConnection);
  ui->tabWidget->addTab(form, tr("unknown"));
  ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

void MainWindow::onFileOpen()
{
  auto form = activeForm();
  if(!form) {
    return;
  }
  if(!modifiedQuestion(form)) {
    return;
  }

  auto fileName = form->fileName();
  if(!fileName.isEmpty()) {
    onFileNew();
    form = activeForm();
    if(!form) {
      return;
    }
    fileName.clear();
  }

  fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "",
                                            tr("Text files (*.txt);;All files(*.*)"));

  if(fileName.isEmpty()) {
    return;
  }
  form->loadFile(fileName);
  onTabChanged();
}

void MainWindow::onFileSave()
{
  auto form = activeForm();
  if(!form) {
    return;
  }
  auto fileName = form->fileName();
  if(fileName.isEmpty()) {
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Save file"), "",
                                            tr("Text files (*.txt);;All files(*.*)"));
  }

  if(fileName.isEmpty()) {
    return;
  }

  QFileInfo fileInfo(fileName);
  if(fileInfo.suffix().isEmpty()) {
    fileName += ".txt";
  }

  form->saveFile(fileName);
  onTabChanged();
}


void MainWindow::onFileSaveAs()
{
  auto form = activeForm();
  if(!form) {
    return;
  }
  auto fileName = QFileDialog::getSaveFileName(this,
                                          tr("Save file"), "",
                                          tr("Text files (*.txt);;All files(*.*)"));
  if(fileName.isEmpty()) {
    return;
  }

  QFileInfo fileInfo(fileName);
  if(fileInfo.suffix().isEmpty()) {
    fileName += ".txt";
  }


  form->saveFile(fileName);
  onTabChanged();
}

void MainWindow::onFileClose()
{
  auto form = activeForm();
  if(!form) {
    return;
  }
  if(!modifiedQuestion(form)) {
    return;
  }
  int index = ui->tabWidget->currentIndex();
  ui->tabWidget->removeTab(index);
}

void MainWindow::onExit()
{
  close();
}

void MainWindow::onConnectionOpen()
{
  ConnectionDialog dialog(this);
  if(dialog.exec() == QDialog::Accepted) {
    auto vid = dialog.vid();
    auto pid = dialog.pid();
    if(!connection->open(vid, pid)) {
      QMessageBox::critical(this, tr("Error open connection"), QString::fromStdString(connection->message()));
      return;
    }
    ui->actionConnectionOpen->setEnabled(false);
    ui->actionConnectionClose->setEnabled(true);
    ui->actionSendData->setEnabled(true);
  }
}

void MainWindow::onConnectionSend()
{
  auto form = activeForm();
  if(!form) {
    return;
  }
  auto data = parseText(form->text());
  ui->inputForm->addLogText(InputForm::Info, QString("%1(%2)").arg(tr("Sent"), QString::number(data.size())), data);
  connection->write(data.data(), data.size());
  if(connection->isError()) {
    ui->inputForm->addLogText(InputForm::Error, QString::fromStdString(connection->message()));
  }
}

void MainWindow::onConnectionClose()
{
  connection->close();
  ui->actionConnectionOpen->setEnabled(true);
  ui->actionConnectionClose->setEnabled(false);
  ui->actionSendData->setEnabled(false);
}

void MainWindow::onTabChanged()
{
  auto form = activeForm();
  if(!form) {
    return;
  }
  int index = ui->tabWidget->currentIndex();
  auto fileName = form->fileName();
  if(fileName.isEmpty()) {
    fileName = tr("unknown");
  } else {
    QFileInfo fileInfo(fileName);
    fileName = fileInfo.fileName();
  }
  bool modified = form->isModified();
  ui->tabWidget->setTabText(index, fileName + QString(modified ? " *" : ""));
  ui->actionSave->setEnabled(modified);
}

void MainWindow::onFileChanged()
{
  onTabChanged(); //TODO !!!
}

void MainWindow::onTabCloseRequest(int index)
{
  auto w = ui->tabWidget->widget(index);
  OutputForm *form = qobject_cast<OutputForm *>(w);
  if(form && modifiedQuestion(form)) {
    ui->tabWidget->removeTab(index);
  }
}

void MainWindow :: onTimer()
{
  if(connection->isOpened()) {
    char buffer[1024];
    int size = connection->read(buffer, sizeof(buffer), 10);
    if(size > 0) {
      ui->inputForm->addLogText(InputForm::Info, QString("%1(%2)").arg(tr("Received"), QString::number(size)), QByteArray(buffer, size));
    }
    if(size < 0) {
      ui->inputForm->addLogText(InputForm::Error, QString::fromStdString(connection->message()));
    }
  }
}
