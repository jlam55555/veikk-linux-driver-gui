#include "veikkctl_dbus_util.h"
#include "veikkconfig.h"
#include <unistd.h>
#include <QApplication>

QDBusInterface *VeikkDBusUtil::getInterface()
{
	return new QDBusInterface{VEIKKCTL_DBUS_SERVICE_NAME,
			VEIKKCTL_DBUS_OBJECT_PATH,
			VEIKKCTL_DBUS_INTERFACE_NAME,
			QDBusConnection::sessionBus()};
}

bool VeikkDBusUtil::isDaemonRunning()
{
	QDBusConnection conn = QDBusConnection::sessionBus();
	QDBusConnectionInterface *dbusConnIntf = conn.interface();

	return dbusConnIntf->isServiceRegistered(VEIKKCTL_DBUS_SERVICE_NAME);
}

// set called in child service
static void setupDBusService()
{
	QDBusConnection conn = QDBusConnection::sessionBus();
	int ret_val, argc = 1;
	char *argv[] = {"veikkctl",NULL};
	QApplication app{argc, argv};
	VeikkDBusServer *server = new VeikkDBusServer{app};
	QFile logs{"logs.out"};

	if (logs.open(QIODevice::WriteOnly)) {
		QTextStream out{&logs};
		out << "Hello, world!";
		out.flush();

	qInfo() << "reached here in child";

	if (!conn.registerService(VEIKKCTL_DBUS_SERVICE_NAME)) {
		qWarning() << "setupDBusService: error registering dbus service";
		//server->stop();
		out << "meh";
		out.flush();
	} else {
		qInfo() << "reached here 2 in child";
		out << "reached here 2 in child";
		out.flush();
	}

	if (!conn.registerObject(VEIKKCTL_DBUS_OBJECT_PATH,
			VEIKKCTL_DBUS_INTERFACE_NAME, server,
			QDBusConnection::ExportAllSlots)) {
		qWarning() << "setupDBusService: error registering dbus object";
		server->stop();
	} else {
		qInfo() << "reached here 3 in child";
	}

	qInfo() << "got here";
	out << "got here";
		out.flush();
	ret_val = 0;
		logs.close();
	}

	app.exec();

	// forked process doesn't die properly in a Qt application, need to
	// manually exit
	_exit(ret_val);
}

bool VeikkDBusUtil::startDaemon()
{
	if (isDaemonRunning()) {
		qInfo() << "startDaemon: veikkctl daemon is already running";
		return -1;
	}

	char buf[256];
	readlink("/proc/self/exe", buf, 256);
	qInfo() << "CWD:" << buf;

	// child process: register service, spawn daemon, etc.
	// return code doesn't really matter here
	if (fork()) {
		qInfo() << "called from child only";
		setupDBusService();
	}

	return 0;
}

bool VeikkDBusUtil::reloadDaemon()
{
	QDBusInterface *intf;

	// if the daemon is not running, start it
	if (!isDaemonRunning()) {
		qInfo() << "reloadDaemon: veikkctl daemon is not running; "
			   "starting it now";
		return startDaemon();
	}

	intf = getInterface();
	intf->call("reload");
	delete intf;
	return 0;
}

bool VeikkDBusUtil::stopDaemon()
{
	QDBusInterface *intf;

	// if the daemon is not running, don't need to do anything
	if (!isDaemonRunning()) {
		qInfo() << "stopDaemon: veikkctl daemon is not running";
		return 0;
	}

	intf = getInterface();
	intf->call("stop");
	delete intf;
	return 0;
}
