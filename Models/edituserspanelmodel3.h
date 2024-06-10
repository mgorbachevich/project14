#ifndef EDITUSERSPANELMODEL3_H
#define EDITUSERSPANELMODEL3_H

#include "baselistmodel3.h"

class Users;

class EditUsersPanelModel3 : public BaseListModel3
{
    Q_OBJECT

public:
    explicit EditUsersPanelModel3(AppManager *parent): BaseListModel3(parent) {}
    QHash<int, QByteArray> roleNames() const override;
    void update(Users*);
};

#endif // EDITUSERSPANELMODEL3_H
