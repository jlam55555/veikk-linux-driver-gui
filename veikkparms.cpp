#include "veikkconfig.h"
#include <QFile>
#include <QSettings>
#include <climits>

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

// get values from sysfs; check if modparms exist; screen size doesn't have to
// be saved because updated automatically
// TODO: error checking
int VeikkParms::loadFromSysfs() {
    deserializeAndSetScreenMap(getSysfsModparm("screen_map").toULongLong());
    deserializeAndSetOrientation(getSysfsModparm("orientation").toULong());
    deserializeAndSetPressureMap(getSysfsModparm("pressure_map").toULongLong());
    return 0;
}

// get values from a conf file
// TODO: implement this
int VeikkParms::loadFromFile(QString src) {
    QSettings settings{src, QSettings::Format::NativeFormat};
    QRect ss, sm;
    quint32 ori;
    qint16 pm[4];
    QString props[] = {"screen_size/width", "screen_size/height",
                       "screen_map/x", "screen_map/y",
                       "screen_map/width", "screen_map/height",
                       "orientation/orientation",
                       "pressure_map/a0", "pressure_map/a1",
                       "pressure_map/a2", "pressure_map/a3"};

    // check that all properties are there
    for(QString prop: props)
        if(!settings.contains(prop)) {
            qInfo() << "Missing property:" << prop;
            return -1;
        }

    // parse values; error checking will be done in setters
    // TODO: make sure pm variables don't overflow, check sign of orientation
    // 		 as these don't get checked in setters
    ss = QRect{
        0, 0,
        settings.value("screen_size/width").toInt(),
        settings.value("screen_size/height").toInt()
    };
    sm = QRect {
        settings.value("screen_map/x").toInt(),
        settings.value("screen_map/x").toInt(),
        settings.value("screen_map/width").toInt(),
        settings.value("screen_map/height").toInt()
    };
    ori = settings.value("orientation/orientation").toUInt();
    pm[0] = quint16(settings.value("pressure_map/a0").toFloat()*100);
    pm[1] = quint16(settings.value("pressure_map/a1").toFloat()*100);
    pm[2] = quint16(settings.value("pressure_map/a2").toFloat()*100);
    pm[3] = quint16(settings.value("pressure_map/a3").toFloat()*100);

    // warn if any formatting errors
//    qInfo() << settings.status();

    setScreenSize(ss);
    setScreenMap(sm);
    setOrientation(ori);
    setPressureMap(pm);
    return 0;
}

// data setters -- parse from Q-types
// TODO: error checking and returning error values; these are often called
// 		 with user-set values
void VeikkParms::setScreenSize(QRect newScreenSize) {
    if(newScreenSize.width()<=0 || newScreenSize.height()<=0)
        return;
    if(newScreenSize.width()>USHRT_MAX || newScreenSize.height()>USHRT_MAX)
        return;
    screenSize.width = quint16(newScreenSize.width());
    screenSize.height = quint16(newScreenSize.height());
}
void VeikkParms::setScreenMap(QRect newScreenMap) {
    if(newScreenMap.width()<=0 || newScreenMap.height()<=0)
        return;
    if(newScreenMap.x()<SHRT_MIN || newScreenMap.y()>SHRT_MAX ||
       newScreenMap.width()>USHRT_MAX || newScreenMap.height()>USHRT_MAX)
        return;
    screenMap.x = qint16(newScreenMap.x());
    screenMap.y = qint16(newScreenMap.y());
    screenMap.width = quint16(newScreenMap.width());
    screenMap.height = quint16(newScreenMap.height());
}
void VeikkParms::setOrientation(quint32 newOrientation) {
    if(newOrientation>3)
        return;
    orientation = newOrientation;
}
void VeikkParms::setPressureMap(qint16 *newCoefs) {
    // no error checks
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
    // apply restoreParms to modprobe.d
    restoreParms->applyModprobed();
    return 0;
}

// exports config to conf file
int VeikkParms::exportConfig(QString dest) {
    QSettings settings{dest, QSettings::Format::NativeFormat};

    settings.beginGroup("screen_size");
    settings.setValue("width", screenSize.width);
    settings.setValue("height", screenSize.height);
    settings.endGroup();

    settings.beginGroup("screen_map");
    settings.setValue("x", screenMap.x);
    settings.setValue("y", screenMap.y);
    settings.setValue("width", screenMap.width);
    settings.setValue("height", screenMap.height);
    settings.endGroup();

    settings.beginGroup("orientation");
    settings.setValue("orientation", orientation);
    settings.endGroup();

    settings.beginGroup("pressure_map");
    settings.setValue("a0", pressureMap.a0/100.0);
    settings.setValue("a1", pressureMap.a1/100.0);
    settings.setValue("a2", pressureMap.a2/100.0);
    settings.setValue("a3", pressureMap.a3/100.0);
    settings.endGroup();
}

// TODO: error checking
// TODO: update write to stream format
// note: this will fail if not root
int VeikkParms::setSysfsModparm(QString parmName, QString value) {
    const QString baseUrl = "/sys/module/veikk/parameters/";
    QFile parmFile{baseUrl + parmName};

    if(!parmFile.open(QIODevice::WriteOnly|QIODevice::Text))
        return errno;
    if(parmFile.write(value.toLocal8Bit().constData())<0)
        return errno;
    return 0;
}
QString VeikkParms::getSysfsModparm(QString parmName) {
    const QString baseUrl = "/sys/module/veikk/parameters/";
    QFile parmFile{baseUrl + parmName};

    if(!parmFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return "";
    return QString{parmFile.readAll()};
}

// TODO: error checking
// for use by applyConfig on restoreParms to save changes in /etc/modprobe.d
void VeikkParms::applyModprobed() {
    QFile modprobeConfFile{"/etc/modprobe.d/veikk.conf"};
    QTextStream format{&modprobeConfFile};

    if(!modprobeConfFile.open(QIODevice::WriteOnly|QIODevice::Text))
        return;

    format << "options veikk"
           << " screen_size=" << serializeScreenSize()
           << " screen_map=" << serializeScreenMap()
           << " orientation=" << serializeOrientation()
           << " pressure_map=" << serializePressureMap();
}

// static function for storing default pressure maps in qcombobox; equivalent
// conversion as in serializePressureMap() (non-static version)
quint64 VeikkParms::serializePressureMap(qint16 a0, qint16 a1, qint16 a2,
                                         qint16 a3) {
    struct serializablePressureMap pressureMap { a0, a1, a2, a3 };
    return *reinterpret_cast<quint64 *>(&pressureMap);
}
// static function for deserializing one of the default pressure maps stored in
// the qcombobox -- see above
void VeikkParms::setPressureMap(quint64 newPressureMap) {
    pressureMap = *reinterpret_cast<struct serializablePressureMap *>
                        (&newPressureMap);
}

// the following getters get the parms in serialized format
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

// the following setters set the parms from serialized format
// no need for error checking, these only come directly from sysfs
void VeikkParms::deserializeAndSetScreenSize(quint32 newScreenSize) {
    screenSize =
            *reinterpret_cast<struct serializableScreenSize *>(&newScreenSize);
}
void VeikkParms::deserializeAndSetScreenMap(quint64 newScreenMap) {
    screenMap =
            *reinterpret_cast<struct serializableScreenMap *>(&newScreenMap);
}
void VeikkParms::deserializeAndSetOrientation(quint32 newOrientation) {
    orientation = newOrientation;
}
void VeikkParms::deserializeAndSetPressureMap(quint64 newPressureMap) {
    pressureMap = *reinterpret_cast<struct serializablePressureMap *>
                    (&newPressureMap);
}
