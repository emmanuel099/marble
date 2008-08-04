//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataStyleMap.h"
#include <QtCore/QDataStream>

class GeoDataStyleMapPrivate
{
  public:
    GeoDataStyleMapPrivate()
    {
    }

    ~GeoDataStyleMapPrivate()
    {
    }
    
    QString lastKey;
};


GeoDataStyleMap::GeoDataStyleMap()
    : d( new GeoDataStyleMapPrivate )
{
}

GeoDataStyleMap::~GeoDataStyleMap()
{
    delete d;
}

QString GeoDataStyleMap::lastKey()
{
    return d->lastKey;
}

void GeoDataStyleMap::setLastKey( QString key )
{
    d->lastKey = key;
}

void GeoDataStyleMap::pack( QDataStream& stream ) const
{
    GeoDataStyleSelector::pack( stream );
    // lastKey doesn't need to be stored as it is needed at runtime only
    stream << *this;
}

void GeoDataStyleMap::unpack( QDataStream& stream )
{
    GeoDataStyleSelector::unpack( stream );
    
    stream >> *this;
}
