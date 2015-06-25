#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QColor>
#include <QThread>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <stdio.h>
#include <iostream>
#include <inttypes.h>

#include "allpainter.h"
#include "geneticalgorithm.h"
#include "threadworker.h"
#include "graphpainter.h"

using namespace std;

namespace Ui {
class MainWindow;
}

/*struct Farbe
{
    uint8_t r,g,b;
};*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        bool LoadPicture(QStringList files);
        ~MainWindow();
        bool load_newpic(int number); //Lädt das bild an der position number in multiframe_files und returnt: true: alles in ordnung, false: fehler, abbruch

    private slots:
        void on_startbutton_clicked();
        void on_actionBild_speichern_triggered();
        void on_actionSpeichern_triggered();
        void on_generation_spinbox_valueChanged(int);
        void on_inrun_change();//Wenn einer der dynamischen parameter aufgerufen wurde
        void on_snapshot_enabler_clicked(bool checked);
        void on_snapshot_choose_location_clicked();
        void on_generations_button_toggled(bool checked);
        void on_fitness_button_toggled(bool checked);
        void on_time_button_toggled(bool checked);
        void on_generations_button_2_toggled(bool checked);
        void on_fitness_button_2_toggled(bool checked);
        void on_time_button_2_toggled(bool checked);

        void on_multiframe_chooser_clicked();

private:
        Ui::MainWindow *ui;
        AllPainter * painter;
        Farbe ** m_Picture;
        uint m_picsizex,m_picsizey;
        AlgorithmBase * genetic;
        int steps;
        ThreadWorker * worker;
        bool do_snapshot = false;
        bool multiframe = false;
        QStringList multiframe_files;
};

#endif // MAINWINDOW_H
