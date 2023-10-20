
#pragma once

#include <QFuture>

class CommunicationException : public QException
{

public:
    explicit CommunicationException(const QString& message);

    void raise() const override;

    CommunicationException* clone() const override;

    QString getMessage() const;

signals:

private:

    QString m_message;

};
