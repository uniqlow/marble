/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef GEOSCENEITEM_H
#define GEOSCENEITEM_H

#include <QtCore/QString>

#include "GeoDocument.h"

/**
 * @short The section item in a legend of a GeoScene document.
 */
class GeoSceneItem : public GeoNode {
 public:
    GeoSceneItem();
    ~GeoSceneItem();

    QString text() const;
    void setText(const QString& text);

    QString pixmap() const;
    void setPixmap(const QString& pixmap);

    QString color() const;
    void setColor(const QString& color);

 protected:
    QString m_text;
    QString m_pixmap;
    QString m_color;
};


#endif // GEOSCENEITEM_H
