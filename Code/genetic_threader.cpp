#include "genetic_threader.h"

genetic_threader::genetic_threader(AlgorithmBase * base,QObject *parent) :
    QObject(parent), genetic(base)
{
}

void genetic_threader::process()
{
    genetic->working_thread();
    emit finished();
}
