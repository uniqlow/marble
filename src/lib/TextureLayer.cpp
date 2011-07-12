//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>//

#include "TextureLayer.h"

#include <QtCore/qmath.h>
#include <QtCore/QCache>
#include <QtCore/QPointer>
#include <QtCore/QTimer>

#include "SphericalScanlineTextureMapper.h"
#include "EquirectScanlineTextureMapper.h"
#include "MercatorScanlineTextureMapper.h"
#include "TileScalingTextureMapper.h"
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneGroup.h"
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoSceneSettings.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "StackedTile.h"
#include "StackedTileLoader.h"
#include "TextureColorizer.h"
#include "ViewParams.h"

namespace Marble
{

class TextureLayer::Private
{
public:
    Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator, TextureLayer *parent );

    void mapChanged();
    void updateTextureLayers();

    const GeoSceneLayer *sceneLayer() const;
    GeoSceneGroup *textureLayerSettings() const;

public:
    TextureLayer  *const m_parent;
    StackedTileLoader    m_tileLoader;
    QCache<TileId, QPixmap> m_pixmapCache;
    TextureMapperInterface *m_texmapper;
    QPointer<TextureColorizer> m_texcolorizer;
    GeoSceneDocument              *m_mapTheme;
};

TextureLayer::Private::Private( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator, TextureLayer *parent )
    : m_parent( parent )
    , m_tileLoader( downloadManager, mapThemeManager, sunLocator )
    , m_pixmapCache( 100 )
    , m_texmapper( 0 )
    , m_texcolorizer( 0 )
    , m_mapTheme( 0 )
{
}

void TextureLayer::Private::mapChanged()
{
    if ( m_texmapper ) {
        m_texmapper->setRepaintNeeded();
    }

    emit m_parent->repaintNeeded( QRegion() );
}

void TextureLayer::Private::updateTextureLayers()
{
    QVector<GeoSceneTexture const *> result;

    foreach ( const GeoSceneAbstractDataset *pos, sceneLayer()->datasets() ) {
        GeoSceneTexture const * const candidate = dynamic_cast<GeoSceneTexture const *>( pos );
        if ( !candidate )
            continue;
        bool enabled = true;
        if ( textureLayerSettings() ) {
            const bool propertyExists = textureLayerSettings()->propertyValue( candidate->name(), enabled );
            enabled |= !propertyExists; // if property doesn't exist, enable texture nevertheless
        }
        if ( enabled )
            result.append( candidate );
    }

    m_tileLoader.setTextureLayers( result );
    m_pixmapCache.clear();
}

const GeoSceneLayer *TextureLayer::Private::sceneLayer() const
{
    if ( !m_mapTheme )
        return 0;

    GeoSceneHead const * head = m_mapTheme->head();
    if ( !head )
        return 0;

    GeoSceneMap const * map = m_mapTheme->map();
    if ( !map )
        return 0;

    const QString mapThemeId = head->target() + '/' + head->theme();
    mDebug() << "StackedTileLoader::updateTextureLayers" << mapThemeId;

    return map->layer( head->theme() );
}

GeoSceneGroup* TextureLayer::Private::textureLayerSettings() const
{
    if ( !m_mapTheme )
        return 0;

    if ( !m_mapTheme->settings() )
        return 0;

    return m_mapTheme->settings()->group( "Texture Layers" );
}




TextureLayer::TextureLayer( MapThemeManager *mapThemeManager, HttpDownloadManager *downloadManager, SunLocator *sunLocator )
    : QObject()
    , d( new Private( mapThemeManager, downloadManager, sunLocator, this ) )
{
}

TextureLayer::~TextureLayer()
{
    delete d;
}

void TextureLayer::paintGlobe( GeoPainter *painter,
                               ViewParams *viewParams,
                               const QRect& dirtyRect )
{
    if ( !d->m_mapTheme )
        return;

    if ( !d->m_mapTheme->map()->hasTextureLayers() )
        return;

    if ( !d->m_texmapper )
        return;

    // choose the smaller dimension for selecting the tile level, leading to higher-resolution results
    const int levelZeroWidth = d->m_tileLoader.tileSize().width() * d->m_tileLoader.tileColumnCount( 0 );
    const int levelZeroHight = d->m_tileLoader.tileSize().height() * d->m_tileLoader.tileRowCount( 0 );
    const int levelZeroMinDimension = ( levelZeroWidth < levelZeroHight ) ? levelZeroWidth : levelZeroHight;

    qreal linearLevel = ( 4.0 * (qreal)( viewParams->radius() ) / (qreal)( levelZeroMinDimension ) );

    if ( linearLevel < 1.0 )
        linearLevel = 1.0; // Dirty fix for invalid entry linearLevel

    // As our tile resolution doubles with each level we calculate
    // the tile level from tilesize and the globe radius via log(2)

    qreal tileLevelF = log( linearLevel ) / log( 2.0 );
    int tileLevel = (int)( tileLevelF );

//    mDebug() << "tileLevelF: " << tileLevelF << " tileLevel: " << tileLevel;

    if ( tileLevel > d->m_tileLoader.maximumTileLevel() )
        tileLevel = d->m_tileLoader.maximumTileLevel();

    const bool changedTileLevel = tileLevel != d->m_texmapper->tileZoomLevel();

    //    mDebug() << "Texture Level was set to: " << tileLevel;
    d->m_texmapper->setTileLevel( tileLevel );

    if ( changedTileLevel ) {
        emit tileLevelChanged( tileLevel );
    }

    d->m_texmapper->mapTexture( painter, viewParams, dirtyRect, d->m_texcolorizer );
}

