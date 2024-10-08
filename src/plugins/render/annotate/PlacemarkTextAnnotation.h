// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2013 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
//

#ifndef PLACEMARKTEXTANNOTATION_H
#define PLACEMARKTEXTANNOTATION_H

#include "SceneGraphicsItem.h"
#include <QColor>

namespace Marble
{

class PlacemarkTextAnnotation : public SceneGraphicsItem
{
public:
    explicit PlacemarkTextAnnotation(GeoDataPlacemark *placemark);
    ~PlacemarkTextAnnotation() override;

    void paint(GeoPainter *painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel) override;

    bool containsPoint(const QPoint &eventPos) const override;

    void dealWithItemChange(const SceneGraphicsItem *other) override;

    void move(const GeoDataCoordinates &source, const GeoDataCoordinates &destination) override;

    /**
     * @brief Provides information for downcasting a SceneGraphicsItem.
     */
    const char *graphicType() const override;

    /**
     * @brief Real label color, which is being hidden when placemark has focus
     */
    QColor labelColor() const;

protected:
    bool mousePressEvent(QMouseEvent *event) override;
    bool mouseMoveEvent(QMouseEvent *event) override;
    bool mouseReleaseEvent(QMouseEvent *event) override;

    void dealWithStateChange(SceneGraphicsItem::ActionState previousState) override;

private:
    const ViewportParams *m_viewport;
    bool m_movingPlacemark;
    QColor m_labelColor;

    QRegion m_region;
};

}

#endif // PLACEMARKTEXTANNOTATION_H
