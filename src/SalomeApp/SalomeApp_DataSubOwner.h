
#ifndef SALOMEAPP_DATASUBOWNER_H
#define SALOMEAPP_DATASUBOWNER_H

#include <SalomeApp.h>
#include <SalomeApp_DataOwner.h>

/*!
  Class provide sub owner.
 */
class SALOMEAPP_EXPORT SalomeApp_DataSubOwner : public SalomeApp_DataOwner
{
public:
    SalomeApp_DataSubOwner( const QString&, const int );
    virtual ~SalomeApp_DataSubOwner();

    virtual bool isEqual( const SUIT_DataOwner& ) const;
    int          index() const;

private:
    int      myIndex;
};

#endif
