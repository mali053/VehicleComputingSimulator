#ifndef MY_WIDGET_H
#define MY_WIDGET_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QString>

class MyWidget : public QWidget {
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = nullptr, int speed = 100); // קצב ברירת מחדל הוא 100 מ"ל
    void addConnection(int xSrc, int ySrc, int xDest, int yDest, const QString &messageId, const QString &status);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateDrawing();

private:
    void drawArrow(QPainter &painter, const QPoint &src, const QPoint &dst, const QColor &color);

    QTimer *timer;
    int currentPointIndex;
    int animationStep;
    bool isAnimating;

    int xSrc, ySrc, xDest, yDest;
    QString messageId;
    QString status;
    QVector<QPoint> points;
};

#endif // MY_WIDGET_H
