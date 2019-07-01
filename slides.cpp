#include "slides.h"
#include "minorGems/network/web/server/WebServer.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/game.h"

#include "minorGems/graphics/converters/PNGImageConverter.h"
#include "minorGems/graphics/converters/JPEGImageConverter.h"


static int nextSlideNumber = 0;

static double slideStartTime;



static File slidesDir( NULL, "slides" );

static File thumbsDir( NULL, "slideThumbs" );


static WebServer *server;


typedef struct SlideImage {
        int id;
        File *thumbFile;
        SpriteHandle sprite;
        int w, h;
        char pixelArt;
    } SlideImage;

    
SimpleVector<SlideImage> slides;




void initSlides() {
    // make thumbs
    int numFiles;
    File **childFiles = slidesDir.getChildFiles( &numFiles );

    for( int i=0; i<numFiles; i++ ) {
        
        File *f = childFiles[i];

        char *name = f->getFileName();

        FileInputStream stream( f );

        Image *image = NULL;

        printf( "Opening file %s\n", name );

        if( strstr( name, ".png" ) != NULL ) {
            PNGImageConverter converter;    
            image = converter.deformatImage( &stream );
            }
        else if( strstr( name, ".jpg" ) != NULL ) {
            JPEGImageConverter converter( 90 );
            image = converter.deformatImage( &stream );
            }
        
        
        if( image != NULL ) {    
            SlideImage s;
            s.id = i;
            s.thumbFile = NULL;
            s.sprite = fillSprite( image, false );
            s.w = image->getWidth();
            s.h = image->getHeight();
            
            s.pixelArt = false;
            
            if( strstr( name, "_pixels" ) != NULL ) {
                s.pixelArt = true;
                }

            delete image;
            
            slides.push_back( s );
            }
        
        delete [] name;

        delete f;
        }
    
    delete [] childFiles;

    slideStartTime = game_getCurrentTime();
    }



void freeSlides() {
    for( int i=0; i<slides.size(); i++ ) {
        SlideImage s = slides.getElementDirect( i );
        freeSprite( s.sprite );
        }
    slides.deleteAll();
    }





void drawCurrentSlide( double viewWidth, double viewHeight ) {
    
    if( slides.size() > nextSlideNumber ) {
        SlideImage s = slides.getElementDirect( nextSlideNumber );

        toggleLinearMagFilter( ! s.pixelArt );

        setDrawColor( 1, 1, 1, 1 );
        
        doublePair pos = { 0, 0 };

        double zoom = 1.0;

        if( s.w / viewWidth > s.h / viewHeight ) {
            zoom = viewWidth / s.w;
            }
        else {
            zoom = viewHeight / s.h;
            }
        
        
        drawSprite( s.sprite, pos, zoom );
        }

    if( game_getCurrentTime() - slideStartTime > 3 ) {
        slideStartTime = game_getCurrentTime();
        
        nextSlideNumber++;
        
        if( nextSlideNumber >= slides.size() ) {
            nextSlideNumber = 0;
            }
        }
    }

