#include "Projection.h"

#include <QRect>
#include <QRectF>

#include <math.h>

// from wikipedia
#define EQUATORIALRADIUS 6378137.0
#define POLARRADIUS      6356752.0
#define EQUATORIALMETERCIRCUMFERENCE  40075016.68
#define EQUATORIALMETERHALFCIRCUMFERENCE  20037508.34
#define EQUATORIALMETERPERDEGREE    222638.981555556

#include "Node.h"

// ProjectionPrivate

class ProjectionPrivate
{
public:
#ifndef _MOBILE
    ProjProjection theWGS84Proj;
#endif
    QString projType;
    QString projProj4;
    QRectF ProjectedViewport;
    int ProjectionRevision;
    bool IsMercator;
    bool IsLatLong;

public:
    ProjectionPrivate()
        : ProjectionRevision(0)
        , IsMercator(false)
        , IsLatLong(false)
    {
    }

    QPointF mercatorProject(const QPointF& c) const
    {
        qreal x = c.x() / 180. * EQUATORIALMETERHALFCIRCUMFERENCE;
        qreal y = log(tan(angToRad(c.y())) + 1/cos(angToRad(c.y()))) / M_PI * (EQUATORIALMETERHALFCIRCUMFERENCE);

        return QPointF(x, y);
    }

    Coord mercatorInverse(const QPointF& point) const
    {
        qreal longitude = point.x()*180.0/EQUATORIALMETERHALFCIRCUMFERENCE;
        qreal latitude = radToAng(atan(sinh(point.y()/EQUATORIALMETERHALFCIRCUMFERENCE*M_PI)));

        return Coord(longitude, latitude);
    }

    inline QPointF latlonProject(const QPointF& c) const
    {
        return QPointF(c.x()/**EQUATORIALMETERPERDEGREE*/, c.y()/**EQUATORIALMETERPERDEGREE*/);
    }

    inline Coord latlonInverse(const QPointF& point) const
    {
        return Coord(point.x()/*/EQUATORIALMETERPERDEGREE*/, point.y()/*/EQUATORIALMETERPERDEGREE*/);
    }
};

//Projection

Projection::Projection(void)
: p(new ProjectionPrivate)
{
#if defined(Q_OS_WIN) && !defined(_MOBILE)
    QString pdir(QDir::toNativeSeparators(qApp->applicationDirPath() + "/" STRINGIFY(SHARE_DIR) "/proj"));
    const char* proj_dir = pdir.toUtf8().constData();
//    const char* proj_dir = "E:\\cbro\\src\\merkaartor-devel\\binaries\\bin\\share\\proj";
    pj_set_searchpath(1, &proj_dir);
#endif // Q_OS_WIN

#ifndef _MOBILE
    theProj = NULL;
    p->theWGS84Proj = Projection::getProjection("+proj=longlat +ellps=WGS84 +datum=WGS84");
    setProjectionType(M_PREFS->getProjectionType());
#endif
}

Projection::~Projection(void)
{
#ifndef _MOBILE
    pj_free(theProj);
#endif // _MOBILE
    delete p;
}

QPointF Projection::inverse2Point(const QPointF & Map) const
{
    if (p->IsLatLong)
        return p->latlonInverse(Map);
    else
    if (p->IsMercator)
        return p->mercatorInverse(Map);
#ifndef _MOBILE
    else
        return projInverse(Map);
#endif
    return QPointF();
}

QPointF Projection::project(const QPointF & Map) const
{
    if (p->IsMercator)
        return p->mercatorProject(Map);
    else
    if (p->IsLatLong)
        return p->latlonProject(Map);
#ifndef _MOBILE
    else
        return projProject(Map);
#endif
    return QPointF();
}

