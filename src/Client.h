//
// Created by Wavy on 25.11.2018.
//

#ifndef HEXAPOSER_CLIENT_H
#define HEXAPOSER_CLIENT_H


#include <QtWidgets/QDialog>
#include <QtNetwork/QAbstractSocket>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkSession>

class Client : public QDialog
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);

private slots:
            void requestNewFortune();
    void readFortune();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableGetFortuneButton();
    void sessionOpened();

private:
    QComboBox *hostCombo = nullptr;
    QLineEdit *portLineEdit = nullptr;
    QLabel *statusLabel = nullptr;
    QPushButton *getFortuneButton = nullptr;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
    QString currentFortune;

    QNetworkSession *networkSession = nullptr;
};

#endif //HEXAPOSER_CLIENT_H
