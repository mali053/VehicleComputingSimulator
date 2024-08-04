#include "my_widget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QPen>
#include <QBrush>
#include <QPolygon>
#include <QDebug>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


MyWidget::MyWidget(QWidget *parent, int speed) 
    : QWidget(parent), currentPointIndex(0), animationStep(0) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MyWidget::updateDrawing);
    timer->setInterval(speed); // קצב עדכון של 100 מ"ל או לפי הצורך

    // התחלת אנימציה לא פעילה
    isAnimating = false;
}
void MyWidget::addConnection(int xSrc, int ySrc, int xDest, int yDest, const QString &messageId, const QString &status) {
    this->xSrc = xSrc;
    this->ySrc = ySrc;
    this->xDest = xDest;
    this->yDest = yDest;
    this->messageId = messageId;
    this->status = status;

    // ניקוי נקודות ישנות
    points.clear();
    animationStep = 0;
    
    // מחשב נקודות בין הנקודות מקור ויעד לציור קו מקווקו
    int numDashes = 20;
    double deltaX = (xDest - xSrc) / static_cast<double>(numDashes);
    double deltaY = (yDest - ySrc) / static_cast<double>(numDashes);

    for (int i = 0; i <= numDashes; ++i) {
        if (i % 2 == 0) { // קו בכל מקטע
            points.append(QPoint(xSrc + static_cast<int>(i * deltaX), ySrc + static_cast<int>(i * deltaY)));
        }
    }
    points.append(QPoint(xDest, yDest));

    currentPointIndex = 0;
    isAnimating = true;

    timer->start(); // מתחיל את הציור האיטי

    update(); // מעדכן את הווידג'ט לציור הקו
}

void MyWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // הגדרת עט לציור קו עבה ומקווקו
    QPen pen(Qt::black, 4); // קו עבה בצבע שחור
    pen.setStyle(Qt::DashLine); // תבנית מקווקו
    pen.setDashPattern({5, 5}); // תבנית של 10 פיקסלים קו, 5 פיקסלים רווח
    painter.setPen(pen);

    // ציור הקו
    if (!points.isEmpty()) {
        painter.drawPolyline(points.data(), points.size());
    }
}

void MyWidget::updateDrawing() {
    if (isAnimating) {
        animationStep = (animationStep + 1) % 30;  // מניחים 30 שלבים באנימציה
        update(); // גורם לציור מחדש של הווידג'ט
    }
}

void MyWidget::drawArrow(QPainter &painter, const QPoint &src, const QPoint &dst, const QColor &color) {
    QPen pen(color);
    pen.setWidth(2);
    painter.setPen(pen);

    QLine line(src, dst);
    painter.drawLine(line);

    // ציור ראש החץ
    double angle = std::atan2(-line.dy(), line.dx());
    double arrowSize = 10;
    QPointF arrowP1 = dst - QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = dst - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygon arrowHead;
   // arrowHead << dst.toPoint() << arrowP1.toPoint() << arrowP2.toPoint();
    arrowHead << dst << arrowP1.toPoint() << arrowP2.toPoint();

    painter.drawPolygon(arrowHead);
}
