#include "veikkconfig.h"
#include <QFile>

VeikkParms::VeikkParms()
    : screenSize{0, 0}, screenMap{0, 0, 0, 0},
      orientation{0}, pressureMap{0, 100, 0, 0} { }

// used to restore current config to restore point or default
void VeikkParms::restoreConfig(VeikkParms &vp) {
    memcpy(screenSize, vp.screenSize, sizeof screenSize);
    memcpy(screenMap, vp.screenMap, sizeof screenMap);
    orientation = vp.orientation;
    memcpy(pressureMap, vp.pressureMap, sizeof pressureMap);
}

// data setters
void VeikkParms::setScreenSize(QRect newScreenSize) {
    screenSize[0] = quint16(newScreenSize.width());
    screenSize[1] = quint16(newScreenSize.height());
}
void VeikkParms::setScreenMap(QRect newScreenMap) {
    screenMap[0] = quint16(newScreenMap.x());
    screenMap[1] = quint16(newScreenMap.y());
    screenMap[2] = quint16(newScreenMap.width());
    screenMap[3] = quint16(newScreenMap.height());
}
void VeikkParms::setOrientation(quint32 newOrientation) {
    orientation = newOrientation;
}
void VeikkParms::setPressureMap(qint16 *newCoefs) {
    memcpy(pressureMap, newCoefs, sizeof pressureMap);
}

// applies config to sysfs
// TODO: error checking
int VeikkParms::applyConfig(ModparmType type) {
    if(type & VEIKK_MP_SCREEN_SIZE) {
        setSysfsModparm("screen_size", QString::number(screenSizeExport()));
    }
    if(type & VEIKK_MP_SCREEN_MAP) {
        setSysfsModparm("screen_map", QString::number(screenMapExport()));
    }
    if(type & VEIKK_MP_ORIENTATION) {
        setSysfsModparm("orientation", QString::number(orientationExport()));
    }
    if(type & VEIKK_MP_PRESSURE_MAP) {
        setSysfsModparm("pressure_map", QString::number(pressureMapExport()));
    }
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

// the following functions format the numbers in the same format that the
// driver accepts them in
quint32 VeikkParms::screenSizeExport() {
    return (quint32(screenSize[0])<<16)|screenSize[1];
}
quint64 VeikkParms::screenMapExport() {
    return (quint64(screenMap[0])<<48)
         | (quint64(screenMap[1])<<32)
         | (quint64(screenMap[2])<<16)
         | screenMap[3];
}
quint32 VeikkParms::orientationExport() {
    return orientation;
}
quint64 VeikkParms::pressureMapExport() {
    return (quint64(pressureMap[3])<<48)
         | (quint64(pressureMap[2])<<32)
         | (quint64(pressureMap[1])<<16)
         | quint64(pressureMap[0]);
}
