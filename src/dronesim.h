#ifndef DRONESIM_H
#define DRONESIM_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>

class ATCommandParser;
class NavdataGenerator;
class NetworkThread;

class DroneSim : public QObject
{
    Q_OBJECT
public:
    explicit DroneSim(QObject *parent = 0);

    void start();

signals:

public slots:

private:
    NetworkThread* m_NetworkThread;
};

class NetworkThread : public QThread
{
    Q_OBJECT
public:
    NetworkThread(DroneSim* parent);

    void run();

signals:

public slots:
    void dataInNavSock();
    void dataInCmdSock();
private:
    void updateState();

    enum DroneState
    {
        state_bootstrap,
        state_init,
        state_navdata_demo,
        state_ready,
        state_flying
    };
    DroneState m_state;

    // AT Command parser
    ATCommandParser* m_ATCmdParser;

    // Navdata Generator
    NavdataGenerator* m_navdataGen;

    // Network sockets and the needed port numbers
    QUdpSocket* m_navSock;
    QUdpSocket* m_cmdSock;
    const qint16 m_navdataPort;
    const qint16 m_cmdPort;

    // Handle to the parent
    DroneSim* m_parent;

    // IP address of the client
    QHostAddress m_hostAddr;

    // Buffers for receiving data from the UDP sockets
    char m_navSockBuf[2048];
    char m_cmdSockBuf[2048];

};

#endif // DRONESIM_H
