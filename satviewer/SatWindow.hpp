#pragma once

#include <QApplication>
#include <QIntValidator>
#include <QLineEdit>
#include <QMainWindow>
#include <QToolBar>

#include "SelectionSvgWidget.hpp"
#include "Stepper.hpp"

class ogdfWrapper;
class QLineEdit;
class QIntValidator;
class QScrollArea;
class QScrollBar;

/*
    Manages main window, SVG renderer, scrolling and toolbar buttons
    Initializes Stepper and calls toolbar functionality on it.
*/
class SatWindow : public QMainWindow {
    Q_OBJECT
public:
    void run();

    explicit SatWindow(QWidget* parent = 0);
    virtual ~SatWindow() = default;

    void setFilename(const std::string& filename);
    void setSolverOptions(const SolverOptions& options);

protected:
    SelectionSvgWidget* m_svgWidget;
    QScrollArea* m_scrollArea;
    Stepper m_stepper;
    QToolBar m_toolbar;
    double m_scaleFactor;
    SolverOptions m_solverOptions;

    void endOfTrace(bool eof);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar* scrollBar, double factor);
    void reloadSvg();
    void exportToPNG(const std::string& filename);
    void setInitialWindowSize(QSize imageSize);
    // These pointers belong to the QToolbar and are its responsibility
    QAction* m_stepAction;
    QAction* m_conflictAction;
    QAction* m_branchAction;
    QAction* m_restartAction;
    QAction* m_lastRestartAction;
    std::string m_filename;

    QLineEdit m_cullBox;
    QIntValidator m_validator;

private slots:
    void handleStepButton();
    void handleBranchButton();
    void handleConflictButton();
    void handleRestartButton();
    void handleLastRestartButton();
    void handleCullInput();
    void handleRelayoutButton();
    void handleShowAllButton();
    void handleExportButton();
    void zoomIn();
    void zoomOut();
    void cluster();
    void handleRectangleDrawn(TwoPoints rectangle, bool set);
};
