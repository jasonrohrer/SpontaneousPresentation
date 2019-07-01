#include "minorGems/game/game.h"
#include "minorGems/game/Font.h"
#include "minorGems/util/stringUtils.h"

#include "slides.h"



Font *mainFont;

char initDone = false;


// position of view in world
doublePair lastScreenViewCenter = {0, 0 };



// world width of one view
double viewWidth = 1280;
double viewHeight = 720;


// this is the desired visible width
// if our screen is wider than this (wider than 16:9 aspect ratio)
// then we will put letterbox bars on the sides
// Usually, if screen is not 16:9, it will be taller, not wider,
// and we will put letterbox bars on the top and bottom 
double visibleViewWidth = viewWidth;



// fraction of viewWidth visible vertically (aspect ratio)
double viewHeightFraction;





const char *getWindowTitle() {
    return "Presentation";
    }


char doesOverrideGameImageSize() {
    return false;
    }


void getGameImageSize( int *outWidth, int *outHeight ) {
    }


char shouldNativeScreenResolutionBeUsed() {
    return true;
    }


char isNonIntegerScalingAllowed() {
    return true;
    }





const char *getAppName() {
    return "RunPresentation";
    }



int getAppVersion() {
    return 1;
    }



const char *getLinuxAppName() {
    return "RunPresentationApp";
    }



char *getCustomRecordedGameData() {
    return stringDuplicate( "" );
    }


char showMouseDuringPlayback() {
    return true;
    }


char *getHashSalt() {
    return stringDuplicate( "test" );
    }


const char *getFontTGAFileName() {
    return "font_32_64.tga";
    }




void drawString( const char *inString, char inForceCenter ) {

    setDrawColor( 1, 1, 1, 0.75 );
        
    doublePair messagePos = lastScreenViewCenter;

    TextAlignment align = alignCenter;
    
    if( initDone && !inForceCenter ) {
        // transparent message
        setDrawColor( 1, 1, 1, 0.75 );

        // stick messages in corner
        messagePos.x -= viewWidth / 2;
        
        messagePos.x +=  20;
    

    
        messagePos.y += (viewWidth * viewHeightFraction) /  2;
    
        messagePos.y -= 32;

        align = alignLeft;
        }
    else {
        // fully opaque message
        setDrawColor( 1, 1, 1, 1 );

        // leave centered
        }
    

    int numLines;
    
    char **lines = split( inString, "\n", &numLines );
    
    for( int i=0; i<numLines; i++ ) {
        

        mainFont->drawString( lines[i], messagePos, align );
        messagePos.y -= 32;
        
        delete [] lines[i];
        }
    delete [] lines;
    }



void initDrawString( int inWidth, int inHeight ) {
    toggleLinearMagFilter( true );
    toggleMipMapMinFilter( false );
    toggleTransparentCropping( true );
    
    mainFont = new Font( getFontTGAFileName(), 6, 16, false, 16 );
    
    setViewCenterPosition( lastScreenViewCenter.x, lastScreenViewCenter.y );

    viewHeightFraction = inHeight / (double)inWidth;
    
    if( viewHeightFraction < 9.0 / 16.0 ) {
        // weird, wider than 16:9 aspect ratio
        
        viewWidth = viewHeight / viewHeightFraction;
        }
    

    setViewSize( viewWidth );
    setLetterbox( visibleViewWidth, viewHeight );
    }


void freeDrawString() {
    delete mainFont;
    }



char isDemoMode() {
    return false;
    }


const char *getDemoCodeSharedSecret() {
    return "test";
    }


const char *getDemoCodeServerURL() {
    return "";
    }



void initFrameDrawer( int inWidth, int inHeight, int inTargetFrameRate,
                      const char *inCustomRecordedGameData,
                      char inPlayingBack ) {
    
    setViewCenterPosition( lastScreenViewCenter.x, lastScreenViewCenter.y );

    viewHeightFraction = inHeight / (double)inWidth;
    
    if( viewHeightFraction < 9.0 / 16.0 ) {
        // weird, wider than 16:9 aspect ratio
        
        viewWidth = viewHeight / viewHeightFraction;
        }
    

    setViewSize( viewWidth );
    setLetterbox( visibleViewWidth, viewHeight );

    initSlides();
    initDone = true;
    }



void freeFrameDrawer() {
    freeSlides();
    }



void drawFrame( char inUpdate ) {
    wakeUpPauseFrameRate();
    drawCurrentSlide( viewWidth, viewHeight );
    }



void pointerMove( float inX, float inY ) {
    }


void pointerDown( float inX, float inY ) {
    }



void pointerDrag( float inX, float inY ) {
    }


void pointerUp( float inX, float inY ) {
    }





void keyDown( unsigned char inASCII ) {
    }


void keyUp( unsigned char inASCII ) {
    }




void specialKeyDown( int inKeyCode ) {
    }


void specialKeyUp( int inKeyCode ) {
    }




// should sound be initialized?
char getUsesSound() {
    return false;
    }



// called before the first call to getSoundSamples
// allows static allocation of buffers used in callback
void hintBufferSize( int inLengthToFillInBytes ) {
    }

void freeHintedBuffers() {
    }

    


// gets the next buffer-full of sound samples from the game engine
// inBuffer should be filled with stereo Sint16 samples, little endian,
//    left-right left-right ....
// NOTE:  may not be called by the same thread that calls drawFrame,
//        depending on platform implementation
void getSoundSamples( Uint8 *inBuffer, int inLengthToFillInBytes ) {
    return;
    }

