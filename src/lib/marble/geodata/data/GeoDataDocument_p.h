// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATADOCUMENTPRIVATE_H
#define MARBLE_GEODATADOCUMENTPRIVATE_H

#include "GeoDataContainer_p.h"
#include "GeoDataNetworkLinkControl.h"
#include "GeoDataSchema.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDocumentPrivate : public GeoDataContainerPrivate
{
public:
    GeoDataDocumentPrivate()
        : m_documentRole(UnknownDocument)
    {
    }

    GeoDataDocumentPrivate(const GeoDataDocumentPrivate &other)
        : GeoDataContainerPrivate(other)
        , m_styleHash(other.m_styleHash)
        , m_styleMapHash(other.m_styleMapHash)
        , m_schemaHash(other.m_schemaHash)
        , m_filename(other.m_filename)
        , m_baseUri(other.m_baseUri)
        , m_networkLinkControl(other.m_networkLinkControl)
        , m_property(other.m_property)
        , m_documentRole(other.m_documentRole)
    {
    }

    EnumFeatureId featureId() const override
    {
        return GeoDataDocumentId;
    }

    QMap<QString, GeoDataStyle::Ptr> m_styleHash;
    QMap<QString, GeoDataStyleMap> m_styleMapHash;
    QMap<QString, GeoDataSchema> m_schemaHash;
    QString m_filename;
    QString m_baseUri;
    GeoDataNetworkLinkControl m_networkLinkControl;
    QString m_property;
    DocumentRole m_documentRole;
};

} // namespace Marble

#endif
