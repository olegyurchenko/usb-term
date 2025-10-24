#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class OutputForm;
class MainWindow : public QMainWindow
{
  Q_OBJECT
  OutputForm *activeForm();
  bool modifiedQuestion(OutputForm *form);
  void closeEvent(QCloseEvent *e) override;


public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected slots:
  void onFileNew();
  void onFileOpen();
  void onFileSave();
  void onFileSaveAs();
  void onFileClose();
  void onExit();
  void onConnectionOpen();
  void onConnectionSend();
  void onConnectionClose();
  void onTabChanged();
  void onFileChanged();
  void onTabCloseRequest(int index);

private:
  Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
