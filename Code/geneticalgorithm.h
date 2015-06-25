#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>

#include <QMutex>
#include <QPainter>
#include <QImage>
#include <QThread>
#include <QObject>
//#include <QElapsedTimer>//Zu Debugzwecken

#include "genetic_threader.h"

using namespace std;

typedef unsigned uint;

struct Farbe
{
    uint8_t r,g,b;
};

struct TriangleData
{
    uint16_t x1,y1,x2,y2,x3,y3;
    uint8_t colr,colg,colb;
};
struct CircleData
{
    uint16_t midx,midy;
    uint8_t radius;
    uint8_t colr,colg,colb;
};
struct EllipseData
{
    uint16_t midx,midy;
    uint8_t radiusx,radiusy;
    uint8_t colr,colg,colb;
};

class AlgorithmBase
{
    public:
        AlgorithmBase(){};
        virtual int generation()=0;
        virtual void paint_best(QPainter*)=0;
        virtual void paint_best(QImage*)=0;
        virtual void init_variables(bool,int=15,int=0,uint=1,uint=0,uint=10,uint=60,uint=0)=0;
        virtual void save_evolution(QString)=0;
        virtual void load_evolution(QString)=0;
        virtual void working_thread()=0;
        virtual void set_extra_variables(uint,uint,uint,uint,uint)=0;
        virtual void reset_progres()=0;
};

template<class T>
class GeneticAlgorithm : public AlgorithmBase, public QObject
{
    public:
        GeneticAlgorithm(uint,uint,Farbe **,uint,uint);
        ~GeneticAlgorithm();
        int generation();
        void paint_best(QPainter*);
        void paint_best(QImage*);
        void init_variables(bool bw,int=15,int=0,uint=1,uint=0,uint=10,uint=60,uint=0);
        void set_extra_variables(uint,uint,uint,uint,uint);
        void save_evolution(QString);
        void load_evolution(QString);
        void working_thread();
        void reset_progres();
    private:
        uint64_t fitness(uint,Farbe **,uint);
        void paint(uint,uint,Farbe **);
        void SWAP(int& a, int& b);//Hilfsfunktion zum zeichnen der Dreiecke
        void row(uint,uint,uint,Farbe,Farbe **);//Hilfsfunktion  zum zeichnen von Ellipsen
        void change_position(uint,uint,uint);
        void mutate_single(uint,uint);
        void bestpic_to_act();
        void mutate_all();
        void random_assign(uint,int);

        struct individual
        {
            T * gene;
        }*m_individuals;
        uint m_populationsize;
        uint m_genomesize;
        uint picsizex,picsizey;
        Farbe ** pic_compare;
        Farbe ** render_pic;
        Farbe ** best_pic;
        QMutex bestpic_action;
        //Ab hier für das Multithreaden der generation()
        QMutex number_action;
        int actnum;//Aktueller Stand der Berechnung
        uint64_t * fitn;//Die Liste der Fitnesswerte
        volatile uint ready;//Anzahl der Threads, die fertig sind
        //Ab hier verschiedene kleinere Einstellungen
        bool blackwhite;
        int startsize;
        uint threadnum;
        uint fitnessmode;//Ob die Fitnessfunktion linear(0) oder quadratisch(1) ist
        uint mutsize;//Die stärke der mutationen
        uint mutrate;//Die Anzahl der Mutationen
        uint varbuffer[5];//Enthält Einstellungen temporär
        bool varbuffer_changed;//Ob diese Einstellungen geändert wurden
        bool spawnmode; //ob in der mitte oder verteilt gespawnt wird
        QMutex varbuffer_action;//Zugriff auf die varbuffer einstellungen
        uint xovertype;//Typ des Crossovers: 0->Standard, 1->Uniform, 2->Gemischt
};


#endif // GENETICALGORITHM_H
