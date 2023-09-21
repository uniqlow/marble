// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlModelTagWriter.h"

#include "GeoDataModel.h"
#include "GeoDataAlias.h"
#include "GeoDataTypes.h"
#include "GeoDataLocation.h"
#include "GeoDataOrientation.h"
#include "GeoDataResourceMap.h"
#include "GeoDataScale.h"
#include "GeoWriter.h"
#include "KmlGroundOverlayWriter.h"
#include "KmlLinkTagWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerModel(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataModelType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlModelTagWriter);

bool KmlModelTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataModel *model = static_cast<const GeoDataModel*>( node );

    writer.writeStartElement( QString::fromUtf8(kml::kmlTag_Model) );
    KmlObjectTagWriter::writeIdentifiers( writer, model );

    KmlGroundOverlayWriter::writeAltitudeMode( writer, model->altitudeMode());

    const GeoDataLocation location = model->location() ;

    writer.writeStartElement( QString::fromUtf8(kml::kmlTag_Location) );

    writer.writeOptionalElement( kml::kmlTag_longitude, QString::number( location.longitude( GeoDataCoordinates::Degree ) ), "0" );
    writer.writeOptionalElement( kml::kmlTag_latitude, QString::number( location.latitude( GeoDataCoordinates::Degree ) ), "0" );
    writer.writeOptionalElement( kml::kmlTag_altitude, QString::number( location.altitude() ), "0" );

    writer.writeEndElement();

    const GeoDataOrientation orientation = model->orientation();

    writer.writeStartElement( QString::fromUtf8(kml::kmlTag_Orientation) );

    writer.writeOptionalElement( kml::kmlTag_heading, QString::number( orientation.heading() ), "0" );
    writer.writeOptionalElement( kml::kmlTag_tilt, QString::number( orientation.tilt() ), "0" );
    writer.writeOptionalElement( kml::kmlTag_roll, QString::number( orientation.roll() ), "0" );

    writer.writeEndElement();

    const GeoDataScale scale = model->scale() ;

    writer.writeStartElement( QString::fromUtf8(kml::kmlTag_Scale) );

    writer.writeOptionalElement( kml::kmlTag_x, QString::number( scale.x() ), "1" );
    writer.writeOptionalElement( kml::kmlTag_y, QString::number( scale.y() ), "1" );
    writer.writeOptionalElement( kml::kmlTag_z, QString::number( scale.z() ), "1" );

    writer.writeEndElement();

    writeElement( &model->link(), writer );

    const GeoDataResourceMap map = model->resourceMap() ;

    writer.writeStartElement( QString::fromUtf8(kml::kmlTag_ResourceMap) );

    const GeoDataAlias alias = map.alias() ;
    writer.writeStartElement( QString::fromUtf8(kml::kmlTag_Alias) );

    writer.writeTextElement( QString::fromUtf8(kml::kmlTag_targetHref), alias.targetHref() );
    writer.writeTextElement( QString::fromUtf8(kml::kmlTag_sourceHref), alias.sourceHref() );

    writer.writeEndElement();

    writer.writeEndElement();

    writer.writeEndElement();
    return true;
}

}
