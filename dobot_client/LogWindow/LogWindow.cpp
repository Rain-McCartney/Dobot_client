
#include <QTime>
#include <QMessageBox>
#include "LogWindow.hpp"

LogWindow::LogWindow(QWidget *parent)
    :QPlainTextEdit{parent},
      m_isDebug(true),
      m_isInfo(true),
      m_isError(true),
      m_isWarning(true)
{
    QPalette logWindowPalette = this->palette();
    logWindowPalette.setColor(QPalette::Base, Qt::black);
    logWindowPalette.setColor(QPalette::Text, Qt::green);
    this->setPalette(logWindowPalette);
    this->setReadOnly(true);

    connect(this, &LogWindow::printMessageSignal, this, &LogWindow::appendPlainText);
}

void LogWindow::printDebugMessage(const QString &category,
                                  const QString &message)
{
    if (m_isDebug)
    {
        this->printMessage(category, message);
    }
    else
    {
        return;
    }
}

void LogWindow::printInfoMessage(const QString &category,
                                 const QString &message)
{
    if (m_isInfo)
    {
        this->printMessage(category, message);
    }
    else
    {
        return;
    }
}

void LogWindow::printErrorMessage(const QString &category, const QString &message)
{
    if (m_isError)
    {
        this->printMessage(category, message);
    }
    else
    {
        return;
    }
}

void LogWindow::printWarningMessage(const QString &category,
                                    const QString &message)
{
    if (m_isWarning)
    {
        this->printMessage(category, message);
    }
    else
    {
        return;
    }
}

void LogWindow::setDebugMessageEnabled(const bool &enabled)
{
    m_isDebug = enabled;
}

void LogWindow::setInfoMessageEnabled(const bool &enabled)
{
    m_isInfo = enabled;
}

void LogWindow::setErrorMessageEnabled(const bool &enabled)
{
    m_isError = enabled;
}

void LogWindow::setWarningMessageEnabled(const bool &enabled)
{
    m_isWarning = enabled;
}

bool LogWindow::getDebugMessageState() const
{
    return m_isDebug;
}

bool LogWindow::getInfoMessageState() const
{
    return m_isInfo;
}

bool LogWindow::getErrorMessageState() const
{
    return m_isError;
}

bool LogWindow::getWarningMessageState() const
{
    return m_isWarning;
}

void LogWindow::printMessage(const QString &category,
                             const QString &message)
{ 
    QString date = QString("|%1|: ").arg(QTime::currentTime().toString());

    QString mainLine;
    mainLine.append(category);
    mainLine.append(date);
    mainLine.append(message);

    emit printMessageSignal(mainLine);
}
