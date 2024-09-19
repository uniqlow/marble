
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlAnimatedUpdateTagWriter.h"

#include "GeoDataAnimatedUpdate.h"
#include "GeoDataTypes.h"
#include "GeoDataUpdate.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerAnimatedUpdate(GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataAnimatedUpdateType, kml::kmlTag_nameSpaceOgc22),
                                                    new KmlAnimatedUpdateTagWriter);

bool KmlAnimatedUpdateTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    Q_ASSERT(dynamic_cast<const GeoDataAnimatedUpdate *>(node));
    const GeoDataAnimatedUpdate *animUpdate = static_cast<const GeoDataAnimatedUpdate *>(node);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_nameSpaceGx22), QString::fromUtf8(kml::kmlTag_AnimatedUpdate));
    KmlObjectTagWriter::writeIdentifiers(writer, animUpdate);
    writer.writeOptionalElement("gx:duration", animUpdate->duration(), 0.0);
    if (animUpdate->update()) {
        GeoDataUpdate const *update = dynamic_cast<const GeoDataUpdate *>(animUpdate->update());
        if (update) {
            writeElement(update, writer);
        }
    }
    writer.writeOptionalElement("gx:delayedStart", animUpdate->delayedStart(), 0.0);
    writer.writeEndElement();
    return true;
}

}
