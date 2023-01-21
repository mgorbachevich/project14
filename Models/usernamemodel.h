#ifndef USERNAMEMODEL_H
#define USERNAMEMODEL_H

#include "baselistmodel.h"
#import "constants.h"

class UserNameModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit UserNameModel(QObject *parent): BaseListModel(parent) {}
    void update(const DBRecordList&);
    DBRecord& userByIndex(const int);

private:
    DBRecordList users;
};

#endif // USERNAMEMODEL_H
