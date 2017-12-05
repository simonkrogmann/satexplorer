#include <QApplication>
#include <QSvgWidget>
#include <QFileDialog>

#include "SatWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SatWindow window;
    if (argc >= 2)
    {
        window.setFilename(argv[1]);
    }
    window.run();
    window.show();

    return a.exec();
}

