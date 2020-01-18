#ifndef VEIKKPARMS_H
#define VEIKKPARMS_H

#include <QRect>
#include <QString>

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

    void restoreConfig(VeikkParms *restoreParms, int parms);

    int loadFromSysfs();
    int loadFromFile(QString &pathToConf);

    // setters and getters: these translate to/from internal types to more
    // easily-usable-in-UI Q-types
    void setScreenSize(QRect newScreenSize);
    void setScreenMap(QRect newScreenMap);
    void setOrientation(quint32 newOrientation);
    void setPressureMap(qint16 *newCoefs);

    QSize getScreenSize();
    QRect getScreenMap();
    quint32 getOrientation();
    void getPressureMap(qint16 *coefs);
    bool isInvalidScreenMap();

    int applyConfig(VeikkParms *restoreParms, int parms);
    int exportConfig(QString dest);

    // for use with combobox, which has to store data in compressed form
    static quint64 serializePressureMap(qint16 a0, qint16 a1, qint16 a2,
                                        qint16 a3);
    void setPressureMap(quint64 newPressureMap);

private:
    // these structs are semantic and directly serializable to the correct
    // format for use with the driver; see driver for more details.
    // note: order of elements must match declarations in driver! these types
    // are only meant for internal use to be easier to deal with serialization /
    // deserialization; convert to/from Q-types when interfacing with this class
    // from the UI (i.e., screenSize/Map->QRect, orientation->qint32,
    // pressureMap->qint16[4])
    struct serializableScreenSize {
        quint16 width, height;
    } screenSize;
    struct serializableScreenMap {
        qint16 x, y;
        quint16 width, height;
    } screenMap;
    quint32 orientation;
    struct serializablePressureMap {
        qint16 a0, a1, a2, a3;
    } pressureMap;

    int setSysfsModparm(QString parmName, QString value);
    QString getSysfsModparm(QString parmName);

    // for use by applyConfig on restoreParms after applying to make changes
    // persist after boot, using /etc/modprobe.d
    void applyModprobed();

    quint32 serializeScreenSize();
    quint64 serializeScreenMap();
    quint32 serializeOrientation();
    quint64 serializePressureMap();

    void deserializeAndSetScreenSize(quint32 newScreenSize);
    void deserializeAndSetScreenMap(quint64 newScreenMap);
    void deserializeAndSetOrientation(quint32 newOrientation);
    void deserializeAndSetPressureMap(quint64 newPressureMap);
};

#endif // VEIKKPARMS_H
