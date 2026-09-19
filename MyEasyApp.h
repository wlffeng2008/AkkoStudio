#ifndef MYEASYAPP_H
#define MYEASYAPP_H

#include <QApplication>
#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>

class MyEasyApp : public QApplication
{
    Q_OBJECT
public:
    MyEasyApp(int &argc, char **argv) : QApplication(argc, argv) {}

    bool notify(QObject *receiver, QEvent *event) override
    {
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            emit onKeyEvent(receiver,keyEvent) ;

        }
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);
            emit onMouseEvent(receiver,mEvent);
        }
        transEvent(receiver, event);
        return QApplication::notify(receiver, event);
    }

signals:
    void transEvent(QObject *receiver, QEvent *event);
    void onKeyEvent(QObject *receiver,QKeyEvent *kEvent) ;
    void onMouseEvent(QObject *receiver,QMouseEvent *mEvent) ;
};


#endif // MYEASYAPP_H