QPointF Projection::project(Node* aNode) const
{
    if (aNode && !aNode->projection().isNull() && aNode->projectionRevision() == p->ProjectionRevision)
        return aNode->projection();

    QPointF pt;
    if (p->IsMercator)
        pt = p->mercatorProject(aNode->position());
    else
    if (p->IsLatLong)
        pt = p->latlonProject(aNode->position());
#ifndef _MOBILE
    else
        pt = projProject(aNode->position());
#endif

    aNode->setProjectionRevision(p->ProjectionRevision);
    aNode->setProjection(pt);

    return pt;
}

QLineF Projection::project(const QLineF & Map) const
{
    if (p->IsMercator)
        return QLineF(p->mercatorProject(Map.p1()), p->mercatorProject(Map.p2()));
    else
    if (p->IsLatLong)
        return QLineF(p->latlonProject(Map.p1()), p->latlonProject(Map.p2()));
#ifndef _MOBILE
    else
        return QLineF(projProject(Map.p1()), projProject(Map.p2()));
#endif
    return QLineF();
}


Coord Projection::inverse2Coord(const QPointF & Screen) const
{
    if (p->IsLatLong)
        return p->latlonInverse(Screen);
    else
    if (p->IsMercator)
        return p->mercatorInverse(Screen);
#ifndef _MOBILE
    else
        return projInverse(Screen);
#endif
    return Coord();
}

QRectF Projection::toProjectedRectF(const QRectF& Viewport, const QRect& screen) const
{
    QPointF tl, br;
    QRectF pViewport;

    tl = project(Viewport.topLeft());
    br = project(Viewport.bottomRight());
    pViewport = QRectF(tl, br);

    QPointF pCenter(pViewport.center());

    qreal wv, hv;
    //wv = (pViewport.width() / Viewport.londiff()) * ((double)screen.width() / Viewport.londiff());
    //hv = (pViewport.height() / Viewport.latdiff()) * ((double)screen.height() / Viewport.latdiff());

    qreal Aspect = (double)screen.width() / screen.height();
    qreal pAspect = fabs(pViewport.width() / pViewport.height());

    if (pAspect > Aspect) {
        wv = fabs(pViewport.width());
        hv = fabs(pViewport.height() * pAspect / Aspect);
    } else {
        wv = fabs(pViewport.width() * Aspect / pAspect);
        hv = fabs(pViewport.height());
    }

    pViewport = QRectF((pCenter.x() - wv/2), (pCenter.y() + hv/2), wv, -hv);

    return pViewport;
}

CoordBox Projection::fromProjectedRectF(const QRectF& Viewport) const
{
    Coord tl, br;
    CoordBox bbox;

    tl = inverse2Coord(Viewport.topLeft());
    br = inverse2Coord(Viewport.bottomRight());
    bbox = CoordBox(tl, br);

    return bbox;
}

#ifndef _MOBILE

void Projection::projTransform(ProjProjection srcdefn,
                           ProjProjection dstdefn,
                           long point_count, int point_offset, qreal *x, qreal *y, qreal *z )
{
    pj_transform(srcdefn, dstdefn, point_count, point_offset, x, y, z);
}

void Projection::projTransformFromWGS84(long point_count, int point_offset, qreal *x, qreal *y, qreal *z ) const
{
    pj_transform(p->theWGS84Proj, theProj, point_count, point_offset, x, y, z);
}

void Projection::projTransformToWGS84(long point_count, int point_offset, qreal *x, qreal *y, qreal *z ) const
{
    pj_transform(theProj, p->theWGS84Proj, point_count, point_offset, x, y, z);
}

QPointF Projection::projProject(const QPointF & Map) const
{
    qreal x = angToRad(Map.x());
    qreal y = angToRad(Map.y());

    projTransformFromWGS84(1, 0, &x, &y, NULL);

    return QPointF(x, y);
}

Coord Projection::projInverse(const QPointF & pProj) const
{
    qreal x = pProj.x();
    qreal y = pProj.y();

    projTransformToWGS84(1, 0, &x, &y, NULL);

    return Coord(radToAng(x), radToAng(y));
}
#endif // _MOBILE

bool Projection::projIsLatLong() const
{
    return p->IsLatLong;
}

