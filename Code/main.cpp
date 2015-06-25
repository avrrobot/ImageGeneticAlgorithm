#include "mainwindow.h"
#include <QApplication>
#include <QFileDialog>

#include "geneticalgorithm.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QStringList fileNames;
    /*QMessageBox box;
    box.setText("Welcher Modus soll verwendet werden?");
    QPushButton * singlebutton = box.addButton("Einzelbild",QMessageBox::ActionRole);
    QPushButton * multibutton = box.addButton("Videoframes",QMessageBox::ActionRole);
    QPushButton * abortbutton = box.addButton(QMessageBox::Cancel);
    box.exec();
    if(box.clickedButton() == singlebutton)
    {

    }else if(box.clickedButton() == multibutton)
    {

    }else
    {
        return 0;
    }*/
    do{
        //filename = QFileDialog::getOpenFileName(NULL,"Open Image", "", "Image Files (*.png *.jpg *.bmp)");
        QFileDialog dialog(nullptr);
        dialog.setFileMode(QFileDialog::ExistingFiles);
        dialog.setNameFilter("Image Files (*.png *.jpg *.bmp)");
        if (dialog.exec())
            fileNames = dialog.selectedFiles();
        if(fileNames.size() == 0)
        {
            cout <<"Keine Datei ausgewählt, Programm wird beendet!"<<endl;
            return -1;
        }
    }while(!w.LoadPicture(fileNames));

    w.show();

    return a.exec();
}
