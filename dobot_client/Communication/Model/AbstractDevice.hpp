
#pragma once

#include <QObject>

class AbstractDevice : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDevice(QObject *parent = nullptr);


signals:

};

