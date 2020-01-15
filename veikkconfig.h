/**
 * This MainWindow class adds functionality to the basic UI in Ui::MainWindow
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QScreen>
#include <QCheckBox>
#include <QComboBox>

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
    QSpinBox *screenMapXSpinBox, *screenMapYSpinBox,
             *screenMapWidthSpinBox, *screenMapHeightSpinBox,
             *screenMapSpinboxes[4];
    QLineEdit *screenWidthLineEdit, *screenHeightLineEdit;
    QCheckBox *screenDefaultMap;
    QComboBox *screenOrientation;

    // handlers
    void showEvent(QShowEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

    // other
    QScreen *screen;

public slots:
    void screenSizeChanged(QRect newScreenSize);
    void tabChanged(int curTab);
    void updatePressureForm(qint16 *newCoefs);
    void getPressureCoefs(qint16 *coefs);
    void updatePressureCoefs();
    void updateScreenMapForm(QRect newScreenMap);
    QRect getScreenMapParms();
    void updateScreenMapParms();
    void setDefaultScreenMap(int checkState);

    void exportConfig();

signals:
    void updatePressureCurve(qint16 *newCoefs);
    void updateScreenMapRect(QRect newScreenMap);
};
#endif // MAINWINDOW_H
