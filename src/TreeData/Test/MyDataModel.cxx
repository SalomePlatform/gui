#include "MyDataModel.hxx"

//
// =================================================================
// MyDataObject implementation
// =================================================================
//

const string MyDataObject::PROPERTY_KEY_TYPE    = "type";
const string MyDataObject::PROPERTY_KEY_CIRCUIT ="circuit";
const string MyDataObject::PROPERTY_KEY_REPFONC ="rf";

MyDataObject::MyDataObject() : DataObject() {
  this->setProperty(PROPERTY_KEY_TYPE, "Tuyauterie");
  this->setProperty(PROPERTY_KEY_CIRCUIT,"RRA");
  this->setProperty(PROPERTY_KEY_REPFONC,"RF_01");
}

/*! This function specified the localization of the object in the
 * hierarchical organization
 */
string MyDataObject::getPath() {
  // We choose here a convention for organizing the path for this
  // class of object.
  /*
  string path = getProperty(PROPERTY_KEY_CIRCUIT) + pathsep
    + getProperty(PROPERTY_KEY_REPFONC) + pathsep
    + getProperty(PROPERTY_KEY_TYPE);
  */
  string path = getProperty(PROPERTY_KEY_TYPE) + pathsep
    + getProperty(PROPERTY_KEY_CIRCUIT) + pathsep
    + getProperty(PROPERTY_KEY_REPFONC);
    
  return path;
}

//
// =================================================================
// MyDataModel implementation
// =================================================================
//
MyDataModel::MyDataModel() : DataModel() {
}

DataObject * MyDataModel::newDataObject() {
  MyDataObject * dataObject = new MyDataObject();
  return dataObject;
}
