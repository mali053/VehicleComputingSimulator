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
#include <iostream>

// Add this function to your DraggableSquare class
void DraggableSquare::print() const {
    std::cout << "DraggableSquare:" << std::endl;
    std::cout << "  Process ID: " << process.getId() << std::endl;
    std::cout << "  Drag Start Position: (" << dragStartPosition.x() << ", " << dragStartPosition.y() << ")" << std::endl;
    std::cout << "  Initial Position: (" << initialPosition.x() << ", " << initialPosition.y() << ")" << std::endl;
    std::cout << "  Color: " << label->styleSheet().toStdString() << std::endl;
    std::cout << "  Size: (" << this->width() << ", " << this->height() << ")" << std::endl;
}

void DraggableSquare::setSquareColor(const QString &color) {
    setStyleSheet(color);
}
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


// Copy constructor
DraggableSquare::DraggableSquare(const DraggableSquare &other)
    : QWidget(other.parentWidget()), dragStartPosition(other.dragStartPosition), initialPosition(other.initialPosition),
      label(new QLabel(other.label->text(), this)), process(other.process) // Copy QLabel's text
{
    setFixedSize(other.width(), other.height());
    setStyleSheet(other.styleSheet());
}

// Copy assignment operator
DraggableSquare &DraggableSquare::operator=(const DraggableSquare &other)
{
    if (this == &other) {
        return *this;
    }

    dragStartPosition = other.dragStartPosition;
    initialPosition = other.initialPosition;
    delete label;
    label = new QLabel(other.label->text(), this); // Copy QLabel's text
    process = other.process;

    setFixedSize(other.width(), other.height());
    setStyleSheet(other.styleSheet());

    return *this;
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