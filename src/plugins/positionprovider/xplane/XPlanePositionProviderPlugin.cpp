//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Ralf Habacker <ralf.habacker@freenet.de>
// Copyright 2019 Emmanuel Pescosta <emmanuelpescosta099@gmail.com>
//

#include "XPlanePositionProviderPlugin.h"

#include <QIcon>
#include <QNetworkDatagram>

using namespace Marble;
/* TRANSLATOR Marble::XPlanePositionProviderPlugin */

enum class XPlaneIndex : quint8 {
    Speeds = 3,
    MagneticCompass = 19,
    LatitudeLongitudeAltitude = 20,
};

constexpr quint16 BindingPort = 40000;

XPlanePositionProviderPlugin::XPlanePositionProviderPlugin()
  : m_socket()
  , m_status(PositionProviderStatusUnavailable)
  , m_position()
  , m_accuracy()
  , m_speed(0.0)
  , m_direction(0.0)
  , m_timestamp()
{
}

QString XPlanePositionProviderPlugin::name() const
{
    return tr("X-Plane position provider Plugin");
}

QString XPlanePositionProviderPlugin::nameId() const
{
    return QStringLiteral("xplane");
}

QString XPlanePositionProviderPlugin::guiString() const
{
    return tr("X-Plane");
}

QString XPlanePositionProviderPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString XPlanePositionProviderPlugin::description() const
{
    return tr("Reports the position of the running X-Plane application.");
}

QString XPlanePositionProviderPlugin::copyrightYears() const
{
    return QStringLiteral("2019");
}

QVector<PluginAuthor> XPlanePositionProviderPlugin::pluginAuthors() const
{
    return {
        PluginAuthor(QStringLiteral("Emmanuel Pescosta"),
                     QStringLiteral("emmanuelpescosta099@gmail.com")),
    };
}

QIcon XPlanePositionProviderPlugin::icon() const
{
    return QIcon();
}

void XPlanePositionProviderPlugin::initialize()
{
    setStatus(PositionProviderStatusAcquiring);

    m_socket.reset(new QUdpSocket);
    connect(m_socket.get(), &QUdpSocket::readyRead, this, [this] {
        while (m_socket->hasPendingDatagrams()) {
            const auto datagram = m_socket->receiveDatagram();
            processData(datagram.data());
        }
    });

    m_socket->bind(QHostAddress::LocalHost, BindingPort);
}

constexpr double feet_to_meters(double feet)
{
    return feet * 0.3048;
}

constexpr double knots_to_mps(double knots)
{
    return knots * 0.51444444444;
}

void XPlanePositionProviderPlugin::processData(const QByteArray &data)
{
    constexpr int HeaderLength = 4 + 1; // DATA + 1 byte
    constexpr int MetaDataLength = 4;
    constexpr int ParameterSize = 4;
    constexpr int ParameterCount = 8;
    constexpr int EntryLength = MetaDataLength + ParameterSize * ParameterCount; // metadata + actual payload

    if (data.length() < HeaderLength) {
        return;
    }
    if (not data.startsWith("DATA")) {
        return;
    }

    const int payloadLength = data.length() - HeaderLength;
    if (payloadLength % EntryLength != 0) { // payload should contain N entries
        return;
    }

    for (auto entryIt = data.constBegin() + HeaderLength; entryIt != data.constEnd(); entryIt += EntryLength) {
        const auto getParameter = [&](const int position) -> float {
            Q_ASSERT(0 <= position and position < ParameterCount);
            const auto parameterIt = entryIt + MetaDataLength + ParameterSize * position;
            return *reinterpret_cast<const float *>(std::addressof(*parameterIt));
        };

        const XPlaneIndex index = static_cast<XPlaneIndex>(*entryIt);
        switch (index) {
            case XPlaneIndex::Speeds: {
                m_speed = knots_to_mps(getParameter(3)); // ground speed
                break;
            }
            case XPlaneIndex::MagneticCompass: {
                m_direction = getParameter(0);
            }   break;
            case XPlaneIndex::LatitudeLongitudeAltitude: {
                auto position = m_position;
                position.setLatitude(getParameter(0), GeoDataCoordinates::Degree);
                position.setLongitude(getParameter(1), GeoDataCoordinates::Degree);
                position.setAltitude(feet_to_meters(getParameter(2)));

                auto accuracy = m_accuracy;
                accuracy.level = GeoDataAccuracy::Detailed;

                setPostion(position, accuracy);
                setStatus(PositionProviderStatusAvailable);
            }   break;
            default:
                // ignore
                break;
        }
    }

    m_timestamp = QDateTime::currentDateTime();
}

void XPlanePositionProviderPlugin::setStatus(const PositionProviderStatus status)
{
    if (status == m_status) {
        return;
    }

    m_status = status;

    emit statusChanged(status);
}

void XPlanePositionProviderPlugin::setPostion(const GeoDataCoordinates &position,
                                              const GeoDataAccuracy& accuracy)
{
    if ((position == m_position) and (accuracy == m_accuracy)) {
        return;
    }

    m_position = position;
    m_accuracy = accuracy;

    emit positionChanged(position, accuracy);
}

bool XPlanePositionProviderPlugin::isInitialized() const
{
    return not m_socket.isNull();
}

PositionProviderPlugin* XPlanePositionProviderPlugin::newInstance() const
{
    return new XPlanePositionProviderPlugin;
}

PositionProviderStatus XPlanePositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates XPlanePositionProviderPlugin::position() const
{
    return m_position;
}

GeoDataAccuracy XPlanePositionProviderPlugin::accuracy() const
{
    return m_accuracy;
}

qreal XPlanePositionProviderPlugin::speed() const
{
    return m_speed;
}

qreal XPlanePositionProviderPlugin::direction() const
{
    return m_direction;
}

QDateTime XPlanePositionProviderPlugin::timestamp() const
{
    return m_timestamp;
}

QString XPlanePositionProviderPlugin::error() const
{
    return QString();
}

#include "moc_XPlanePositionProviderPlugin.cpp"
