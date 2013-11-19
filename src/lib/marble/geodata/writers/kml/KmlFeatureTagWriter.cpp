//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlFeatureTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoDataOverlay.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataDocument.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataExtendedData.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

KmlFeatureTagWriter::KmlFeatureTagWriter(const QString &elementName)
    : m_elementName( elementName )
{
    // nothing to do
}

bool KmlFeatureTagWriter::write( const Marble::GeoNode *node, GeoWriter &writer ) const
{
    if ( node->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        const GeoDataDocument *document = static_cast<const GeoDataDocument*>(node);

        // when a document has only one feature and no styling
        // the document tag is excused
        if( (document->styles().count() == 0)
            && (document->styleMaps().count() == 0)
            && (document->extendedData().isEmpty())
            && (document->featureList().count() == 1) ) {
            writeElement( document->featureList()[0], writer );
            return true;
        }
    }

    writer.writeStartElement( m_elementName );

    GeoDataFeature const *feature = static_cast<const GeoDataFeature*>(node);
    writer.writeOptionalElement( kml::kmlTag_name, feature->name() );
    writer.writeOptionalElement( kml::kmlTag_visibility, QString::number( feature->isVisible() ), "1" );
    writer.writeOptionalElement( "address", feature->address() );

    if( !feature->description().isEmpty() ) {
        writer.writeStartElement( "description" );
        if( feature->descriptionIsCDATA() ) {
            writer.writeCDATA( feature->description() );
        } else {
            writer.writeCharacters( feature->description() );
        }
        writer.writeEndElement();
    }

    if( feature->timeStamp().when().isValid() ) {
        writeElement( &feature->timeStamp(), writer );
    }

    if( feature->timeSpan().isValid() ) {
        writeElement( &feature->timeSpan(), writer );
    }

    if( !feature->extendedData().isEmpty() ) {
        writeElement( &feature->extendedData(), writer );
    }

    bool const result = writeMid( node, writer );
    writer.writeEndElement();
    return result;
}

}
