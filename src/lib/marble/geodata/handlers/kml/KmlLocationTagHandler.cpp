
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlLocationTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataGeometry.h"
#include "GeoDataLocation.h"
#include "GeoDataModel.h"
#include "GeoDataParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Location)

GeoNode *KmlLocationTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_Location)));

    GeoDataLocation location;
    KmlObjectTagHandler::parseIdentifiers(parser, &location);
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Model)) {
        parentItem.nodeAs<GeoDataModel>()->setLocation(location);
        return &parentItem.nodeAs<GeoDataModel>()->location();
    } else {
        return nullptr;
    }
}

}
}
