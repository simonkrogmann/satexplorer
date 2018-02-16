#pragma once
#include <QSvgWidget>

class QRubberBand;

struct TwoPoints {
    QPointF p1;
    QPointF p2;
};

/*
    expands the svg widget with the ability to draw a rectangle and send its
   coordinates to the managing window
*/
class SelectionSvgWidget : public QSvgWidget {
    Q_OBJECT
public:
    explicit SelectionSvgWidget(QWidget *parent = 0)
        : QSvgWidget(parent), rubberBand(0) {}

signals:
    void rectangleDrawn(TwoPoints rectangle, bool set);

private:
    QRubberBand *rubberBand;
    QPoint origin;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};
