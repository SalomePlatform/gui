//
// This program can be used for unit test of dialog box.
// You just have to include the interface file (*.h) and
// use the dialog class as illustrated in the functions TEST_*.
// (gboulant - 12/10/2010)
//
#include <QApplication>
#include <QtGui>
#include "GDialogTest.hxx"

void TEST_show() {
  GDialogTest * dialog = new GDialogTest();
  dialog->show();
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  TEST_show();
  return app.exec();   
}
