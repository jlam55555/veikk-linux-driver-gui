#include "unistd.h"
#include "veikkconfig.h"
#include "veikkctl_dbus_util.h"
#include <QApplication>
#include <QtDBus>
#include <QDebug>

#define VEIKKCTL_VERSION "v3-alpha"

void printHelpMenu()
{
	// TODO: add configuration commands
	qInfo() <<
"veikkctl -- configuration utility for VEIKK digitizers\n"
"(for use in conjunction with the VEIKK Linux driver v3+)\n"
"Usage: veikkctl [COMMAND] [ADDITIONAL ARGS...], where COMMAND is one of:\n"
"\tstart\tStart VEIKK daemon\n"
"\tstop\tStop VEIKK daemon\n"
"\treload\tNew VEIKK device added or config changed\n"
"\thelp\tShow this help menu\n"
"\tversion\tShow version info\n\n";
}

void printVersionInfo()
{
	qInfo() << "veikkctl " << VEIKKCTL_VERSION;
}

int startVeikkDaemon()
{
	int argc = 1;
	char *argv[] = {"veikkctl", NULL};

	QApplication app{argc, argv};

	// connect to dbus session bus
	QDBusConnection conn = QDBusConnection::sessionBus();
	QDBusConnectionInterface *dbusConnIntf = conn.interface();
	QDBusMessage pingMessage;

	// check if service is already registered
	if (dbusConnIntf->isServiceRegistered("com.veikk.VeikkCtl")) {
		qWarning() << "veikkctl is already running";
		return -1;
	}

	// register service
	bool isRegistered = conn.registerService("com.veikk.VeikkCtl");
	if (!isRegistered) {
		qCritical() << "failed to register dbus service";
		return -1;
	}

	// register object
	conn.registerObject("/Server", "com.veikk.VeikkServerInterface",
			new VeikkDBusServer{app},
			QDBusConnection::ExportAllSlots);

	// check if server name is owned; attempt to call its ping method

	app.exec();


	// attempt to own server name

	// if not successful, die and return error

	// if successful, fork and launch daemon
	return 0;
}

/*int stopVeikkDaemon()
{
	QDBusConnection conn = QDBusConnection::sessionBus();
	QDBusConnectionInterface *dbusConnIntf = conn.interface();
	QDBusInterface veikkctlIntf{"com.veikk.VeikkCtl", "/Server",
			"com.veikk.VeikkServerInterface", conn};

	// check if daemon is running; warn if it's not
	if (!dbusConnIntf->isServiceRegistered("com.veikk.VeikkCtl")) {
		qWarning() << "veikkctl is not running";
		return -1;
	}

	veikkctlIntf.call("testping");
	return 0;
}

int reload_veikk_daemon()
{
	// connect to dbs daemon

	// check if server name is owned

	// if server name is owned, kill it

	// start daemon
	return 0;
}*/

int main(int argc, char *argv[])
{
	const char *cmd;

	if (argc < 2) {
		printHelpMenu();
		return 0;
	}
	cmd = argv[1];

	//QApplication app{argc, argv};
	if (!strncmp(cmd, "start", strlen("start"))) {
		return VeikkDBusUtil::startDaemon();
		//startVeikkDaemon();
	} else if (!strncmp(cmd, "stop", strlen("stop"))) {
		return VeikkDBusUtil::stopDaemon();
	} else if (!strncmp(cmd, "reload", strlen("reload"))) {
		// reload veikk daemon
	} else if (!strncmp(cmd, "version", strlen("version"))) {
		printVersionInfo();
		return 0;
	} else {
		printHelpMenu();
		return 0;
	}

	// TODO: check if user is root; if not, issue warning
	if(geteuid()) {}

	//MainWindow win;
	//win.show();
	//return app.exec();
	return 0;
}
