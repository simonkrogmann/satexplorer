#include <QApplication>
#include<QSvgWidget>
#include<QFileDialog>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSvgWidget* window = new QSvgWidget();
    QString path = QFileDialog::getOpenFileName(window, "Open Image", "/home/", "Image Files (*.svg)");
    window->load(path);
    window->show();

    return a.exec();
}

