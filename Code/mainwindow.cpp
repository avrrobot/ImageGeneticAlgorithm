#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    painter = new AllPainter(ui->ergebnis_tab);
    painter->setGeometry(ui->picture_left->geometry());
    painter->show();

    setWindowTitle("Genetic Image Evolution");
    steps = 0;

    connect(ui->threadnumber,SIGNAL(valueChanged(int)),this,SLOT(on_inrun_change()));
    connect(ui->fitmode_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(on_inrun_change()));
    connect(ui->mutrate_slider,SIGNAL(valueChanged(int)),this,SLOT(on_inrun_change()));
    connect(ui->mutsize_slider,SIGNAL(valueChanged(int)),this,SLOT(on_inrun_change()));
    connect(ui->xovertype_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(on_inrun_change()));

    ui->snapshot_location->setText(QDir::currentPath());
    ui->multiframe_location->setText(QDir::currentPath());
}
//Rückgabewert: true: war erfolgreich, false: neue datei wählen
bool MainWindow::LoadPicture(QStringList files)
{
    if(files.size() > 1)
    {
        multiframe = true;
        multiframe_files = files;
    }else
        multiframe = false;
    QString filenames = files[0];
    cout <<"Geladene Datei: "<<files[0].toStdString()<<endl;

    cout <<"Bild laden"<<endl;
    //Bild laden, einfügen
    QPixmap pic(filenames);
    if(pic.width() > 600 || pic.height() > 600)
    {
        int ret = QMessageBox::warning(this, tr("My Application"),
                                       "This Image is bigger than recommended (600x600 maximum)\n"
                                       "Still continue?",
                                       QMessageBox::Yes | QMessageBox::No);
        if(ret == QMessageBox::No)
        {
            cout <<"File can be chosen again!"<<endl;
            return false;
        }

    }

    //Die größen der verschidenen objekte anpassen
    QMainWindow::setFixedSize(pic.width()*2+65,pic.height()+150);

    QRect rect = ui->picture_left->geometry();
    rect.setWidth(pic.width());
    rect.setHeight(pic.height());
    ui->picture_left->setGeometry(rect);
    ui->picture_left->setPixmap(pic);
    rect.setRect(rect.x()+rect.width()+20,rect.y(),rect.right()+rect.width()+20,rect.bottom());
    painter->setGeometry(rect);

    //Bild neu laden, Array für Daten initialisieren und Daten des Bildes in Array übertragen
    QImage pict(filenames);
    m_Picture = new Farbe*[pict.width()];
    bool  blackwhite = true;
    for(int x = 0;x<pict.width();x++)
    {
        m_Picture[x] = new Farbe[pict.height()];
        for(int y = 0;y<pict.height();y++)
        {
            QColor farb = pict.pixel(x,y);
            m_Picture[x][y].r = farb.red();
            m_Picture[x][y].b = farb.blue();
            m_Picture[x][y].g = farb.green();
            if(farb.red() != farb.blue() || farb.red() != farb.green())
                blackwhite = false;
        }
    }
    if(blackwhite)
    {
        cout <<"Image is black and white!"<<endl;
        ui->bw_checkbox->setChecked(true);
    }else{
        cout <<"Image is in color!"<<endl;
        ui->bw_checkbox->setChecked(false);
    }
    m_picsizex = pict.width();
    m_picsizey = pict.height();
    cout <<"Imagesize: "<<m_picsizex<<" "<<m_picsizey<<endl;
    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startbutton_clicked()
{
    if(steps == 0)
    {
        ui->startbutton->setText("pause");
        ui->gene_edit->setDisabled(true);
        ui->individual_edit->setDisabled(true);
        ui->type_combo->setDisabled(true);
        ui->bw_checkbox->setDisabled(true);
        ui->spawn_combo->setDisabled(true);
        ui->startsize->setDisabled(true);


        int gsize = uint(ui->gene_edit->text().toInt()),psize = uint(ui->individual_edit->text().toInt());
        cout <<"genes: "<<gsize<<endl;
        cout <<"individuals: "<<psize<<endl;
        QString form = ui->type_combo->currentText();
        cout <<"type: "<<form.toStdString()<<endl;
        if(form == "Circle")
            genetic = new GeneticAlgorithm<CircleData>(gsize,psize,m_Picture,m_picsizex,m_picsizey);
        else if(form == "Ellipse")
            genetic = new GeneticAlgorithm<EllipseData>(gsize,psize,m_Picture,m_picsizex,m_picsizey);
        else if(form == "Triangle")
            genetic = new GeneticAlgorithm<TriangleData>(gsize,psize,m_Picture,m_picsizex,m_picsizey);
        else
        {
            cout <<"this shape can not be taken!"<<endl;
            return;
        }
        cout <<"Black and white: "<<ui->bw_checkbox->isChecked()<<endl;
        //den rotkanal auf den durchschnitt des rgb raumes setzen
        if(ui->bw_checkbox->isChecked())
        {
            for(uint x = 0;x<m_picsizex;x++)
            {
                for(uint y = 0;y<m_picsizey;y++)
                {
                    m_Picture[x][y].r = (m_Picture[x][y].r+m_Picture[x][y].g+m_Picture[x][y].b)/3;
                }
            }
        }
        cout <<"startsize: "<<ui->startsize->value()<<endl;
        int spawn = ui->spawn_combo->currentIndex();
        //cout <<"Spawnverhalten ist: "<<ui->spawn_combo->currentText().toStdString()<<endl;
        genetic->init_variables(ui->bw_checkbox->isChecked(),ui->startsize->value(),spawn,ui->threadnumber->value(),ui->fitmode_combo->currentIndex(),
                                ui->mutsize_slider->value(),ui->mutrate_slider->value(),ui->xovertype_combo->currentIndex());
        painter->init(genetic);

        QThread* thread = new QThread;
        nextframecriterion cr;
        cr.save_directory = ui->multiframe_location->text();
        cr.multiframe = multiframe;
        cr.framecounter = multiframe_files.size();
        criterion startcr;
        startcr.fitness = ui->fitness_button->isChecked() ? ui->fitness_spinbox->value() : -1;
        startcr.generations = ui->generations_button->isChecked() ? ui->generations_spinbox->value() : -1;
        startcr.time = ui->time_button->isChecked();
        startcr.stoptime = ui->time_timeedit->time();
        cr.startframe = startcr;
        criterion allcr;
        allcr.fitness = ui->fitness_button_2->isChecked() ? ui->fitness_spinbox_2->value() : -1;
        allcr.generations = ui->generations_button_2->isChecked() ? ui->generations_spinbox_2->value() : -1;
        allcr.time = ui->time_button_2->isChecked();
        allcr.stoptime = ui->time_timeedit_2->time();
        cr.allframe = allcr;

        worker = new ThreadWorker(genetic,painter,(GraphPainter*)ui->graph,ui->info_label,ui->generation_spinbox->value(),
                                  do_snapshot ? ui->snapshot_spinbox->value() : 0,ui->snapshot_location->text(),m_picsizex,m_picsizey,cr,this,
                                  ui->reset_progress_box->isChecked());

        worker->moveToThread(thread);
        //connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(thread, SIGNAL(started()), worker, SLOT(process()));
        connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
    }else//Der Thread läuft bereits
    {
        //Den Arbeitsthread pausieren
        worker->sleep_acess.lock();
        worker->do_sleep ^= 1;//Den Zustand umdrehen
        worker->sleep_acess.unlock();

        //Alle Anzeigeobjekte auf den aktuellen Stand bringen
        ui->startbutton->setText(!(steps%2) ? "Pausieren" : "Laufen");
    }

    steps++;
}

bool MainWindow::load_newpic(int number)
{
    if(number >= multiframe_files.size())
    {
        cout <<"Arrived at the last frame!"<<endl;
        return false;
    }
    cout <<"Load new frame: "<<multiframe_files[number].toStdString()<<endl;
    QPixmap pic(multiframe_files[number]);
    if(pic.width() != (int)m_picsizex || pic.height() != (int)m_picsizey)
    {
        cout <<"Image size ist different!"<<endl;
        //Eventuell noch eine messagebox ausgeben
        return false;
    }
    ui->picture_left->setPixmap(pic);

    QImage pict(multiframe_files[number]);
    for(uint x = 0;x<m_picsizex;x++)
    {
        for(uint y = 0;y<m_picsizey;y++)
        {
            QColor farb = pict.pixel(x,y);
            m_Picture[x][y].r = farb.red();
            m_Picture[x][y].b = farb.blue();
            m_Picture[x][y].g = farb.green();
        }
    }
    return true;
}

void MainWindow::on_actionBild_speichern_triggered()
{
    cout <<"save result!"<<endl;
    //Speicherort wählen lassen
    QString filename = QFileDialog::getSaveFileName(this,"choose directory", "", "image files(*.png *.jpg *.bmp)");
    if(filename.toStdString().length() == 0)
    {
        cout <<"No directory chosen!"<<endl;
        return;
    }
    //Bild speichern
    QImage * image = new QImage(m_picsizex,m_picsizey,QImage::Format_RGB32);
    genetic->paint_best(image);
    image->save(filename);
}

void MainWindow::on_actionSpeichern_triggered()
{
    cout <<"save evolution!"<<endl;
    //Speicherort wählen lassen
    QString filename = QFileDialog::getSaveFileName(this,"choose directory", "", "evolutiondata(*.evo)");
    if(filename.toStdString().length() == 0)
    {
        cout <<"No directory chosen!"<<endl;
        return;
    }
    //Evolution speichern
    genetic->save_evolution(filename);
}

void MainWindow::on_generation_spinbox_valueChanged(int anz_gen)
{
    if(!steps)
        return;
    worker->gen_zeich = anz_gen;
}

void MainWindow::on_inrun_change()//Einer der Parameter, die während dem laufen verändert werden dürfen, würde verändert
{
    if(!steps)
        return;
    genetic->set_extra_variables(ui->threadnumber->value(),ui->fitmode_combo->currentIndex(),ui->mutsize_slider->value(),ui->mutrate_slider->value(),
                                 ui->xovertype_combo->currentIndex());
}

void MainWindow::on_snapshot_enabler_clicked(bool checked)
{
    ui->snapshot_choose_location->setEnabled(checked);
    ui->snapshot_enabler->setEnabled(checked);
    ui->snapshot_location->setEnabled(checked);
    ui->snapshot_spinbox->setEnabled(checked);
    do_snapshot = checked;
}

void MainWindow::on_snapshot_choose_location_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "saving directory for image sequence",
                                                     ui->snapshot_location->text(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    ui->snapshot_location->setText(dir);
}

void MainWindow::on_multiframe_chooser_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "saving directory for image sequence",
                                                     ui->multiframe_location->text(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    ui->multiframe_location->setText(dir);
}

void MainWindow::on_generations_button_toggled(bool checked)
{
    bool one_checked = ui->fitness_button->isChecked() || ui->time_button->isChecked() || ui->generations_button->isChecked();
    if(!one_checked)
    {
        ui->generations_button->setChecked(true);
    }else
    {
        ui->generations_spinbox->setEnabled(checked);
    }
}

void MainWindow::on_fitness_button_toggled(bool checked)
{
    bool one_checked = ui->fitness_button->isChecked() || ui->time_button->isChecked() || ui->generations_button->isChecked();
    if(!one_checked)
    {
        ui->fitness_button->setChecked(true);
    }else
    {
        ui->fitness_spinbox->setEnabled(checked);
    }
}

void MainWindow::on_time_button_toggled(bool checked)
{
    bool one_checked = ui->fitness_button->isChecked() || ui->time_button->isChecked() || ui->generations_button->isChecked();
    if(!one_checked)
    {
        ui->time_button->setChecked(true);
    }else
    {
        ui->time_timeedit->setEnabled(checked);
    }
}

void MainWindow::on_generations_button_2_toggled(bool checked)
{
    bool one_checked = ui->fitness_button_2->isChecked() || ui->time_button_2->isChecked() || ui->generations_button_2->isChecked();
    if(!one_checked)
    {
        ui->generations_button_2->setChecked(true);
    }else
    {
        ui->generations_spinbox_2->setEnabled(checked);
    }
}

void MainWindow::on_fitness_button_2_toggled(bool checked)
{
    bool one_checked = ui->fitness_button_2->isChecked() || ui->time_button_2->isChecked() || ui->generations_button_2->isChecked();
    if(!one_checked)
    {
        ui->fitness_button_2->setChecked(true);
    }else
    {
        ui->fitness_spinbox_2->setEnabled(checked);
    }
}

void MainWindow::on_time_button_2_toggled(bool checked)
{
    bool one_checked = ui->fitness_button_2->isChecked() || ui->time_button_2->isChecked() || ui->generations_button_2->isChecked();
    if(!one_checked)
    {
        ui->time_button_2->setChecked(true);
    }else
    {
        ui->time_timeedit_2->setEnabled(checked);
    }
}
