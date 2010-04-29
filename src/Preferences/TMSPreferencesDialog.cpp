//
// C++ Implementation: TMSPreferencesDialog
//
// Description:
//
//
// Author: cbro <cbro@semperpax.com>, Bart Vanhauwaert (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Preferences/TMSPreferencesDialog.h"

#include <QMessageBox>
#include <QDir>
#include <QUrl>
#include <QTextEdit>
#include <QComboBox>

TMSPreferencesDialog::TMSPreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);
    frOSGeo->setVisible(false);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    loadPrefs();
}

TMSPreferencesDialog::~TMSPreferencesDialog()
{
}

void TMSPreferencesDialog::addServer(const TmsServer & srv)
{
    theTmsServers.push_back(srv);
    if (!srv.deleted) {
        QListWidgetItem* item = new QListWidgetItem(srv.TmsName);
        item->setData(Qt::UserRole, (int) theTmsServers.size()-1);
        lvTmsServers->addItem(item);
    }
}

void TMSPreferencesDialog::on_btApplyTmsServer_clicked(void)
{
    int idx = static_cast<int>(lvTmsServers->currentItem()->data(Qt::UserRole).toInt());
    if (idx >= theTmsServers.size())
        return;

    QUrl url(edTmsUrl->text());

    TmsServer& WS(theTmsServers[idx]);
    WS.TmsName = edTmsName->text();
    WS.TmsAdress = url.host();
    WS.TmsPath = url.path();
    WS.TmsTileSize = sbTileSize->value();
    WS.TmsMinZoom = sbMinZoom->value();
    WS.TmsMaxZoom = sbMaxZoom->value();

    lvTmsServers->currentItem()->setText(WS.TmsName);
    selectedServer = WS.TmsName;
}

void TMSPreferencesDialog::on_btAddTmsServer_clicked(void)
{
    QUrl url(edTmsUrl->text());

    addServer(TmsServer(edTmsName->text(), url.host(), url.path(), sbTileSize->value(), sbMinZoom->value(), sbMaxZoom->value()));
    lvTmsServers->setCurrentRow(lvTmsServers->count() - 1);
    on_lvTmsServers_itemSelectionChanged();
}

void TMSPreferencesDialog::on_btDelTmsServer_clicked(void)
{
    int idx = static_cast<int>(lvTmsServers->currentItem()->data(Qt::UserRole).toInt());
    if (idx >= theTmsServers.size())
        return;

    theTmsServers[idx].deleted = true;
    delete lvTmsServers->takeItem(idx);
    on_lvTmsServers_itemSelectionChanged();
}

void TMSPreferencesDialog::on_lvTmsServers_itemSelectionChanged()
{
    QListWidgetItem* it = lvTmsServers->item(lvTmsServers->currentRow());

    int idx = it->data(Qt::UserRole).toInt();
    if (idx >= theTmsServers.size())
        return;

    QUrl url;

    TmsServer& WS(theTmsServers[idx]);
    edTmsName->setText(WS.TmsName);
    url.setScheme("http");
    url.setHost(WS.TmsAdress);
    url.setPath(WS.TmsPath);
    edTmsUrl->setText(url.toString());
    sbTileSize->setValue(WS.TmsTileSize);
    sbMinZoom->setValue(WS.TmsMinZoom);
    sbMaxZoom->setValue(WS.TmsMaxZoom);

    selectedServer = WS.TmsName;
}

QString TMSPreferencesDialog::getSelectedServer()
{
    return selectedServer;
}

void TMSPreferencesDialog::setSelectedServer(QString theValue)
{
    QList<QListWidgetItem *> L = lvTmsServers->findItems(theValue, Qt::MatchExactly);
    lvTmsServers->setCurrentItem(L[0]);
    on_lvTmsServers_itemSelectionChanged();
}

void TMSPreferencesDialog::on_buttonBox_clicked(QAbstractButton * button)
{
    if ((button == buttonBox->button(QDialogButtonBox::Apply))) {
        savePrefs();
    } else
        if ((button == buttonBox->button(QDialogButtonBox::Ok))) {
            savePrefs();
            this->accept();
        }
}

void TMSPreferencesDialog::loadPrefs()
{
    TmsServerList* L = MerkaartorPreferences::instance()->getTmsServers();
    TmsServerListIterator i(*L);
    while (i.hasNext()) {
        i.next();
        addServer(i.value());
    }
}

void TMSPreferencesDialog::savePrefs()
{
    TmsServerList* L = MerkaartorPreferences::instance()->getTmsServers();
    L->clear();
    for (int i = 0; i < theTmsServers.size(); ++i) {
        TmsServer S(theTmsServers[i]);
        L->insert(theTmsServers[i].TmsName, S);
    }
    //MerkaartorPreferences::instance()->setSelectedTmsServer(getSelectedServer());
    M_PREFS->save();
}

