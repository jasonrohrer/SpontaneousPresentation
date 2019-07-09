#include "slides.h"
#include "minorGems/network/web/server/WebServer.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/util/SettingsManager.h"
#include "minorGems/game/gameGraphics.h"
#include "minorGems/game/game.h"

#include "minorGems/graphics/converters/PNGImageConverter.h"
#include "minorGems/graphics/converters/JPEGImageConverter.h"


static int startingSlideID = 0;


static int nextSlideID = -1;

static int curFolderID = -1;


static double slideStartTime;



static File slidesDir( NULL, "slides" );


static WebServer *server;


typedef struct SlideImage {
        int id;
        // -1 if in top folder
        int folderID;
        
        File *sourceFile;
        SpriteHandle sprite;
        int w, h;
        char pixelArt;
        
        // or PNG
        char isJPEG;
    } SlideImage;

    
SimpleVector<SlideImage> slides;

typedef struct SubFolder {
        int id;
        char *name;
    } SubFolder;

SimpleVector<SubFolder> folders;


typedef struct FolderOrImage {
        SubFolder *f;
        SlideImage *im;
    } FolderOrImage;



    


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
            if( strstr( inGetRequestPath, "subFolder_pixels.png" ) != NULL ) {
                File iconFile( NULL, "subFolder_pixels.png" );
                
                if( iconFile.exists() ) {
                    int fileLen;
                
                    unsigned char *fileCont = 
                        iconFile.readFileContents( &fileLen );
                    inOutputStream->write( fileCont, fileLen );
                    delete [] fileCont;
                    }
                }
            else if( strstr( inGetRequestPath, 
                             "parentFolder_pixels.png" ) != NULL ) {
                File iconFile( NULL, "parentFolder_pixels.png" );
                
                if( iconFile.exists() ) {
                    int fileLen;
                
                    unsigned char *fileCont = 
                        iconFile.readFileContents( &fileLen );
                    inOutputStream->write( fileCont, fileLen );
                    delete [] fileCont;
                    }
                }
            else if( strstr( inGetRequestPath, ".jpg" ) != NULL ||
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

                if( strstr( inGetRequestPath, "/folder_" ) != NULL ) {
                    // switching sub-folder
                    sscanf( inGetRequestPath, "/folder_%d", &curFolderID );
                    }
                else {    
                    int newID = -1;
                    
                    sscanf( inGetRequestPath, "/%d", &newID );
                    
                    if( newID != -1 ) {
                        nextSlideID = newID;
                        }
                    }
                
                printf( "curFolderID=%d\n", curFolderID );

                SimpleVector<FolderOrImage> drawList;

                SubFolder parentFolder = { -1, (char*)"up" };
                if( curFolderID != -1 ) {
                    // down in a sub folder
                    // put a link up and out
                
                    // BUT only if there are some slides in parent folder
                    // (if not, no point in navigating out)
                    int parentFolderSlideCount = 0;
                    
                    for( int i=0; i<slides.size(); i++ ) {
                        SlideImage *s = slides.getElement( i );
                        if( s->folderID == -1 ) {
                            parentFolderSlideCount++;
                            }
                        }
                    
                    if( parentFolderSlideCount > 0 ) {
                        FolderOrImage p;
                        p.f = &parentFolder;
                        p.im = NULL;
                        drawList.push_back( p );
                        }
                    }
                
                    
                for( int i=0; i<folders.size(); i++ ) {
                    FolderOrImage p;
                    p.f = folders.getElement( i );
                    p.im = NULL;
                    drawList.push_back( p );
                    }
                
                for( int i=0; i<slides.size(); i++ ) {
                    SlideImage *s = slides.getElement( i );
                    if( s->folderID == curFolderID ) {
                        FolderOrImage p;
                        p.f = NULL;
                        p.im = s;
                        drawList.push_back( p );
                        }
                    }
                
                int numSlides = drawList.size();
                

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

                inOutputStream->writeString( 
                    "<html><head><script>"
#include "clockScript.js"
                    "</script></head>"
                    "<body bgcolor=black "
                    "onload=\"showTime()\""
                    "text=white link=white alink=yellow vlink=white>" );
                inOutputStream->writeString( "<center>" );

                if( SettingsManager::getIntSetting( "showClock", 1 ) ) {
                    
                    inOutputStream->writeString( 
                        "<div style=\"position:absolute; left: 50%;"
                        "     pointer-events:none;\">"
                        "<div id=\"MyClockDisplay\" "
                        "style=\"position:relative; left: -50%;"
                        "pointer-events:none;"
                        "opacity: 0.75; "
                        "font-size: 60px; "
                        "color: #FFAAAA; "
                        "text-shadow: "
                        "3px 3px 0 #000,"
                        "3px 0px 0 #000,"
                        "-3px 0px 0 #000,"
                        "0px 3px 0 #000,"
                        "0px -3px 0 #000,"
                        "-3px 3px 0 #000,"
                        "3px -3px 0 #000,"
                        "-3px -3px 0 #000;\"></div></div>" );
                    }
                
                char *tableString = autoSprintf( "<table border = 1 "
                                                 "cellspacing=0 cellpadding=%d "
                                                 "width=%d height=%d>\n",
                                                 cellPadding, webW, webH );
                
                inOutputStream->writeString( tableString );
                delete [] tableString;

                inOutputStream->writeString( "<tr>\n" );

                int rowCount = 0;
                
                for( int i=0; i<drawList.size(); i++ ) {
                    FolderOrImage d = drawList.getElementDirect( i );
                    
                    SlideImage s;
                    
                    if( d.im != NULL ) {
                        s = *( d.im );
                        }
                    else {
                        // a sub-folder
                        // make a fake image

                        s.id = -1;
                        s.folderID = d.f->id;
                        s.w = 32;
                        s.h = 32;
                        s.pixelArt = true;
                        s.isJPEG = false;
                        }    


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
                    

                    if( s.id == nextSlideID ||
                        ( s.id == -1 &&
                          s.folderID == curFolderID ) ) {
                        delete [] borderStyle;
                        
                        borderStyle = autoSprintf( "border:%dpx dashed yellow;",
                                                   imBorder );
                        }
                        
                    

                    char *line;
                    
                    if( s.id != -1 ) {
                        // a real slide image
                        line = autoSprintf( "<td align=center valign=middle>"
                                            "<a href=%d>"
                                            "<img src=%d%s width=%d height=%d"
                                            " style='%s %s' "
                                            ">"
                                            "</a></td>\n", 
                                            s.id, s.id, ext,
                                            thisW, thisH, style, borderStyle );
                        }
                    else if( s.folderID == -1 ) {
                        // a parent folder link
                        line = autoSprintf( "<td align=center valign=middle>"
                                            "<a href=folder_%d>"
                                            "<img src=parentFolder_pixels.png "
                                            "width=%d height=%d"
                                            " style='%s %s' "
                                            "><br>%s"
                                            "</a></td>\n", 
                                            s.folderID,
                                            thisW, thisH, style, borderStyle,
                                            d.f->name );
                        }
                    else {
                        // a folder link
                        line = autoSprintf( "<td align=center valign=middle>"
                                            "<a href=folder_%d>"
                                            "<img src=subFolder_pixels.png "
                                            "width=%d height=%d"
                                            " style='%s %s' "
                                            "><br>%s"
                                            "</a></td>\n", 
                                            s.folderID,
                                            thisW, thisH, style, borderStyle,
                                            d.f->name );
                        }
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
    



void processFile( File *f, int inFileID, int inFolderID ) {
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
        s.id = inFileID;
        s.folderID = inFolderID;
        
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




void initSlides() {
    startingSlideID = SettingsManager::getIntSetting( "startingSlideID", 0 );
    
    int numFiles;
    File **childFiles = slidesDir.getChildFiles( &numFiles );

    int nextFileID = startingSlideID;
    int nextFolderID = 0;

    for( int i=0; i<numFiles; i++ ) {
        
        File *f = childFiles[i];

        if( f->isDirectory() ) {
            
            SubFolder sub = { nextFolderID, f->getFileName() };
            nextFolderID++;

            folders.push_back( sub );
            
            int numSubFiles;
            File **subChildFiles = f->getChildFiles( &numSubFiles );

            for( int s=0; s<numSubFiles; s++ ) {
                File *subF = subChildFiles[s];
                
                if( ! subF->isDirectory() ) {
                    processFile( subF, nextFileID, sub.id );
                    nextFileID++;
                    }
                else {
                    delete subF;
                    }
                }
            delete [] subChildFiles;
            delete f;
            }
        else {
            processFile( f, nextFileID, -1 );
            nextFileID++;
            }
        }
    
    delete [] childFiles;


    // save for next time, so we always use fresh IDs to avoid
    // browser caching between runs if images are changing
    SettingsManager::setSetting( "startingSlideID", nextFileID );

    
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

    for( int i=0; i<folders.size(); i++ ) {
        SubFolder f = folders.getElementDirect( i );
        delete [] f.name;
        }
    folders.deleteAll();
    
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

