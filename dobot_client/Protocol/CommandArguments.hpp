
#pragma once

#include <QObject>

class CommandArguments : public QObject
{
    Q_OBJECT
public:
    explicit CommandArguments(uint16_t messageCode,
                              std::string commandName,
                              uint8_t* payload,
                              uint16_t payloadSize,
                              QObject *parent = nullptr);

    int getPayloadSize() const;
    uint8_t* getPayload() const;


signals:


private:


    uint16_t m_messageId;

    uint8_t* m_payload;

    int m_payloadSize;

    std::string m_commandName;

};
