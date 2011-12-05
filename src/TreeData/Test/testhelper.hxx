
#ifndef _TESTHELPER_HXX_
#define _TESTHELPER_HXX_

#include <QString>
#include "TreeModel.hxx"
#include "DataObject.hxx"

#define DATAFILENAME "data.txt"

QString      TESTHELPER_testfilename(const char * basefilename);
DataObject * TESTHELPER_dummyObject(QString label);
void         TESTHELPER_loadDataFromFile(TreeModel * dataTreeModel, const QString &filename);

#endif // _TESTHELPER_HXX_
