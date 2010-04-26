//***************************************************************
// CLass: NavitAdapter
//
// Description:
//
//
// Author: Chris Browet <cbro@semperpax.com> (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//******************************************************************

#include "NavitAdapter.h"

#include <QCoreApplication>
#include <QtPlugin>
#include <QAction>
#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QBuffer>

#include <math.h>

static const QUuid theUid ("{afc13af7-d538-48e1-9997-a2b45db5b3ff}");

#define FILTER_OPEN_SUPPORTED \
    tr("Supported formats")+" (*.bin)\n" \
    +tr("All Files (*)")

double angToRad(double a)
{
    return a*M_PI/180.;
}

QPointF mercatorProject(const QPointF& c)
{
    double x = angToRad(c.x()) / M_PI * 20037508.34;
    double y = log(tan(angToRad(c.y())) + 1/cos(angToRad(c.y()))) / M_PI * (20037508.34);

    return QPointF(x, y);
}

NavitAdapter::NavitAdapter()
{
    QAction* loadFile = new QAction(tr("Load Navit file..."), this);
    loadFile->setData(theUid.toString());
    connect(loadFile, SIGNAL(triggered()), SLOT(onLoadFile()));
    theMenu = new QMenu();
    theMenu->addAction(loadFile);
}


NavitAdapter::~NavitAdapter()
{
}

void NavitAdapter::onLoadFile()
{
    QString fileName = QFileDialog::getOpenFileName(
                    NULL,
                    tr("Open Navit file"),
                    "", FILTER_OPEN_SUPPORTED);
    if (fileName.isEmpty())
        return;

    navit.setFilename(fileName);

    return;
}

QString	NavitAdapter::getHost() const
{
    return "";
}

QUuid NavitAdapter::getId() const
{
    return QUuid(theUid);
}

IMapAdapter::Type NavitAdapter::getType() const
{
    return IMapAdapter::DirectBackground;
}

QString	NavitAdapter::getName() const
{
    return "Navit";
}

QMenu* NavitAdapter::getMenu() const
{
    return theMenu;
}

QRectF NavitAdapter::getBoundingbox() const
{
    return QRectF(QPointF(-20037508.34, -20037508.34), QPointF(20037508.34, 20037508.34));
}

QString NavitAdapter::projection() const
{
    return "EPSG:900913";
}

QPixmap NavitAdapter::getPixmap(const QRectF& wgs84Bbox, const QRectF& projBbox, const QRect& src) const
{
    return QPixmap();
}

IImageManager* NavitAdapter::getImageManager()
{
    return NULL;
}

void NavitAdapter::setImageManager(IImageManager* anImageManager)
{
}

void NavitAdapter::cleanup()
{
}

Q_EXPORT_PLUGIN2(MWalkingPapersBackgroundPlugin, NavitAdapter)
