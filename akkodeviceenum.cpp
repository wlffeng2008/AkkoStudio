#include "akkodeviceenum.h"
#include "MainWindow.h"

AkkoDeviceEnum::AkkoDeviceEnum(QObject *parent)
    : QThread{parent}
{
    start();
}

void AkkoDeviceEnum::Exit()
{
    m_bEnding = true;
}

void AkkoDeviceEnum::DoEnum()
{
    m_enum = true;
}

void AkkoDeviceEnum::run()
{
    while(!m_bEnding)
    {
        if(!m_enum)
        {
            QThread::msleep(10);
            continue;
        }

        m_enum = false;

        m_Root->enumDevice();
    }
}