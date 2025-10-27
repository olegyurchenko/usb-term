#include "mainwindow.h"

#include <QApplication>
#include "text_parser.h"
#include "hex_dump.h"
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  parseTest();
  hexDumpTest();
  return a.exec();
}
