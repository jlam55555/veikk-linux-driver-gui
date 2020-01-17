#include "veikkconfig.h"
#include <QFile>

VeikkParms::VeikkParms()
    : screenSize{0, 0}, screenMap{0, 0, 0, 0},
      orientation{0}, pressureMap{0, 100, 0, 0} { }

// used to restore current config to restore point or copy config
void VeikkParms::restoreConfig(VeikkParms *restoreParms, int parms) {
    if(parms & VEIKK_MP_SCREEN_SIZE)
        screenSize = restoreParms->screenSize;
    if(parms & VEIKK_MP_SCREEN_MAP)
        screenMap = restoreParms->screenMap;
    if(parms & VEIKK_MP_ORIENTATION)
        orientation = restoreParms->orientation;
    if(parms & VEIKK_MP_PRESSURE_MAP)
        pressureMap = restoreParms->pressureMap;
}

// data setters -- parse from Q-types
void VeikkParms::setScreenSize(QRect newScreenSize) {
    screenSize.width = quint16(newScreenSize.width());
    screenSize.height = quint16(newScreenSize.height());
}
void VeikkParms::setScreenMap(QRect newScreenMap) {
    screenMap.x = quint16(newScreenMap.x());
    screenMap.y = quint16(newScreenMap.y());
    screenMap.width = quint16(newScreenMap.width());
    screenMap.height = quint16(newScreenMap.height());
}
void VeikkParms::setOrientation(quint32 newOrientation) {
    orientation = newOrientation;
}
void VeikkParms::setPressureMap(qint16 *newCoefs) {
    pressureMap = *reinterpret_cast<struct serializablePressureMap *>(newCoefs);
}

// data getters -- output as Q-types
QSize VeikkParms::getScreenSize() {
    return QSize{screenSize.width, screenSize.height};
}
QRect VeikkParms::getScreenMap() {
    return QRect{screenMap.x, screenMap.y, screenMap.width, screenMap.height};
}
quint32 VeikkParms::getOrientation() {
    return orientation;
}
void VeikkParms::getPressureMap(qint16 *coefs) {
    coefs[0] = pressureMap.a0;
    coefs[1] = pressureMap.a1;
    coefs[2] = pressureMap.a2;
    coefs[3] = pressureMap.a3;
}
bool VeikkParms::isInvalidScreenMap() {
    return screenMap.width<=0 || screenMap.height<=0;
}

// applies config to sysfs
// TODO: error checking
int VeikkParms::applyConfig(VeikkParms *restoreParms, int parms) {
    if(parms & VEIKK_MP_SCREEN_SIZE)
        setSysfsModparm("screen_size", QString::number(serializeScreenSize()));
    if(parms & VEIKK_MP_SCREEN_MAP)
        setSysfsModparm("screen_map", QString::number(serializeScreenMap()));
    if(parms & VEIKK_MP_ORIENTATION)
        setSysfsModparm("orientation", QString::number(serializeOrientation()));
    if(parms & VEIKK_MP_PRESSURE_MAP)
        setSysfsModparm("pressure_map",
                        QString::number(serializePressureMap()));
    // save to restoreParms
    restoreParms->restoreConfig(this, parms);
    return 0;
}

// exports config to conf file
int VeikkParms::exportConfig(QString dest) {
    // TODO: implement this
    return -1;
}

// TODO: error checking
int VeikkParms::setSysfsModparm(QString parmName, QString value) {
    const QString baseUrl = "/sys/module/veikk/parameters/";
    QFile parmFile{baseUrl + parmName};

    if(!parmFile.open(QIODevice::WriteOnly|QIODevice::Text))
        return errno;
    if(parmFile.write(value.toLocal8Bit().constData())<0)
        return errno;
    return 0;
}

// static function for storing default pressure maps in qcombobox; equivalent
// conversion as in serializePressureMap() (non-static version)
quint64 VeikkParms::serializePressureMap(qint16 a0, qint16 a1, qint16 a2,
                                         qint16 a3) {
    struct serializablePressureMap pressureMap { a0, a1, a2, a3 };
    return *reinterpret_cast<quint64 *>(&pressureMap);
}
//
void VeikkParms::setPressureMap(quint64 newPressureMap) {
    pressureMap = *reinterpret_cast<struct serializablePressureMap *>
                        (&newPressureMap);
}

// the following functions format the numbers in the same format that the
// driver accepts them in; see driver for meaning of casts
quint32 VeikkParms::serializeScreenSize() {
    return *reinterpret_cast<quint32 *>(&screenSize);
}
quint64 VeikkParms::serializeScreenMap() {
    return *reinterpret_cast<quint64 *>(&screenMap);
}
quint32 VeikkParms::serializeOrientation() {
    return orientation;
}
quint64 VeikkParms::serializePressureMap() {
    return *reinterpret_cast<quint64 *>(&pressureMap);
}
