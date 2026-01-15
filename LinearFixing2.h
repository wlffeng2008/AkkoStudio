#ifndef LINEARFIXING2_H
#define LINEARFIXING2_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class LinearFixing2 : public QDialog
{
    Q_OBJECT

public:
    explicit LinearFixing2(const QString& title,const QString& content, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
};

#endif // LINEARFIXING1_H
