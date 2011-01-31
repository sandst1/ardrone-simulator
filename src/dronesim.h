#ifndef DRONESIM_H
#define DRONESIM_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>

class DroneThread;

class DroneSim : public QObject
{
    Q_OBJECT
public:
    explicit DroneSim(QObject *parent = 0);

    void start();

signals:

public slots:

private:
    DroneThread* m_dronethread;
};

class DroneThread : public QThread
{
    Q_OBJECT
public:
    DroneThread(DroneSim* parent);

    void run();

signals:

public slots:
    void stateTimerTick();

    void dataInNavSock();
    void receivedCmd();
private:
    enum DroneState
    {
        state_bootstrap,
        state_init,
        state_navdata_demo,
        state_ready,
        state_flying
    };
    DroneState m_state;

    QUdpSocket* m_navSock;
    QUdpSocket* m_cmdSock;
    DroneSim* m_parent;

    QTimer* m_timer;
};

#endif // DRONESIM_H
