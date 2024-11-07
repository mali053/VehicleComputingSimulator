#ifndef __DRAGGABLE_SQUARE_H__
#define __DRAGGABLE_SQUARE_H__

#include <QLabel>
#include <QMouseEvent>
#include <QWidget>
#include <QPushButton>
#include "process.h"

class DraggableSquare : public QWidget {
    Q_OBJECT

public:
    friend class DraggableSquareTest;
    explicit DraggableSquare(
        QWidget *parent = nullptr,
        const QString &color =
            "QWidget {"
            "  background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
            "    stop: 0 #E0F7F7, stop: 1 #00BFAE);"  // Gradient from mint to cyan
            "  border: 3px solid silver;"             // Silver-colored borders
            "  border-radius: 10px;"                  // Rounded corners
            "}",
        int width = 120, int height = 120);
    DraggableSquare(const DraggableSquare &other);  // Copy constructor
    DraggableSquare &operator=(
        const DraggableSquare &other);  // Copy assignment operator
    void setProcess(Process *process);
    Process *getProcess() const;
    const QPoint getDragStartPosition() const;
    void setDragStartPosition(QPoint dragStartPosition);
    void setSquareColor(const QString &color);
    int getId() const
    {
        return id;
    }
    void setId(int _id)
    {
        id = _id;
    }
    ~DraggableSquare() override;
    void print() const;
    void setStopButtonVisible(bool visible);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPushButton *stopButton;
    QLabel *label;
    Process *process;
    int id;
    bool dragging;
    QPoint dragStartPosition;

private slots:
    void editSquare(int id);
    void deleteSquare(int id);
    void handleStopButtonClicked();
};

#endif  // __DRAGGABLE_SQUARE_H__