//
// C++ Interface: ImportExportGdal
//
// Description:
//
//
// Author: cbro <cbro@semperpax.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ImportExportGDAL_H
#define ImportExportGDAL_H

#include <ImportExport/IImportExport.h>

class Projection;
class Layer;
class OGRGeometry;
class OGRLineString;
class OGRPoint;
class OGRCoordinateTransformation;


/**
    @author cbro <cbro@semperpax.com>
*/
class ImportExportGdal : public IImportExport
{
public:
    ImportExportGdal(Document* doc);

    ~ImportExportGdal();

    // Specify the input as a QFile
    virtual bool loadFile(QString filename);
    // Specify the output as a QFile
    virtual bool saveFile(QString filename);
    // import the  input
    virtual bool import(Layer* aLayer);

    //export
    virtual bool export_(const QList<Feature *>& featList);

protected:
    OGRCoordinateTransformation *toWGS84;

    Feature* parseGeometry(Layer* aLayer, OGRGeometry *poGeometry);

    Node *nodeFor(Layer* aLayer, OGRPoint point);
    Way *readWay(Layer* aLayer, OGRLineString *poRing);

private:
    QHash<OGRPoint, Node*> pointHash;
};

#endif
