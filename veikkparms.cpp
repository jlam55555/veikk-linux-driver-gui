#include "veikkconfig.h"
#include <QFile>
#include <QSettings>
#include <climits>

VeikkParms::VeikkParms()
    : screenSize{0, 0}, screenMap{0, 0, 0, 0},
      orientation{0}, pressureMap{0, 100, 0, 0} { }

// get error message for custom errors
QString VeikkParms::strerror(VPStatus err) {
    QString errnoMsg{::strerror(errno)};
    errno = 0;
    switch(err) {
    case VP_STATUS_NORMAL:
        return "No error.";
    case VP_STATUS_ACCES:
        return "File access/permissions error.";
    case VP_STATUS_FORMAT:
        return "Format error.";
    case VP_STATUS_MISSING:
        return "Missing parameters.";
    case VP_STATUS_INVAL:
        return "Invalid settings.";
    case VP_STATUS_OVRFLW:
        return "Invalid settings overflow type.";
    case VP_STATUS_ERRNO:
        return errnoMsg;
    default:
        return "Error number invalid.";
    }
}

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
VeikkParms::VPStatus VeikkParms::loadFromSysfs() {
    QString sm, ori, pm;
    VPStatus err;

    if((err=getSysfsModparm("screen_map", sm)) ||
       (err=getSysfsModparm("orientation", ori)) ||
       (err=getSysfsModparm("pressure_map", pm)))
        return err;

    deserializeAndSetScreenMap(sm.toULongLong());
    deserializeAndSetOrientation(ori.toUInt());
    deserializeAndSetPressureMap(pm.toULongLong());
    return VP_STATUS_NORMAL;
}

// get values from a conf file
VeikkParms::VPStatus VeikkParms::loadFromFile(QString src) {
    QSettings settings{src, QSettings::Format::NativeFormat};
    QRect ss, sm;
    quint32 ori;
    qint16 pm[4];
    VPStatus err;
    QString props[] = {"screen_size/width", "screen_size/height",
                       "screen_map/x", "screen_map/y",
                       "screen_map/width", "screen_map/height",
                       "orientation/orientation",
                       "pressure_map/a0", "pressure_map/a1",
                       "pressure_map/a2", "pressure_map/a3"};

    // check that all properties are there
    for(QString prop: props)
        if(!settings.contains(prop))
            return VP_STATUS_MISSING;

    // parse values; error checking will be done in setters
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
    // some conversion happens here without error checking; will force convert
    // some invalid parameters to valid ones (okay)
    ori = settings.value("orientation/orientation").toUInt();
    pm[0] = quint16(settings.value("pressure_map/a0").toFloat()*100);
    pm[1] = quint16(settings.value("pressure_map/a1").toFloat()*100);
    pm[2] = quint16(settings.value("pressure_map/a2").toFloat()*100);
    pm[3] = quint16(settings.value("pressure_map/a3").toFloat()*100);

    // errors with formatting or access
    if(settings.status()==QSettings::Status::FormatError)
        return VP_STATUS_FORMAT;
    else if(settings.status()==QSettings::Status::AccessError)
        return VP_STATUS_ACCES;

    if((err = setScreenSize(ss))<0)
        return err;
    if((err = setScreenMap(sm))<0)
        return err;
    if((err = setOrientation(ori))<0)
        return err;
    if((err = setPressureMap(pm))<0)
        return err;
    return VP_STATUS_NORMAL;
}

