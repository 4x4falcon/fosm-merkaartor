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

#include "NavitBin.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>

#include <QDataStream>

NavitBin::NavitBin()
{
}

bool NavitBin::setFilename(QString filename)
{
    zip = new QuaZip(filename);
    if(!zip->open(QuaZip::mdUnzip)) {
        QMessageBox::critical(0,QCoreApplication::translate("NavitBackground","Not a valid file"),QCoreApplication::translate("NavitBackground","Cannot load file."));
        return false;
    }
    file = new QuaZipFile(zip);

    readTile("index");

    return true;
}

void NavitBin::readTile(QString fn)
{
    zip->setCurrentFile(fn);
    if(!file->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0,QCoreApplication::translate("NavitBackground","Not a valid file"),QCoreApplication::translate("NavitBackground","Cannot load file: ").arg(file->getZipError()));
        return;
    }
    qint32 len;
    qint32 type;
    qint32 coordLen;
    qint32 x, y;
    qint32 attrLen;
    qint32 attrType;
    qint8 attr;

    QByteArray ba = file->readAll();
    QDataStream data(ba);
    data.setByteOrder(QDataStream::LittleEndian);
    while (!data.atEnd()) {
        QByteArray attributes;

        data >> len;
        qDebug() << "Len: " << len;
        data >> type;
        qDebug() << "type: " << type;
        data >> coordLen;
        qDebug() << "coordLen: " << coordLen;
        for (int i=0; i<coordLen/2; ++i) {
            data >> x >> y;
        }
        for (int j=2+1+coordLen; j<len; j+=2) {
            data >> attrLen;
            qDebug() << "attrLen: " << attrLen;
            data >> attrType;
            for (int i=0; i<(attrLen-1)*sizeof(qint32); ++i) {
                data >> attr;
                attributes.append(attr);
            }
            j += attrLen-1;
        }
    }
    file->close();
}
