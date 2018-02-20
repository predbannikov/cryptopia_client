#include "graphics.h"

DrawWidget::DrawWidget(QWidget *parent):
QGraphicsView(parent)
{

}

void DrawWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::green);
    painter.setFont(QFont("Arial", this->rect().height()/3));
    painter.drawText(this->rect(), Qt::AlignCenter, "Crypto bot");

    QLineF line(10.0, 80.0, 90.0, 20.0);

    painter.drawLine(line);

}
