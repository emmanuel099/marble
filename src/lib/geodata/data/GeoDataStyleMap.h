//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin     <ps_ml@gmx.de>
//


#ifndef GEODATASTYLEMAP_H
#define GEODATASTYLEMAP_H


#include <QtCore/QString>
#include <QtCore/QMap>

#include "GeoDataObject.h"
#include "GeoDataStyleSelector.h"

#include "geodata_export.h"

class GeoDataStyleMapPrivate;
/**
 * @short a class to map different styles to one style
 *
 * A GeoDataStyleMap connects styles for highlighted and normal
 * context. Besides GeoDataStyleSelector it derives from
 * QMap<QString, QString>.
 *
 * @See GeoDataStyle
 * @See GeoDataStyleSelector
 */
class GEODATA_EXPORT GeoDataStyleMap : public GeoDataStyleSelector,
                                       public QMap<QString, QString>
{
  public:
    /**
    * @brief return the last key
    */
    QString lastKey();
    /**
    * @brief Set the last key
    * this property is needed to set an entry in the kml parser
    * after the parser has set the last key, it will read the value
    * and add both to this map
    * @param key the last key
    */
    void setLastKey( QString key );
    /**
     * @brief Serialize the stylemap to a stream
     * @param  stream  the stream
     */
    virtual void pack( QDataStream& stream ) const;
    /**
     * @brief  Unserialize the stylemap from a stream
     * @param  stream  the stream
     */
    virtual void unpack( QDataStream& stream );

//  protected:
    GeoDataStyleMap();
    ~GeoDataStyleMap();

  private:
    Q_DISABLE_COPY( GeoDataStyleMap )
    GeoDataStyleMapPrivate * const d;
};

#endif // GeoDataStyleMap_H
