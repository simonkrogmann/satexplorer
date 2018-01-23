#include <QApplication>
#include <QCommandLineParser>

#include "SatWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument(
        "cnf File", QCoreApplication::translate("main", "cnf File to solve"));
    QCommandLineOption forceSolve(
        "f", QCoreApplication::translate(
                 "main", "solve graph even if a solved file already exists"));
    parser.addOption(forceSolve);
    parser.process(a);

    SatWindow window;
    window.setForceSolve(parser.isSet(forceSolve));
    if (parser.positionalArguments().size() > 0) {
        window.setFilename(parser.positionalArguments().at(0).toStdString());
    }

    window.run();
    window.show();

    return a.exec();
}
