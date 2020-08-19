/**
 * SPDX-FileCopyrightText: 2015 Aleix Pol Gonzalez <aleixpol@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "lockdeviceplugin.h"

#include <KLocalizedString>
#include <KPluginFactory>

#include <QDebug>
#include "plugin_lock_debug.h"

#include <core/device.h>
#include <dbushelper.h>

#include <QTimer>


K_PLUGIN_CLASS_WITH_JSON(LockDevicePlugin, "kdeconnect_lockdevice.json")

LockDevicePlugin::LockDevicePlugin(QObject* parent, const QVariantList& args)
    : KdeConnectPlugin(parent, args)
    , m_remoteLocked(false)
    , m_iface(QStringLiteral("org.freedesktop.login1"), QStringLiteral("/org/freedesktop/login1/session/auto"), QDBusConnection::systemBus())
    , m_propertiesInterface(QStringLiteral("org.freedesktop.login1"), QString(), QDBusConnection::systemBus())
{
    connect(&m_propertiesInterface, &OrgFreedesktopDBusPropertiesInterface::PropertiesChanged, this, [this](const QString& interface, const QVariantMap& properties, QStringList invalidatedProperties ) {

        if (interface != QLatin1String("org.freedesktop.login1.Session")) {
            return;
        }

        if (!properties.contains(QStringLiteral("LockedHint"))) {
            return;
        }

        m_localLocked = properties.value(QStringLiteral("LockedHint")).toBool();
        qDebug() << "Change" << m_localLocked;
        sendState();
    });

    m_localLocked = m_iface.lockedHint();
}

LockDevicePlugin::~LockDevicePlugin()
{
}

bool LockDevicePlugin::isLocked() const
{
    return m_remoteLocked;
}

void LockDevicePlugin::setLocked(bool locked)
{
    NetworkPacket np(PACKET_TYPE_LOCK_REQUEST, {{QStringLiteral("setLocked"), locked}});
    sendPacket(np);
}

bool LockDevicePlugin::receivePacket(const NetworkPacket & np)
{
    if (np.has(QStringLiteral("isLocked"))) {
        bool locked = np.get<bool>(QStringLiteral("isLocked"));
        if (m_remoteLocked != locked) {
            m_remoteLocked = locked;
            Q_EMIT lockedChanged(locked);
        }
    }

    if (np.has(QStringLiteral("requestLocked"))) {
        sendState();
    }

    if (np.has(QStringLiteral("setLocked"))) {
        const bool lock = np.get<bool>(QStringLiteral("setLocked"));

        if (lock) {
            m_iface.Lock();
        } else {
            m_iface.Unlock();
        }

        sendState();
    }

    return true;
}

void LockDevicePlugin::sendState()
{
    NetworkPacket np(PACKET_TYPE_LOCK, {{QStringLiteral("isLocked"), m_localLocked}});
    sendPacket(np);
}

void LockDevicePlugin::connected()
{
    NetworkPacket np(PACKET_TYPE_LOCK_REQUEST, {{QStringLiteral("requestLocked"), QVariant()}});
    sendPacket(np);
}

QString LockDevicePlugin::dbusPath() const
{
    return QStringLiteral("/modules/kdeconnect/devices/") + device()->id() + QStringLiteral("/lockdevice");
}

#include "lockdeviceplugin.moc"
