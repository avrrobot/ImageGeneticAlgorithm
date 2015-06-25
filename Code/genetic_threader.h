#ifndef GENETIC_THREADER_H
#define GENETIC_THREADER_H

#include <QObject>
#include "geneticalgorithm.h"

class AlgorithmBase;//Prototype

class genetic_threader : public QObject
{
    Q_OBJECT
    public:
        explicit genetic_threader(AlgorithmBase *,QObject *parent = 0);

    public slots:
        void process();

    signals:
        void finished();
        void error(QString err);

    private:
        AlgorithmBase * genetic;

};

#endif // GENETIC_THREADER_H