void TextureLayer::setShowTileId( bool show )
{
    d->m_tileLoader.setShowTileId( show );
}

void TextureLayer::setTextureColorizer( TextureColorizer *texcolorizer )
{
    if ( d->m_texcolorizer ) {
        disconnect( d->m_texcolorizer, 0, this, 0 );
    }

    d->m_texcolorizer = texcolorizer;

    if ( d->m_texcolorizer ) {
        connect( d->m_texcolorizer, SIGNAL( datasetLoaded() ), SLOT( mapChanged() ) );
    }
}

void TextureLayer::setupTextureMapper( Projection projection )
{
    if ( !d->m_mapTheme || !d->m_mapTheme->map()->hasTextureLayers() )
        return;
  // FIXME: replace this with an approach based on the factory method pattern.
    delete d->m_texmapper;

    switch( projection ) {
        case Spherical:
            d->m_texmapper = new SphericalScanlineTextureMapper( &d->m_tileLoader, this );
            break;
        case Equirectangular:
            d->m_texmapper = new EquirectScanlineTextureMapper( &d->m_tileLoader, this );
            break;
        case Mercator:
            if ( d->m_tileLoader.tileProjection() == GeoSceneTexture::Mercator ) {
                d->m_texmapper = new TileScalingTextureMapper( &d->m_tileLoader, &d->m_pixmapCache, this );
            } else {
                d->m_texmapper = new MercatorScanlineTextureMapper( &d->m_tileLoader, this );
            }
            break;
        default:
            d->m_texmapper = 0;
    }
    Q_ASSERT( d->m_texmapper );
    connect( d->m_texmapper, SIGNAL( tileUpdatesAvailable() ), SLOT( mapChanged() ) );
}

void TextureLayer::setNeedsUpdate()
{
    if ( d->m_texmapper ) {
        d->m_texmapper->setRepaintNeeded();
    }
}

void TextureLayer::setVolatileCacheLimit( quint64 kilobytes )
{
    d->m_tileLoader.setVolatileCacheLimit( kilobytes );
}

void TextureLayer::update()
{
    mDebug() << "TextureLayer::update()";
    d->m_tileLoader.clear();
    d->mapChanged();
}

void TextureLayer::reload()
{
    d->m_tileLoader.reloadVisibleTiles();
}

void TextureLayer::downloadTile( const TileId &tileId )
{
    d->m_tileLoader.downloadTile( tileId );
}

void TextureLayer::setMapTheme(GeoSceneDocument* mapTheme)
{
    if ( d->textureLayerSettings() ) {
        disconnect( d->textureLayerSettings(), SIGNAL( valueChanged( QString, bool ) ),
                    this,                      SLOT( updateTextureLayers() ) );
    }

    d->m_mapTheme = mapTheme;

    if ( d->textureLayerSettings() ) {
        connect( d->textureLayerSettings(), SIGNAL( valueChanged( QString, bool )),
                 this,                      SLOT( updateTextureLayers() ) );
    }
    d->updateTextureLayers();
}

int TextureLayer::tileZoomLevel() const
{
    if (!d->m_texmapper)
        return -1;

    return d->m_texmapper->tileZoomLevel();
}

QSize TextureLayer::tileSize() const
{
    return d->m_tileLoader.tileSize();
}

GeoSceneTexture::Projection TextureLayer::tileProjection() const
{
    return d->m_tileLoader.tileProjection();
}

int TextureLayer::tileColumnCount( int level ) const
{
    return d->m_tileLoader.tileColumnCount( level );
}

int TextureLayer::tileRowCount( int level ) const
{
    return d->m_tileLoader.tileRowCount( level );
}

qint64 TextureLayer::volatileCacheLimit() const
{
    return d->m_tileLoader.volatileCacheLimit();
}

int TextureLayer::preferredRadiusCeil( int radius ) const
{
    const int tileWidth = d->m_tileLoader.tileSize().width();
    const int levelZeroColumns = d->m_tileLoader.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = log( linearLevel ) / log( 2.0 );
    const int tileLevel = qCeil( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

int TextureLayer::preferredRadiusFloor( int radius ) const
{
    const int tileWidth = d->m_tileLoader.tileSize().width();
    const int levelZeroColumns = d->m_tileLoader.tileColumnCount( 0 );
    const qreal linearLevel = 4.0 * (qreal)( radius ) / (qreal)( tileWidth * levelZeroColumns );
    const qreal tileLevelF = log( linearLevel ) / log( 2.0 );
    const int tileLevel = qFloor( tileLevelF );

    if ( tileLevel < 0 )
        return ( tileWidth * levelZeroColumns / 4 ) >> (-tileLevel);

    return ( tileWidth * levelZeroColumns / 4 ) << tileLevel;
}

}

#include "TextureLayer.moc"
