#include <QApplication>
#include <QCommandLineParser>

#include "SatWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument(
        "cnf File", QCoreApplication::translate("main", "cnf File to solve"));
    QCommandLineOption forceSolve(
        "f", QCoreApplication::translate(
                 "main", "solve graph even if a solved file already exists"));
    parser.addOption(forceSolve);
    QCommandLineOption showSimplified(
        "s", QCoreApplication::translate("main", "visualize simplified graph"));
    parser.addOption(showSimplified);
    parser.process(app);

    SatWindow window;
    window.setForceSolve(parser.isSet(forceSolve));
    window.setShowSimplified(parser.isSet(showSimplified));
    if (parser.positionalArguments().size() > 0) {
        window.setFilename(parser.positionalArguments().at(0).toStdString());
    }

    window.run();
    window.show();

    return app.exec();
}
