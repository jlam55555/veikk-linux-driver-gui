/**
 * This MainWindow class adds functionality to the basic UI in Ui::MainWindow
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QDoubleSpinBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    // relevant elements
    QTabWidget *tabWidget;
    QGraphicsView *pressureCurveView, *screenMapView;

    // control elements
    QDoubleSpinBox *pressureCoefSpinboxes[4];

    // handlers
    void showEvent(QShowEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

public slots:
    void tabChanged(int curTab);
    void updatePressureForm(qint16 *newCoefs);
    void updatePressureCoefs();

signals:
    void updatePressureCurve(qint16 *newCoefs);
};
#endif // MAINWINDOW_H
