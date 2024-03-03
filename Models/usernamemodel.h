#ifndef USERNAMEMODEL_H
#define USERNAMEMODEL_H

#include "baselistmodel.h"
#include "constants.h"

class UserNameModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit UserNameModel(AppManager *parent): BaseListModel(parent) {}
    void update(const DBRecordList&);
};

#endif // USERNAMEMODEL_H
