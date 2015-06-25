#ifndef THREADWORKER_H
#define THREADWORKER_H

#include "allpainter.h"
#include "geneticalgorithm.h"
#include "graphpainter.h"
//#include "mainwindow.h"
#include <QLabel>
#include <QMutex>
#include <QThread>
#include <QTime>

class MainWindow;

struct criterion
{
    int generations = -1; //wenn verwendet: -1, sonst generation zum stoppen
    int fitness = -1; //das gleiche wie oben mit fitness
    bool time = false;
    QTime stoptime;
};

struct nextframecriterion
{
    bool multiframe = false;
    QString save_directory;
    int framecounter;
    criterion startframe;
    criterion allframe;
};

class ThreadWorker : public QObject
{
    Q_OBJECT
    public:
        ThreadWorker(AlgorithmBase * base,AllPainter * paint,GraphPainter * g,QLabel * l,int gz,int snaptime,QString snaploc,uint x,uint y,
                     nextframecriterion cr, MainWindow * cal, bool reset) : gen_zeich(gz), genetic(base), painter(paint), graph(g), caller(cal),
                    label(l), snapshot_time(snaptime), snapshot_location(snaploc),px(x),py(y), criterions(cr), reset_frame(reset)
        {
            //An dieser Stelle muss der genetische Algorithmus initialisiert sein
            do_sleep = false;
        }
        QMutex sleep_acess;//Ob auf die Variable do_sleep  zugegriffen werden darf
        bool do_sleep;//Ob der Thread gerade pausieren soll
        //Sonstige einstellungen
        int gen_zeich;//Alle wie viele generationen gezeichnet werden soll
    public slots:
        void process();
    signals:
        void finished();
        void error(QString err);
    private:
        AlgorithmBase * genetic;
        AllPainter * painter;
        GraphPainter * graph;
        MainWindow * caller;
        QLabel * label;
        int snapshot_time;
        QString snapshot_location;
        uint px,py;
        nextframecriterion criterions;
        uint framecount = 0;
        bool reset_frame;
};

#endif // THREADWORKER_H
