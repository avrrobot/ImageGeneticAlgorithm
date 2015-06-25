#ifndef GRAPHPAINTER_H
#define GRAPHPAINTER_H

#include <QWidget>
#include <vector>
#include <iostream>
#include <inttypes.h>

using namespace std;
typedef unsigned uint;

class GraphPainter : public QWidget
{
    Q_OBJECT
    public:
        GraphPainter(QWidget *parent = 0);
        QSize minimumSizeHint() const;
        QSize sizeHint() const;
        //Warnung: Fehler bei negativen Zahlen
        void add_point(uint64_t value);
        bool change_point(uint position,uint64_t value);
    protected:
        void paintEvent(QPaintEvent *event);
    private:
        vector<uint64_t> points;
        int maximum,minimum;
};

#endif // GRAPHPAINTER_H
