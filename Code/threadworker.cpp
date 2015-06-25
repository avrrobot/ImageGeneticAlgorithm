#include "mainwindow.h"
#include "threadworker.h"
#include <QCoreApplication>

void ThreadWorker::process()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    int generation = 0;
    while(1)
    {
        uint64_t fit = 0;
        for(int i = 0;i<gen_zeich;i++)
        {
            fit = genetic->generation();
            cout <<"generation "<<generation++<<" has fitness: "<<fit<<endl;
            if(criterions.multiframe)
            {
                bool nextframe = false;
                if(framecount == 0)
                {
                    if(generation >= criterions.startframe.generations)
                        nextframe = true;
                    if((int)fit <= criterions.startframe.fitness && criterions.startframe.fitness != -1)
                        nextframe = true;
                    //Noch die Zeit einfügen
                }else
                {
                    if(generation >= criterions.allframe.generations)
                        nextframe = true;
                    if((int)fit <= criterions.allframe.fitness && criterions.allframe.fitness != -1)
                        nextframe = true;
                    //Noch die Zeit einfügen
                }
                if(nextframe)
                {
                    //save image
                    QString filename = QString("%1/snapshot%2.png").arg(criterions.save_directory).arg(framecount);
                    QImage * image = new QImage(px,py,QImage::Format_RGB32);
                    genetic->paint_best(image);
                    image->save(filename);
                    delete image;

                    if(reset_frame)
                    {
                        genetic->reset_progres();
                    }

                    framecount++;
                    generation = 0;
                    //Lade das neue Bild in den verwendeten pointer
                    if(caller->load_newpic(framecount) == false) //Fehler beim laden
                    {
                        emit finished();
                        return;
                    }
                }
            }
            QString labeltext = QString("generation %1 with fitness %2").arg(generation).arg(fit);
            if(criterions.multiframe)
            {
                labeltext.append(QString(" at frame %1").arg(framecount));
            }
            label->setText(labeltext);
            graph->add_point(fit);
            graph->update();

            //Wartet so lange, wie do_sleep auf true ist
            sleep_acess.lock();
            if(do_sleep)
            {
                painter->update();
            }
            while(do_sleep)
            {
                sleep_acess.unlock();
                QThread::sleep(1);
                sleep_acess.lock();
            }
            sleep_acess.unlock();

            //Speichert ein Snapshot, falls die generation passt
            if(snapshot_time && generation%snapshot_time == 0)
            {
                QString filename = QString("%1/snapshot%2.png").arg(snapshot_location).arg(generation/snapshot_time);
                QImage * image = new QImage(px,py,QImage::Format_RGB32);
                genetic->paint_best(image);
                image->save(filename);
                delete image;
            }
        }
        painter->update();

    }
    emit finished();
}
