//***************************************************************
// CLass: NavitBin
//
// Description:
//
//
// Author: Chris Browet <cbro@semperpax.com> (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//******************************************************************

#ifndef NAVITBIN_H
#define NAVITBIN_H

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include <QPoint>
#include <QPolygon>
#include <QHash>
#include <QStringList>

enum attr_type {
#define ATTR2(x,y) attr_##y=x,
#define ATTR(x) attr_##x,
#include "attr_def.h"
#undef ATTR2
#undef ATTR
};

enum item_type {
#define ITEM2(x,y) type_##y=x,
#define ITEM(x) type_##x,
#include "item_def.h"
#undef ITEM2
#undef ITEM
};

class NavitAttribute
{
public:
    NavitAttribute()
        : type(0) {}
    NavitAttribute(qint32 theType, QByteArray theAttribute)
        : type(theType), attribute(theAttribute) {}

public:
    quint32 type;
    QByteArray attribute;
};

class NavitFeature
{
public:
    NavitFeature()
        : type(0) {}

public:
    quint32 type;
    QVector<QPoint> coordinates;
    QList<NavitAttribute> attributes;
};

class NavitTile
{
public:
    QList<NavitFeature> features;
    QList< QPair < QRect, quint32 > > pointers;
};

class NavitBin
{
public:
    NavitBin();

    bool setFilename(const QString& filename);
    bool readTile(int index) const;
    bool readTile(QString fn) const;

    bool getFeatures(const QString& tileRef, QList <NavitFeature>& theFeats) const;
    bool walkTiles(const QRect& box, const NavitTile& t, QList <NavitFeature>& theFeats) const;
    bool getFeatures(const QRect& box, QList <NavitFeature>& theFeats) const;

private:
    QuaZip* zip;
    QuaZipFile* file;

    mutable QHash<QString, NavitTile > theTiles;
    mutable QStringList tileIndex;
    NavitTile indexTile;
};

#endif // NAVITBIN_H
