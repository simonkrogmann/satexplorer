#include "SelectionSvgWidget.hpp"
#include <QMouseEvent>
#include <QRubberBand>

QPointF normalize(const QPoint &point, const QSize &size) {
    auto x = std::clamp(point.x() / static_cast<float>(size.width()), 0.f, 1.f);
    auto y =
        std::clamp(point.y() / static_cast<float>(size.height()), 0.f, 1.f);
    return {x, y};
}

void SelectionSvgWidget::mousePressEvent(QMouseEvent *event) {
    origin = event->pos();
    if (!rubberBand) rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    rubberBand->setGeometry(QRect(origin, QSize()));
    rubberBand->show();
}

void SelectionSvgWidget::mouseMoveEvent(QMouseEvent *event) {
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
}

void SelectionSvgWidget::mouseReleaseEvent(QMouseEvent *event) {
    rubberBand->hide();
    auto p1 = normalize(origin, size());
    auto p2 = normalize(event->pos(), size());
    emit rectangleDrawn({{std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y())},
                         {std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y())}});
}
