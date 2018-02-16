#include <QApplication>
#include <QCommandLineParser>

#include "SatWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument(
        "cnf File", QCoreApplication::translate("main", "cnf File to solve"));
    QCommandLineOption forceRecomputation(
        "f", QCoreApplication::translate(
                 "main", "solve graph even if a solved file already exists"));
    parser.addOption(forceRecomputation);
    QCommandLineOption simplified(
        "s", QCoreApplication::translate("main", "visualize simplified graph"));
    parser.addOption(simplified);
    QCommandLineOption onlyImplications(
        "i", QCoreApplication::translate("main", "show implication graph"));
    parser.addOption(onlyImplications);
    parser.process(app);

    SolverOptions options;
    options.forceRecomputation = parser.isSet(forceRecomputation);
    options.onlyImplications = parser.isSet(onlyImplications);
    options.simplified = parser.isSet(simplified);

    SatWindow window;
    window.setSolverOptions(options);
    if (parser.positionalArguments().size() > 0) {
        window.setFilename(parser.positionalArguments().at(0).toStdString());
    }

    window.run();
    window.show();

    return app.exec();
}
