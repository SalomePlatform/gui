#ifndef _MYDATAMODEL_H_
#define _MYDATAMODEL_H_

//
// =================================================================
// Definition of an atom in the data model as an implementation of
// the virtual class DataObject
// =================================================================
//

#include "DataObject.hxx"
class MyDataObject: public DataObject {
public:
  MyDataObject();
  virtual string getPath();
  static const string PROPERTY_KEY_TYPE;
  static const string PROPERTY_KEY_CIRCUIT;
  static const string PROPERTY_KEY_REPFONC;
};


//
// =================================================================
// Definition of the data model as an implementation of the virtual
// class DataModel. It implements the DataObject factory.
// =================================================================
//
#include "DataModel.hxx"
class MyDataModel: public DataModel {
public:
  MyDataModel();
  virtual DataObject * newDataObject();
};

#endif // _MYDATAMODEL_H_
