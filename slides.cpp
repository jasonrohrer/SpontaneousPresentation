#include "slides.h"
#include "minorGems/network/web/server/WebServer.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/game.h"

#include "minorGems/graphics/converters/PNGImageConverter.h"
#include "minorGems/graphics/converters/JPEGImageConverter.h"


static int nextSlideID = 0;

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
                    nextSlideID = newID;
                    }

                int numSlides = slides.size();
                
                int w = ceil( sqrt( numSlides ) );
                
                int h = ceil( numSlides / (double) w );
                
                int cellPadding = 0;
                
                int imBorder = 
                    SettingsManager::getIntSetting( "imageBorderWidth", 10 );
                

                int webW = SettingsManager::getIntSetting( "webWidth", 1280 );
                int webH = SettingsManager::getIntSetting( "webHeight", 720 );
                

                int imageMaxW = webW / w;
                int imageMaxH = webH / h;
                
                imageMaxW -= 2 * ( cellPadding + imBorder );
                imageMaxH -= 2 * ( cellPadding + imBorder );

                inOutputStream->writeString( "<html><body bgcolor=black>" );
                inOutputStream->writeString( "<center>" );

                char *tableString = autoSprintf( "<table border = 1 "
                                                 "cellspacing=0 cellpadding=%d "
                                                 "width=%d height=%d>\n",
                                                 cellPadding, webW, webH );
                
                inOutputStream->writeString( tableString );
                delete [] tableString;

                inOutputStream->writeString( "<tr>\n" );

                int rowCount = 0;
                
                for( int i=0; i<slides.size(); i++ ) {
                    SlideImage s = slides.getElementDirect( i );
                    
                    int thisW = s.w;
                    int thisH = s.h;
                    

                    if( thisW > imageMaxW ) {
                        float scale = imageMaxW / (float)thisW;
                        thisW = imageMaxW;
                        
                        thisH *= scale;
                        }
                    if( thisH > imageMaxH ) {
                        float scale = imageMaxH / (float)thisH;
                        thisH = imageMaxH;
                        
                        thisW *= scale;
                        }
                    
                    if( thisH < imageMaxH && thisW < imageMaxW ) {
                        if( thisH / (float)imageMaxH > 
                            thisW / (float)imageMaxW ) {
                            
                            float scale = imageMaxH / (float)thisH;
                            thisH = imageMaxH;
                            
                            thisW *= scale;
                            }
                        else {
                            float scale = imageMaxW / (float)thisW;
                            thisW = imageMaxW;
                            
                            thisH *= scale;
                            }
                        }
                    
                    

                    const char *ext = ".png";
                    
                    if( s.isJPEG ) {
                        ext = ".jpg";
                        }
                    
                    const char *style = "";
                    
                    if( s.pixelArt ) {
                        style = 
                            "image-rendering: pixelated; "
                            "image-rendering: -moz-crisp-edges;"
                            "image-rendering: crisp-edges; ";
                        }
                    
                    
                    char *borderStyle = autoSprintf( "border:%dpx solid black;",
                                                     imBorder );
                    

                    if( s.id == nextSlideID ) {
                        delete [] borderStyle;
                        
                        borderStyle = autoSprintf( "border:%dpx dashed yellow;",
                                                   imBorder );
                        }
                    
                    

                    char *line =
                        autoSprintf( "<td align=center valign=middle>"
                                     "<a href=%d>"
                                     "<img src=%d%s width=%d height=%d"
                                     " style='%s %s' "
                                     ">"
                                     "</a></td>\n", //wFraction, hFraction, 
                                     s.id, s.id, ext,
                                     thisW, thisH, style, borderStyle );
                    inOutputStream->writeString( line );
                    delete [] line;
                    delete [] borderStyle;
                    
                    rowCount ++;
                    
                    if( rowCount >= w ) {
                        inOutputStream->writeString( "</tr>\n\n<tr>" );
                        rowCount = 0;
                        }
                    }
                inOutputStream->writeString( "</tr>\n" );
                inOutputStream->writeString( "</table>" );

                inOutputStream->writeString( "</center>" );
                inOutputStream->writeString( "</body>" );
                inOutputStream->writeString( "</html>" );
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



        virtual int getCacheMaxAge( char *inGetRequestPath ) {
            if( strstr( inGetRequestPath, ".jpg" ) != NULL ) {
                return 3600 * 2;
                }
            else if( strstr( inGetRequestPath, ".png" ) != NULL ) {
                return 3600 * 2;
                }
            else {
                return 0;
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
    
    SlideImage *s = getSlideByID( nextSlideID );
    
    if( s != NULL ) {
        toggleLinearMagFilter( ! s->pixelArt );

        setDrawColor( 1, 1, 1, 1 );
        
        doublePair pos = { 0, 0 };

        double zoom = 1.0;

        if( s->w / viewWidth > s->h / viewHeight ) {
            zoom = viewWidth / s->w;
            }
        else {
            zoom = viewHeight / s->h;
            }
        
        
        drawSprite( s->sprite, pos, zoom );
        }

    }

