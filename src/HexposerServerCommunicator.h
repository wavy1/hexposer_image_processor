//
// Created by Wavy on 15.11.2018.
//

#ifndef HEXPOSERSERVERCOMMUNICATOR_H
#define HEXPOSERSERVERCOMMUNICATOR_H

#include <QGuiApplication>
#include <QWidget>
#include <QtWidgets>
#include <QtCore/QSettings>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkSession>
#include <QtWidgets/QLabel>
#include <c++/iostream>


class HexposerServerCommunicator : public QDialog
{
Q_OBJECT
public:
    explicit HexposerServerCommunicator(QWidget *parent = nullptr);

private Q_SLOTS:
    void sessionOpened();
    void sendMessage();

private:
    QLabel *statusLabel = nullptr;
    QTcpServer *tcpServer = nullptr;
    QVector<QString> fortunes;
    QNetworkSession *networkSession= nullptr;
};

#endif // HEXPOSERSERVERCOMMUNICATOR_H