//bool Projection::projIsMercator()
//{
//    return p->IsMercator;
//}


#ifndef _MOBILE
ProjProjection Projection::getProjection(QString projString)
{
    ProjProjection theProj = pj_init_plus(QString("%1 +over").arg(projString).toLatin1());
    return theProj;
}
#endif // _MOBILE

bool Projection::setProjectionType(QString aProjectionType)
{
    if (aProjectionType == p->projType)
        return true;

#ifndef _MOBILE
    if (theProj) {
        pj_free(theProj);
        theProj = NULL;
    }
#endif // _MOBILE

    p->ProjectionRevision++;
    p->projType = aProjectionType;
    p->projProj4 = QString();
    p->IsLatLong = false;
    p->IsMercator = false;

    // Hardcode "Google " projection
    if (
            p->projType.isEmpty() ||
            p->projType.toUpper().contains("OSGEO:41001") ||
            p->projType.toUpper().contains("EPSG:3785") ||
            p->projType.toUpper().contains("EPSG:900913") ||
            p->projType.toUpper().contains("EPSG:3857")
            )
    {
        p->IsMercator = true;
        p->projType = "EPSG:3857";
        return true;
    }
    // Hardcode "lat/long " projection
    if (
            p->projType.toUpper().contains("EPSG:4326")
            )
    {
        p->IsLatLong = true;
        p->projType = "EPSG:4326";
        return true;
    }

#ifndef _MOBILE
    try {
        p->projProj4 = M_PREFS->getProjection(aProjectionType).projection;
        theProj = getProjection(p->projProj4);
        if (!theProj) {
            p->projType = "EPSG:3857";
            p->IsMercator = true;
            return false;
        }
//        else {
//            if (pj_is_latlong(theProj))
//                p->projType = "EPSG:4326";
//                p->IsLatLong = true;
//        }
    } catch (...) {
        return false;
    }
    return (theProj != NULL || p->IsLatLong || p->IsMercator);
#else
    return false;
#endif // _MOBILE
}

QString Projection::getProjectionType() const
{
    return p->projType;
}

QString Projection::getProjectionProj4() const
{
    if (p->IsLatLong)
        return "+init=EPSG:4326";
    else if (p->IsMercator)
        return "+init=EPSG:3857";
#ifndef _MOBILE
    else
        return QString(pj_get_def(theProj, 0));
#endif
    return QString();
}

int Projection::projectionRevision() const
{
    return p->ProjectionRevision;
}

// Common routines

qreal Projection::latAnglePerM() const
{
    qreal LengthOfOneDegreeLat = EQUATORIALRADIUS * M_PI / 180;
    return 1 / LengthOfOneDegreeLat;
}

qreal Projection::lonAnglePerM(qreal Lat) const
{
    qreal LengthOfOneDegreeLat = EQUATORIALRADIUS * M_PI / 180;
    qreal LengthOfOneDegreeLon = LengthOfOneDegreeLat * fabs(cos(Lat));
    return 1 / LengthOfOneDegreeLon;
}

bool Projection::toXML(QXmlStreamWriter& stream)
{
    bool OK = true;

    stream.writeStartElement("Projection");
    stream.writeAttribute("type", p->projType);
    if (!p->IsLatLong && !p->IsMercator && !p->projProj4.isEmpty()) {
        stream.writeCharacters(p->projProj4);
    }
    stream.writeEndElement();


    return OK;
}

void Projection::fromXML(QXmlStreamReader& stream)
{
    if (stream.name() == "Projection") {
        QString proj;
        if (stream.attributes().hasAttribute("type"))
            proj = stream.attributes().value("type").toString();
        else
            proj = QCoreApplication::translate("Projection", "Document");
        stream.readNext();
        if (stream.tokenType() == QXmlStreamReader::Characters) {
            setProjectionType(stream.text().toString());
            p->projType = proj;
            stream.readNext();
        } else
            setProjectionType(proj);
    }
}
