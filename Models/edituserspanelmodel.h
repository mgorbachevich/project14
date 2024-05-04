#ifndef EDITUSERSPANELMODEL_H
#define EDITUSERSPANELMODEL_H

#include "baselistmodel.h"

class Users;

class EditUsersPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit EditUsersPanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(Users*);

private:
    Users* users;
};

#endif // EDITUSERSPANELMODEL_H
