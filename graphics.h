#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QGraphicsView>

//*********************    DrawWidget    ************************

class DrawWidget : public QGraphicsView
{
    Q_OBJECT
public:
    DrawWidget(QWidget* parent = 0);
    void paintEvent(QPaintEvent *);

};

#endif // GRAPHICS_H
