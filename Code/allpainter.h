#ifndef ALLPAINTER_H
#define ALLPAINTER_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>

#include "geneticalgorithm.h"

class AllPainter : public QWidget
{
    Q_OBJECT
    public:
        AllPainter(QWidget *parent = 0);
        QSize minimumSizeHint() const;
        QSize sizeHint() const;
        void init(AlgorithmBase *);
    protected:
        void paintEvent(QPaintEvent *event);
    private:
        AlgorithmBase * genetic;
        bool is_init;
};

#endif // ALLPAINTER_H
