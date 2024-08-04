#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <qboxlayout.h>
#include "draggable_square.h"
#include <QWidget>
#include <QLabel>
#include <QString>

// Update the constructor definition to match the declaration
DraggableSquare::DraggableSquare(QWidget *parent, const QString &color, int width, int height) 
    : QWidget(parent), label(new QLabel(this))
{
    setFixedSize(width, height);
    setStyleSheet(color);
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

const QPoint DraggableSquare::getDragStartPosition() const
{
    return dragStartPosition;
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