void TMSPreferencesDialog::on_btGetServices_clicked()
{
    QUrl theUrl(edTmsUrl->text());
    if ((theUrl.host() == "") || (theUrl.path() == "")) {
        QMessageBox::critical(this, tr("Merkaartor: GetServices"), tr("Address and Path cannot be blank."), QMessageBox::Ok);
    }

    lvTmsServices->clear();

    http = new QHttp(this);
    connect (http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
        this, SLOT(readResponseHeader(const QHttpResponseHeader &)));

    httpGetId = sendRequest(theUrl);

}

int TMSPreferencesDialog::sendRequest(QUrl url)
{
    QString requestUrl = url.encodedPath();
    if (!url.encodedQuery().isNull())
        requestUrl += "?" + url.encodedQuery();
    QHttpRequestHeader header("GET", requestUrl);
    qDebug() << header.toString();
    const char *userAgent = "Mozilla/9.876 (X11; U; Linux 2.2.12-20 i686, en) Gecko/25250101 Netscape/5.432b1";

    QString host = url.host();
    if (url.port() != -1)
        host += ":" + QString::number(url.port());
    header.setValue("Host", host);
    header.setValue("User-Agent", userAgent);

    http->setHost(url.host(), url.port() == -1 ? 80 : url.port());
    http->setProxy(M_PREFS->getProxy(url));

    return http->request(header);
}

void TMSPreferencesDialog::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    qDebug() << responseHeader.toString();
    switch (responseHeader.statusCode())
    {
        case 200:
            break;

        case 301:
        case 302:
        case 307:
            http->abort();
            sendRequest(QUrl(responseHeader.value("Location")));
            break;

        default:
            http->abort();
            QMessageBox::information(this, tr("Merkaartor: GetServices"),
                                  tr("Download failed: %1.")
                                  .arg(responseHeader.reasonPhrase()));
    }
}

void TMSPreferencesDialog::httpRequestFinished(int id, bool error)
{
    if (error) {
        if (http->error() != QHttp::Aborted)
            QMessageBox::critical(this, tr("Merkaartor: GetServices"), tr("Error reading services.\n") + http->errorString(), QMessageBox::Ok);
        return;
    }

    QDomDocument doc;
    QString content = http->readAll();
    qDebug() << content;
    doc.setContent(content);
    if (doc.isNull())
        return;

    QDomElement e = doc.firstChildElement();
    if (e.nodeName().toLower() == "services") {
        QDomNodeList l = e.elementsByTagName("TileMapService");
        for (int i=0; i<l.size(); ++i) {
            QString href = l.at(i).toElement().attribute("href");
            if (!href.isNull()) {
                sendRequest(QUrl(href));
            }
        }
    } else if (e.nodeName().toLower() == "tilemapservice") {
        QDomNodeList l = e.elementsByTagName("TileMap");
        for (int i=0; i<l.size(); ++i) {
            QString href = l.at(i).toElement().attribute("href");
            if (!href.isNull()) {
                sendRequest(QUrl(href));
            }
        }
    } else if (e.nodeName().toLower() == "tilemap") {
        QString url;
        QString title;
        QString srs;
        CoordBox bbox;
        Coord origin;
        QSize tilesize;
        QString tileformat;
        int minzoom = 9999, maxzoom = 0;

        url = e.attribute("tilemapservice");
        QDomElement c = e.firstChildElement();
        while (!c.isNull()) {
            if (c.nodeName().toLower() == "title") {
                title = c.firstChild().toText().nodeValue();
                url = url + title + "/";
            } else if (c.nodeName().toLower() == "srs") {
                srs = c.firstChild().toText().nodeValue();
            } else if (c.nodeName().toLower() == "boundingbox") {
                Coord bl(angToInt(c.attribute("miny").toDouble()), angToInt(c.attribute("minx").toDouble()));
                Coord tr(angToInt(c.attribute("maxy").toDouble()), angToInt(c.attribute("maxx").toDouble()));
                bbox = CoordBox(bl, tr);
            } else if (c.nodeName().toLower() == "origin") {
                Coord pt(angToInt(c.attribute("y").toDouble()), angToInt(c.attribute("x").toDouble()));
                origin = pt;
            } else if (c.nodeName().toLower() == "tileformat") {
                tilesize.setWidth(c.attribute("width").toInt());
                tilesize.setHeight(c.attribute("height").toInt());
                tileformat = c.attribute("extension");
            } else if (c.nodeName().toLower() == "tilesets") {
                QDomElement t = c.firstChildElement();
                while (!t.isNull()) {
                    int o = t.attribute("order").toInt();
                    minzoom = o < minzoom ? o : minzoom;
                    maxzoom = o > maxzoom ? o : maxzoom;
                    t = t.nextSiblingElement();
                }
            }
            c = c.nextSiblingElement();
        }
        lvTmsServices->addItem(title);
        frOSGeo->setVisible(true);
        frOSM->setVisible(false);
    }
}
