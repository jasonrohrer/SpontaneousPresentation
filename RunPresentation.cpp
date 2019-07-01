#include "minorGems/game/game.h"
#include "minorGems/util/stringUtils.h"




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
    return "font_32_34.tga";
    }


void drawString( const char *inString, char inForceCenter ) {
    }



void initDrawString( int inWidth, int inHeight ) {
    }


void freeDrawString() {
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
    }


void freeFrameDrawer() {
    }


void drawFrame( char inUpdate ) {
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

