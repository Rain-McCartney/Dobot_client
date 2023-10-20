
#pragma once

#include <QObject>
#include <QPlainTextEdit>

class LogWindow : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit LogWindow(QWidget* parent = nullptr);

    void printDebugMessage(const QString &category,
                           const QString& message);

    void printInfoMessage(const QString &category,
                          const QString& message);

    void printErrorMessage(const QString &category,
                           const QString& message);

    void printWarningMessage(const QString &category,
                             const QString& message);


    void setDebugMessageEnabled(const bool& enabled);
    void setInfoMessageEnabled(const bool& enabled);
    void setErrorMessageEnabled(const bool& enabled);
    void setWarningMessageEnabled(const bool& enabled);

    bool getDebugMessageState() const;
    bool getInfoMessageState() const;
    bool getErrorMessageState() const;
    bool getWarningMessageState() const;

signals:

    void printMessageSignal(const QString& message);

private:

    /**
     * \brief The flag to print debug messages.
     */
    bool m_isDebug;

    /**
     * \brief The flag to print info messages.
     */
    bool m_isInfo;

    /**
     * \brief The flag to print error messages.
     */
    bool m_isError;

    /**
     * \brief The flag to print warning messages.
     */
    bool m_isWarning;

    void printMessage(const QString &category,
                      const QString& message);

};
