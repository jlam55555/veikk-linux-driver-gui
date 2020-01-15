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

// TODO: remove
#include <QDebug>

// supported modparm types -- may not line up with
// enum veikk_modparms from the driver
typedef enum {
    VEIKK_MP_SCREEN_MAP		= 0x1,
    VEIKK_MP_SCREEN_SIZE 	= 0x2,
    VEIKK_MP_ORIENTATION 	= 0x4,
    VEIKK_MP_PRESSURE_MAP 	= 0x8,
    VEIKK_MP_SCREEN			= 0x7,
    VEIKK_MP_ALL			= 0xf
} ModparmType;

// used to store an entire configuration, in a format easily translatable
// to/from Qt front-end and export/import config files; also used to save and
// restore configurations
class VeikkParms {
public:
    // default constructor sets defaults; defaults outlined in driver
    VeikkParms();

    void restoreConfig(VeikkParms &vp);

    void setPressureMap(qint16 *newCoefs);
    void setScreenMap(QRect newScreenMap);
    void setScreenSize(QRect newScreenSize);

    int applyConfig(ModparmType type);
    int exportConfig(QString dest);

private:
    quint16 screenSize[2];
    quint16 screenMap[4];
    quint32 orientation;
    qint16 pressureMap[4];

    int setSysfsModparm(QString parmName, QString value);
    quint32 screenSizeExport();
    quint64 screenMapExport();
    quint32 orientationExport();
    quint64 pressureMapExport();
};

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

    // configuration data
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
    QComboBox *screenOrientation;

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

signals:
    void updatePressureCurve(qint16 *newCoefs);
    void updateScreenMapRect(QRect newScreenMap);
};
#endif // MAINWINDOW_H
