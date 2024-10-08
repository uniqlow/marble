// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMOBJECTMANAGER_H
#define MARBLE_OSMOBJECTMANAGER_H

#include <QtGlobal>
#include <marble_export.h>

namespace Marble
{

class GeoDataPlacemark;

/**
 * @brief The OsmObjectManager class is used to assign osmData to placemarks that
 * do not have them at write time. Objects with no osmData usually consist of
 * new placemarks created in the editor and placemarks loaded from KML files.
 */
class MARBLE_EXPORT OsmObjectManager
{
public:
    /**
     * @brief initializeOsmData assigns valid osmData
     * to a placemark that does not have it.
     */
    static void initializeOsmData(GeoDataPlacemark *placemark);

    /**
     * @brief registerId is used to keep track of the minimum id @see m_minId
     */
    static void registerId(qint64 id);

private:
    /**
     * @brief newly created placemarks are assigned negative unique IDs.
     * In order to assure there are no duplicate IDs, they are assigned the
     * minId - 1 id.
     */
    static qint64 m_minId;
};

}

#endif
