/**
 * This MainWindow class adds functionality to the basic UI in Ui::MainWindow
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "veikkparms.h"
#include <QMainWindow>
#include <QGraphicsView>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QScreen>
#include <QCheckBox>
#include <QComboBox>

// TODO: remove
#include <QDebug>

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
    QScreen *screen;

    // configuration data; current is synced with UI, restoreParms is the last
    // saved configuration
    VeikkParms currentParms{}, restoreParms{};

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
    QComboBox *screenOrientation, *pressureMapDefaults;

    // handlers
    void showEvent(QShowEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

    // composite parameter value getters
    void getPressureCoefs(qint16 *coefs);
    QRect getScreenMapParms();

public slots:
    void screenSizeChanged(QRect newScreenSize);
    void tabChanged(int curTab);
    void updatePressureForm(qint16 *newCoefs);
    void updatePressureCoefs();
    void updateScreenMapForm(QRect newScreenMap);
    void updateScreenMapParms();
    void setDefaultScreenMap(int checkState);
    void setComboBoxDefaultPressure();
    void resetScreenChanges();
    void resetPressureChanges();
    void loadParmsFromSysconfig();
    void updateUiFromParms();
    void launchDialog(QString text, bool isModal);

signals:
    void updatePressureCurve(qint16 *newCoefs);
    void updateScreenMapRect(QRect newScreenMap);
};
#endif // MAINWINDOW_H
