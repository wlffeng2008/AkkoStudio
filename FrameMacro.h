#ifndef FRAMEMACRO_H
#define FRAMEMACRO_H

#include <QFrame>
#include <QElapsedTimer>

#include "modulemacromanager.h"
class MacroSquare;

namespace Ui {
class FrameMacro;
}

class FrameMacro : public QFrame
{
    Q_OBJECT

public:
    explicit FrameMacro(QWidget *parent = nullptr);
    ~FrameMacro();

    void addMacroSquare(const QString&text,quint8 type,quint16 value,bool down=true);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

private:
    Ui::FrameMacro *ui;
    QElapsedTimer m_tcount;
    quint16 m_lastDelay = 0;
    MacroSquare *m_delay = nullptr;
    bool m_recording = false;
    bool m_canAdd = true;

    ModuleMacroManager *m_pMM = nullptr;
    MacroProject *m_prj = nullptr;

    void updateView();
    void removeView();

    void addMacroBar(QObject *item);
    void deleteMacro(QWidget *item);

    bool m_loading=false;
    void loadEvents();
    void saveEvents();
};

#endif // FRAMEMACRO_H
