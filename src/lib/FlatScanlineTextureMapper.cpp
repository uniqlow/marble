//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//


#include"FlatScanlineTextureMapper.h"

#include <cmath>

#include <QtCore/QDebug>

#include "GeoDataPoint.h"
#include "GeoPolygon.h"
#include "MarbleDirs.h"
#include "TextureTile.h"
#include "TileLoader.h"

FlatScanlineTextureMapper::FlatScanlineTextureMapper( TileLoader *tileLoader, QObject * parent )
    : AbstractScanlineTextureMapper( tileLoader, parent )
{
    m_oldCenterLon   = 0.0;
    m_oldYPaintedTop = 0;
}


void FlatScanlineTextureMapper::mapTexture( ViewParams *viewParams )
{
    QImage* canvasImage = viewParams->m_canvasImage;
    const int radius = viewParams->m_radius;

   // Initialize needed variables:
    double lon = 0.0;
    double lat = 0.0;

    m_tilePosX = 65535;
    m_tilePosY = 65535;

    m_toTileCoordinatesLon = (double)(m_tileLoader->globalWidth( m_tileLevel ) 
                             / 2 - m_tilePosX);
    m_toTileCoordinatesLat = (double)(m_tileLoader->globalHeight( m_tileLevel ) 
                             / 2 - m_tilePosY);

    int yTop;
    int yPaintedTop;
    int yPaintedBottom;

    // Reset backend
    m_tileLoader->resetTilehash();
    selectTileLevel( viewParams );

    // Calculate translation of center point
    double centerLon, centerLat;
    viewParams->centerCoordinates( centerLon, centerLat );

    int yCenterOffset =  (int)((float)( 2 * radius / M_PI) * centerLat );

    //Calculate y-range the represented by the center point, yTop and yBottom 
    //and what actually can be painted
    if( viewParams->m_projection == Equirectangular ) {
        yPaintedTop    = yTop = m_imageHeight / 2 - radius + yCenterOffset;
        yPaintedBottom        = m_imageHeight / 2 + radius + yCenterOffset;
    }
    else if( viewParams->m_projection == Mercator ) {
        yPaintedTop    = yTop = m_imageHeight / 2 - 2 * radius + yCenterOffset;
        yPaintedBottom        = m_imageHeight / 2 + 2 * radius + yCenterOffset;
    }

    if (yPaintedTop < 0)                yPaintedTop = 0;
    if (yPaintedTop > m_imageHeight)    yPaintedTop = m_imageHeight;
    if (yPaintedBottom < 0)             yPaintedBottom = 0;
    if (yPaintedBottom > m_imageHeight) yPaintedBottom = m_imageHeight;

    // Calculate how many degrees are being represented per pixel.
    const float rad2Pixel = M_PI / (float)( 2 * radius );

    float leftLon = + centerLon - ( rad2Pixel * m_imageWidth / 2 );
    while ( leftLon < -M_PI ) leftLon += 2 * M_PI;
    while ( leftLon >  M_PI ) leftLon -= 2 * M_PI;
    // Paint the map.
    for ( int y = yPaintedTop ;y < yPaintedBottom; ++y ) {
        if( viewParams->m_projection == Equirectangular )
            lat = M_PI/2 - (y - yTop )* rad2Pixel;
        else if( viewParams->m_projection == Mercator )
            lat = atan( sinh( ((m_imageHeight / 2 + yCenterOffset) - y) / (double)(2 * radius) * M_PI ) );
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        lon = leftLon;

        QRgb * scanLineBegin = m_scanLine;
        const QRgb * scanLineEnd   = m_scanLine + m_imageWidth;

        for ( QRgb * scanLine = scanLineBegin; scanLine < scanLineEnd;
              ++scanLine ) {
            lon += rad2Pixel;
            if ( lon < -M_PI ) lon += 2 * M_PI;
            if ( lon >  M_PI ) lon -= 2 * M_PI;
            pixelValue( lon, lat, scanLine );
        }
    }

    // Remove unused lines
    const int clearStart = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? yPaintedBottom : 0;
    const int clearStop  = ( yPaintedTop - m_oldYPaintedTop <= 0 ) ? m_imageHeight  : yTop;

    for ( int y = clearStart; y < clearStop; ++y ) {
        m_scanLine = (QRgb*)( canvasImage->scanLine( y ) );
        for ( int x = 0; x < m_imageWidth; ++x ) {
            *(m_scanLine + x) = 0;
        }
    }
    m_oldYPaintedTop = yPaintedTop;

    m_tileLoader->cleanupTilehash();
}


#include "FlatScanlineTextureMapper.moc"
