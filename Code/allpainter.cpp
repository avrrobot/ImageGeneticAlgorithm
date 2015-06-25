#include "allpainter.h"

#include <QPainter>
#include <inttypes.h>

AllPainter::AllPainter(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    is_init = 0;
}

void AllPainter::init(AlgorithmBase * algo)
{
    genetic = algo;
    is_init = 1;
}

QSize AllPainter::minimumSizeHint() const
{
    return QSize(200,200);
}

QSize AllPainter::sizeHint() const
{
    return QSize(200,200);
}

void AllPainter::paintEvent(QPaintEvent */* event*/)
{
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing,false);

    //painter.drawRect(QRect(0,0,width()-1,height()-1));
    if(is_init)
        genetic->paint_best(&painter);
    else
    {
        for(int i = 0;i<10;i++)
        {
            QPoint pos(rand()%200,rand()%200);
            uint8_t radiusx = rand()%20;
            uint8_t radiusy = rand()%20;
            painter.drawEllipse(pos,radiusx,radiusy);
        }
   }
}
