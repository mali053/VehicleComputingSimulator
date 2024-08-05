#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <iostream>
#include <QStyleOption>
#include <QPainter>
#include <qboxlayout.h>
#include "draggable_square.h"
#include "process.h"
#include "main_window.h"

DraggableSquare::DraggableSquare(QWidget *parent, const QString &color, 
                                 int width, int height) 
    : QWidget(parent), label(new QLabel(this)), dragging(false)
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

void DraggableSquare::setProcess(const Process &process)
{
    this->process = process;
    this->id = process.getId();

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

DraggableSquare::~DraggableSquare()
{
    if (label) {
        delete label;
        label = nullptr;
    }
    
    qDebug() << "DraggableSquare with ID" << id << "is being destroyed.";
}

void DraggableSquare::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        if (id < 0 || id > 4) { // Prevent menu for IDs 1 to 4
            QMenu contextMenu(this);

            QAction *editAction = contextMenu.addAction("Edit");
            QAction *deleteAction = contextMenu.addAction("Delete");

            QAction *selectedAction = contextMenu.exec(event->globalPos());

            if (selectedAction == editAction) {
                editSquare(id);
            } else if (selectedAction == deleteAction) {
                deleteSquare(id);
            }
        }
    } else if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
        initialPosition = pos();
        dragging = true;
    } else {
        QWidget::mousePressEvent(event);
    }
}

void DraggableSquare::mouseMoveEvent(QMouseEvent *event)
{
    if (!dragging) {
        return;
    }
    if (!(event->buttons() & Qt::LeftButton)) return;

    QPoint newPos = mapToParent(event->pos() - dragStartPosition);
    newPos.setX(qMax(0, qMin(newPos.x(), parentWidget()->width() - width())));
    newPos.setY(qMax(0, qMin(newPos.y(), parentWidget()->height() - height())));
    
    move(newPos);
}

void DraggableSquare::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }

    QWidget::mouseReleaseEvent(event);
}

void DraggableSquare::editSquare(int id)
{
    MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget()->window());
    if (mainWindow) {
        mainWindow->editSquare(id);
    }
}

void DraggableSquare::deleteSquare(int id)
{
    MainWindow *mainWindow = qobject_cast<MainWindow*>(parentWidget()->window());
    if (mainWindow) {
        mainWindow->deleteSquare(id);
    }
}