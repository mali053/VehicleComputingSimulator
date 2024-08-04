#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <qboxlayout.h>
#include "draggable_square.h"

DraggableSquare::DraggableSquare(QWidget *parent) : QWidget(parent), label(new QLabel(this))
{
    setFixedSize(100, 100);
    setStyleSheet("background-color: green;");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    setLayout(layout);
}

void DraggableSquare::setProcess(const Process &process)
{
    this->process = process;
    label->setText(QString("ID: %1\nName: %2\nCMake: %3\nQEMU: %4")
        .arg(process.getId())
        .arg(process.getName())
        .arg(process.getCMakeProject())
        .arg(process.getQEMUPlatform()));
}

const Process DraggableSquare::getProcess() const
{
    return process;
}

void DraggableSquare::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
        initialPosition = pos();
    }
}

void DraggableSquare::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) return;
    QPoint newPos = mapToParent(event->pos() - dragStartPosition);
    newPos.setX(qMax(0, qMin(newPos.x(), parentWidget()->width() - width())));
    newPos.setY(qMax(0, qMin(newPos.y(), parentWidget()->height() - height())));
    move(newPos);
}
