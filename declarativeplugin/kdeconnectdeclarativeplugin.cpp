/**
 * SPDX-FileCopyrightText: 2013 Albert Vaca <albertvaka@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "kdeconnectdeclarativeplugin.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QDBusPendingCall>
#include <QDBusPendingReply>

#include "objectfactory.h"
#include "responsewaiter.h"

#include "interfaces/devicessortproxymodel.h"
#include "interfaces/devicesmodel.h"
#include "interfaces/notificationsmodel.h"
#include <remotecommandsmodel.h>
#include <remotesinksmodel.h>

QObject* createDBusResponse()
{
    return new DBusAsyncResponse();
}

template<typename T> void registerFactory(const char* uri, const char *name) {
    qmlRegisterSingletonType<ObjectFactory>(uri, 1, 0, name,
        [](QQmlEngine* engine, QJSEngine*) -> QObject* {
            return new ObjectFactory(engine, [](const QVariant& deviceId) -> QObject * {
                return new T(deviceId.toString());
            });
        }
    );
}

void KdeConnectDeclarativePlugin::registerTypes(const char* uri)
{
    qmlRegisterType<DevicesModel>(uri, 1, 0, "DevicesModel");
    qmlRegisterType<NotificationsModel>(uri, 1, 0, "NotificationsModel");
    qmlRegisterType<RemoteCommandsModel>(uri, 1, 0, "RemoteCommandsModel");
    qmlRegisterType<DBusAsyncResponse>(uri, 1, 0, "DBusAsyncResponse");
    qmlRegisterType<DevicesSortProxyModel>(uri, 1, 0, "DevicesSortProxyModel");
    qmlRegisterType<RemoteSinksModel>(uri, 1, 0, "RemoteSinksModel");
    qmlRegisterUncreatableType<MprisDbusInterface>(uri, 1, 0, "MprisDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<LockDeviceDbusInterface>(uri, 1, 0, "LockDeviceDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<FindMyPhoneDeviceDbusInterface>(uri, 1, 0, "FindMyPhoneDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<RemoteKeyboardDbusInterface>(uri, 1, 0, "RemoteKeyboardDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<DeviceDbusInterface>(uri, 1, 0, "DeviceDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<RemoteCommandsDbusInterface>(uri, 1, 0, "RemoteCommandsDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<RemoteSystemVolumeDbusInterface>(uri, 1, 0, "RemoteSystemVolumeInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<ShareDbusInterface>(uri, 1, 0, "ShareDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterUncreatableType<BigscreenDbusInterface>(uri, 1, 0, "BigscreenDbusInterface", QStringLiteral("You're not supposed to instantiate interfaces"));
    qmlRegisterSingletonType<DaemonDbusInterface>(uri, 1, 0, "DaemonDbusInterface",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new DaemonDbusInterface;
        }
    );

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    qmlRegisterAnonymousType<QAbstractItemModel>(uri, 1);
#else
    qmlRegisterType<QAbstractItemModel>();
#endif

    registerFactory<DeviceDbusInterface>(uri, "DeviceDbusInterfaceFactory");
    registerFactory<DeviceBatteryDbusInterface>(uri, "DeviceBatteryDbusInterfaceFactory");
    registerFactory<FindMyPhoneDeviceDbusInterface>(uri, "FindMyPhoneDbusInterfaceFactory");
    registerFactory<SftpDbusInterface>(uri, "SftpDbusInterfaceFactory");
    registerFactory<RemoteKeyboardDbusInterface>(uri, "RemoteKeyboardDbusInterfaceFactory");
    registerFactory<MprisDbusInterface>(uri, "MprisDbusInterfaceFactory");
    registerFactory<RemoteControlDbusInterface>(uri, "RemoteControlDbusInterfaceFactory");
    registerFactory<LockDeviceDbusInterface>(uri, "LockDeviceDbusInterfaceFactory");
    registerFactory<SmsDbusInterface>(uri, "SmsDbusInterfaceFactory");
    registerFactory<RemoteCommandsDbusInterface>(uri, "RemoteCommandsDbusInterfaceFactory");
    registerFactory<ShareDbusInterface>(uri, "ShareDbusInterfaceFactory");
    registerFactory<RemoteSystemVolumeDbusInterface>(uri, "RemoteSystemVolumeDbusInterfaceFactory");
    registerFactory<BigscreenDbusInterface>(uri, "BigscreenDbusInterfaceFactory");
}

void KdeConnectDeclarativePlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);

    engine->rootContext()->setContextProperty(QStringLiteral("DBusResponseFactory")
      , new ObjectFactory(engine, createDBusResponse));

    engine->rootContext()->setContextProperty(QStringLiteral("DBusResponseWaiter")
      , DBusResponseWaiter::instance());
}
