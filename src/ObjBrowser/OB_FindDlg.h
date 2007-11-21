
#ifndef OBJECT_BROWSER_FIND_DIALOG_HEADER
#define OBJECT_BROWSER_FIND_DIALOG_HEADER

#include <QGroupBox>

class OB_Browser;
class QLineEdit;
class QPushButton;
class QCheckBox;

class OB_FindDlg : public QGroupBox
{
  Q_OBJECT

public:
  OB_FindDlg( OB_Browser* );
  virtual ~OB_FindDlg();

private slots:
  void onFind();
  void onClose();

private:
  QLineEdit* myData;
  QPushButton *myToFirst, *myToLast, *myNext, *myPrev, *myClose;
  QCheckBox *myIsCaseSens, *myIsRegExp;
  OB_Browser* myBrowser;
};

#endif
