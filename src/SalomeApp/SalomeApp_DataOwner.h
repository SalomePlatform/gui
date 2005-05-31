
#ifndef SALOMEAPP_DATAOWNER_H
#define SALOMEAPP_DATAOWNER_H

#include <SUIT_DataOwner.h>

class SalomeApp_DataOwner : public SUIT_DataOwner
{
public:
    SalomeApp_DataOwner( const QString& );
    virtual ~SalomeApp_DataOwner();

    virtual bool isEqual( const SUIT_DataOwner& ) const;
    QString      entry() const;

private:
    QString  myEntry;
};

typedef SMART(SalomeApp_DataOwner) SalomeApp_DataOwnerPtr;

#endif
