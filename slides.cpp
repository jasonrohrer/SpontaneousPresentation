#include "slides.h"
#include "minorGems/network/web/server/WebServer.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/game.h"

#include "minorGems/graphics/converters/PNGImageConverter.h"
#include "minorGems/graphics/converters/JPEGImageConverter.h"


static int nextSlideNumber = 0;

static double slideStartTime;



static File slidesDir( NULL, "slides" );


static WebServer *server;


typedef struct SlideImage {
        int id;
        File *sourceFile;
        SpriteHandle sprite;
        int w, h;
        char pixelArt;
        
        // or PNG
        char isJPEG;
    } SlideImage;

    
SimpleVector<SlideImage> slides;


SlideImage *getSlideByID( int inID ) {
    for( int i=0; i<slides.size(); i++ ) {
        SlideImage *s = slides.getElement( i );
        
        if( s->id == inID ) {
            return s;
            }
        }
    return NULL;
    }




class SlidePageGenerator : public PageGenerator {
    public:
        


        virtual void generatePage( char *inGetRequestPath,
                                   OutputStream *inOutputStream ) {

            printf( "Get request for %s\n", inGetRequestPath );


            // serve images?
            if( strstr( inGetRequestPath, ".jpg" ) != NULL ||
                strstr( inGetRequestPath, ".png" ) != NULL ) {
                int id = 0;
                sscanf( inGetRequestPath, "/%d.", &id );
                
                SlideImage *s = getSlideByID( id );
                
                if( s != NULL ) {
                    int fileLen;
                    unsigned char *fileCont = 
                        s->sourceFile->readFileContents( &fileLen );
                    inOutputStream->write( fileCont, fileLen );
                    delete [] fileCont;
                    }
                }
            else {
                // server index
                
                int newID = -1;
                
                sscanf( inGetRequestPath, "/%d", &newID );
                
                if( newID != -1 ) {
                    nextSlideNumber = newID;
                    }

                for( int i=0; i<slides.size(); i++ ) {
                    SlideImage s = slides.getElementDirect( i );
                    
                    const char *ext = ".png";
                    
                    if( s.isJPEG ) {
                        ext = ".jpg";
                        }

                    char *line =
                        autoSprintf( "<a href=%d>"
                                     "<img src=%d%s width=200 height=200>"
                                     "</a><br>", s.id, s.id, ext );
                    inOutputStream->writeString( line );
                    delete [] line;
                    }

                }
}


        
        virtual char *getMimeType( char *inGetRequestPath ) {            

            if( strstr( inGetRequestPath, ".jpg" ) != NULL ) {
                return stringDuplicate( "image/jpeg" );
                }
            else if( strstr( inGetRequestPath, ".png" ) != NULL ) {
                return stringDuplicate( "image/png" );
                }
            else {
                // text?
                return stringDuplicate( "text/html" );
                }
            }
        
    };
    




void initSlides() {
    
    int numFiles;
    File **childFiles = slidesDir.getChildFiles( &numFiles );

    for( int i=0; i<numFiles; i++ ) {
        
        File *f = childFiles[i];

        char *name = f->getFileName();

        FileInputStream stream( f );

        Image *image = NULL;

        printf( "Opening file %s\n", name );
        
        char isJPEG = false;
        
        if( strstr( name, ".png" ) != NULL ) {
            PNGImageConverter converter;    
            image = converter.deformatImage( &stream );
            }
        else if( strstr( name, ".jpg" ) != NULL ) {
            JPEGImageConverter converter( 90 );
            image = converter.deformatImage( &stream );
            isJPEG = true;
            }
        
        
        if( image != NULL ) {    
            SlideImage s;
            s.id = i;
            s.sourceFile = f;
            s.sprite = fillSprite( image, false );
            s.w = image->getWidth();
            s.h = image->getHeight();
            
            s.pixelArt = false;
            s.isJPEG = isJPEG;
            
            if( strstr( name, "_pixels" ) != NULL ) {
                s.pixelArt = true;
                }

            delete image;
            
            slides.push_back( s );
            }
        
        delete [] name;
        }
    
    delete [] childFiles;

    
    int port = SettingsManager::getIntSetting( "port", 8080 );

    server = new WebServer( port, new SlidePageGenerator() );
    

    slideStartTime = game_getCurrentTime();
    }



void freeSlides() {
    for( int i=0; i<slides.size(); i++ ) {
        SlideImage s = slides.getElementDirect( i );
        delete s.sourceFile;
        freeSprite( s.sprite );
        }
    slides.deleteAll();

    delete server;
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

    
    if( false && 
        game_getCurrentTime() - slideStartTime > 3 ) {
        slideStartTime = game_getCurrentTime();
        
        nextSlideNumber++;
        
        if( nextSlideNumber >= slides.size() ) {
            nextSlideNumber = 0;
            }
        }
    }