// data setters -- parse from Q-types
VeikkParms::VPStatus VeikkParms::setScreenSize(QRect newScreenSize) {
    if(newScreenSize.width()<=0 || newScreenSize.height()<=0)
        return VP_STATUS_INVAL;
    if(newScreenSize.width()>USHRT_MAX || newScreenSize.height()>USHRT_MAX)
        return VP_STATUS_OVRFLW;
    screenSize.width = quint16(newScreenSize.width());
    screenSize.height = quint16(newScreenSize.height());
    return VP_STATUS_NORMAL;
}
VeikkParms::VPStatus VeikkParms::setScreenMap(QRect newScreenMap) {
    if(newScreenMap.width()<=0 || newScreenMap.height()<=0)
        return VP_STATUS_INVAL;
    if(newScreenMap.x()<SHRT_MIN || newScreenMap.y()>SHRT_MAX ||
       newScreenMap.width()>USHRT_MAX || newScreenMap.height()>USHRT_MAX)
        return VP_STATUS_OVRFLW;
    screenMap.x = qint16(newScreenMap.x());
    screenMap.y = qint16(newScreenMap.y());
    screenMap.width = quint16(newScreenMap.width());
    screenMap.height = quint16(newScreenMap.height());
    return VP_STATUS_NORMAL;
}
VeikkParms::VPStatus VeikkParms::setOrientation(quint32 newOrientation) {
    if(newOrientation>3)
        return VP_STATUS_INVAL;
    orientation = newOrientation;
    return VP_STATUS_NORMAL;
}
VeikkParms::VPStatus VeikkParms::setPressureMap(qint16 *newCoefs) {
    // no error checks
    pressureMap = *reinterpret_cast<struct serializablePressureMap *>(newCoefs);
    return VP_STATUS_NORMAL;
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
VeikkParms::VPStatus VeikkParms::applyConfig(VeikkParms &restoreParms, int parms) {
    VPStatus err;
    if((parms&VEIKK_MP_SCREEN_SIZE) &&
       (err=setSysfsModparm("screen_size", QString::number(serializeScreenSize()))))
        return err;
    if((parms&VEIKK_MP_SCREEN_MAP) &&
       (err=setSysfsModparm("screen_map", QString::number(serializeScreenMap()))))
        return err;
    if((parms&VEIKK_MP_ORIENTATION) &&
       (err=setSysfsModparm("orientation", QString::number(serializeOrientation()))))
        return err;
    if((parms&VEIKK_MP_PRESSURE_MAP) &&
        (err=setSysfsModparm("pressure_map",
                        QString::number(serializePressureMap()))))
        return err;
    // save to restoreParms
    restoreParms.restoreConfig(this, parms);
    // apply restoreParms to modprobe.d
    restoreParms.applyModprobed();
    return VP_STATUS_NORMAL;
}

// exports config to conf file
VeikkParms::VPStatus VeikkParms::exportConfig(QString dest) {
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

    settings.sync();
    return settings.status()==QSettings::Status::NoError
            ?VP_STATUS_NORMAL:VP_STATUS_ACCES;
}

// TODO: update write to stream format
// set modparm in sysfs
VeikkParms::VPStatus VeikkParms::setSysfsModparm(QString parmName, QString value) {
    const QString baseUrl = "/sys/module/veikk/parameters/";
    QFile parmFile{baseUrl + parmName};

    if(!parmFile.open(QIODevice::WriteOnly|QIODevice::Text))
        return VP_STATUS_ERRNO;
    if(parmFile.write(value.toLocal8Bit().constData())<0)
        return VP_STATUS_ERRNO;
    return VP_STATUS_NORMAL;
}
VeikkParms::VPStatus VeikkParms::getSysfsModparm(QString parmName, QString &value) {
    const QString baseUrl = "/sys/module/veikk/parameters/";
    QFile parmFile{baseUrl + parmName};

    if(!parmFile.open(QIODevice::ReadOnly|QIODevice::Text))
        return VP_STATUS_ERRNO;
    value = parmFile.readAll();
    return VP_STATUS_NORMAL;
}

// for use by applyConfig on restoreParms to save changes in /etc/modprobe.d
VeikkParms::VPStatus VeikkParms::applyModprobed() {
    QFile modprobeConfFile{"/etc/modprobe.d/veikk.conf"};
    QTextStream format{&modprobeConfFile};

    if(!modprobeConfFile.open(QIODevice::WriteOnly|QIODevice::Text))
        return VP_STATUS_ERRNO;

    format << "options veikk"
           << " screen_size=" << serializeScreenSize()
           << " screen_map=" << serializeScreenMap()
           << " orientation=" << serializeOrientation()
           << " pressure_map=" << serializePressureMap();
    return format.status()==QTextStream::Status::Ok
            ?VP_STATUS_NORMAL:VP_STATUS_ERRNO;
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
