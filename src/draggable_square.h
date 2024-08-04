#ifndef __DRAGGABLE_SQUARE_H__
#define __DRAGGABLE_SQUARE_H__

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include "process.h"

class DraggableSquare : public QWidget
{
    Q_OBJECT

public:
    explicit DraggableSquare(QWidget *parent = nullptr);
    void setProcess(const Process &process);
    const Process getProcess() const;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint dragStartPosition;
    QPoint initialPosition;
    QLabel *label;
    Process process;
};

#endif // __DRAGGABLE_SQUARE_H__
