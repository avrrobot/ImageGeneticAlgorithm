#include "graphpainter.h"

#include <QPainter>

GraphPainter::GraphPainter(QWidget *parent) : QWidget(parent)
{
    maximum = 0;
    minimum = 0;
}

QSize GraphPainter::minimumSizeHint() const
{
    return QSize(30,30);
}

QSize GraphPainter::sizeHint() const
{
    return QSize(1000,1000);
}

void GraphPainter::paintEvent(QPaintEvent */* event*/)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing,true);

    painter.drawRect(QRect(0,0,width()-1,height()-1));
    if(points.size() == 0 || maximum == minimum)
        return;
    QLine lines[points.size()-1];
    //cout <<"Anzahl der Linien: "<<points.size()-1<<endl;
    for(uint i = 0;i<points.size()-1;i++)//Eventuell nicht jede linie, sondern nur jede zweite... zeichnen
    {
        lines[i].setP1(QPoint(i*width()/points.size(),height()-height()*points[i]/(maximum-minimum)));
        lines[i].setP2(QPoint((i+1)*width()/points.size(),height()-height()*points[i+1]/(maximum-minimum)));
    }
    painter.drawLines(lines,points.size()-1);
}

void GraphPainter::add_point(uint64_t value)
{
    //cout <<"Punkt hinzugefügt: "<<value<<endl;
    points.push_back(value);
    if((int)value > maximum)
        maximum = value;
    if((int)value < minimum)
        minimum = value;
}

bool GraphPainter::change_point(uint position,uint64_t value)
{
    if(position >= points.size())
        return false;
    points[position] = value;
    if((int)value > maximum)
        maximum = value;
    if((int)value < minimum)
        minimum = value;
    return true;
}
