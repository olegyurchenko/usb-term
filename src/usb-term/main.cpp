#include "mainwindow.h"

#include <QApplication>
#include "text_parser.h"
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  parseTest();
  return a.exec();
}
