# SpontaneousPresentation
A tool for giving on-the-spot non-linear presentations with visuals.

## Introduction

This tool lets you "throw together a talk" by just dropping images into a folder.  The software gives you a "busy box" UI with one button per image thumbnail (meant to be served via web to a touch screen).  Pressing one of the thumbnails changes which image is displayed on the host machine which is connected to a projector.

Thus, during a non-linear talk, you have a "bank" of images that you can choose from during the talk as-needed.  "Oh, I just mentioned Passage, so throw an image of that on the screen."  Then later, "Oh, I just mentioned Nintendo DS as a platform, throw an image of that on the screen."

There obviously still needs to be SOME planning:  a general idea of what images to include ahead of time.  But, I imagine that after you give a few talks like this, that image collection will get refined, and you'll come up with a good base set of images that are pretty useful for the kind of stuff you generally talk about.  Then maybe you'll pepper in a few special-case images before each new talk.

I've seen a bunch of people speak in my lifetime.  Outside of academia and games, NONE of them gave prepared talks.  Examples include Oliver Stone, Gloria Steinem, Chuck D, Terrence McKenna, and Jordan Peterson.  On the other hand, they had no visuals either.  Power Point and "off-the-cuff-talk" really don't mix.  So they just got up there and talked.  But having a visual component helps to hold people's attention and express things that are hard to put into words.  This tool attempts to bridge that gap.

You could also imagine this tool being used for panels, fireside chats, and other non-linear presentation formats.  In the case of a panel, for example, you could ask each panelist for some images ahead of time, arrange them in folders, and then give each panelist a touch screen UI pointing at the same host machine (maybe just through the browsers on their individual phones).  Then, during the panel discussion, they can pull up relevant images as-needed.  Having the control interface served through a web connection offers quite a bit of flexibility.


## Building 

Easiest to build on Linux, but also possible on Mac and Windows (MinGW) with scome fiddling.

Requires SDL 1.2, libpng, and libjpg development libraries for building.


#### Pull like this:
```
git clone https://github.com/jasonrohrer/SpontaneousPresentation.git
git clone https://github.com/jasonrohrer/minorGems.git
```

#### Build like this:
```
./configure
make
```

## Usage

Drop whatever slides (PNG or JPG) into the "slides" folder.  Add "_pixels" to the name of a file to use sharp-pixel scaling (useful for pixel art screen shots).

You can use one level of sub-folders to futher organize slides.


#### Run like this:
```
./RunPresentation
```

#### Connect to the web-based control UI like this:
```
http://192.168.1.4:8080
```
(or whatever the IP address where RunPresentation is running).



Touch (or click) an image in the web-based UI to change which image is showing on the host machine.

The general idea is to connect to the web-based control UI using a touch screen of some kind, though you can also control it with a mouse pointer.


You can control the target web grid size with webHeight and webWidth in the settings folder.