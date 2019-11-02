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

#ifndef XPLANEPOSITIONPROVIDERPLUGIN_H
#define XPLANEPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include "GeoDataAccuracy.h"
#include "GeoDataCoordinates.h"

#include <QDateTime>
#include <QUdpSocket>

namespace Marble
{

class XPlanePositionProviderPlugin : public PositionProviderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.XPlanePositionProviderPlugin")
    Q_INTERFACES(Marble::PositionProviderPluginInterface)

public:
    XPlanePositionProviderPlugin();

    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QVector<PluginAuthor> pluginAuthors() const override;
    QIcon icon() const override;
    void initialize() override;
    bool isInitialized() const override;

    PositionProviderPlugin* newInstance() const override;

    PositionProviderStatus status() const override;
    GeoDataCoordinates position() const override;
    GeoDataAccuracy accuracy() const override;
    QString error() const override;
    qreal speed() const override;
    qreal direction() const override;
    QDateTime timestamp() const override;

private:
    void processData(const QByteArray &data);
    void setStatus(const PositionProviderStatus status);
    void setPostion(const GeoDataCoordinates &position,
                    const GeoDataAccuracy& accuracy);

private:
    QScopedPointer<QUdpSocket> m_socket;
    PositionProviderStatus m_status;
    GeoDataCoordinates m_position;
    GeoDataAccuracy m_accuracy;
    qreal m_speed;
    qreal m_direction;
    QDateTime m_timestamp;
};

}

#endif // XPLANEPOSITIONPROVIDERPLUGIN_H
