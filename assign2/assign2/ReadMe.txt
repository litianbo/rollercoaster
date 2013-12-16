========================================================================
    CONSOLE APPLICATION : assign2 Project Overview
========================================================================

AppWizard has created this assign2 application for you.

This file contains a summary of what you will find in each of the files that
make up your assign2 application.


assign2.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

assign2.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

assign2.cpp
    This is the main application source file.
For the spline file, I use goodRide.sp, it is written on the track.txt. And I test the code mainly on goodRide.sp
Buttons for switching views:
press '1 'to quit/enter level 1 view, which only display ground, skies and one single spline. (Notice, I set level 1 to be the highest priority, we need to quit level 1 firstly to see other stuffs.
press 't' to show the line mode rail/the tube; press d to see the real rail track, which has cross section rails, in this view, we also can press w/s to rise or lower the camera position(It provides a full view of this track when we increase this to a certain a level, however, the camera still moves according to the spline.), in this mode, press 'f' to get the upper railing
To pause the camera, press 'a'.

*Email me if I am not clear about the above*

Requirement:
Complete all levels?
When the program starts, (without press any button), level 1 is shown, as level 3, we can see the sky (four sides and up side) and the ground, level2; as level 4, the camera moves along the spline,; as level 5, press '1' to quit level 1 mode, then, press 'd' enter the rail view, we can see the cross-section of the two rails.

Properly render Catmull-Rom splines to represent your track?
When the program starts, without press any button, we can see the spline is displayed in level 1, it is smooth and continuous.

Render a texture-mapped ground and sky?
As we can see, the skies and the ground are shown in the camera.

Render a rail cross-section?
press 1 then, press d, we can see the two rail's cross-section.

Move the camera at a reasonable speed in a continuous path and orientation along the coaster.
As we can see, the camera moves smooth and continous according to the computed binormal and tangent of the current points.

Render the coaster in an interesting manner (good visibility, realism).
I simluate this one according to my real expereience in roller coaster (such as the camera position should be on the center of the two rails instead one of them)

Run at interactive frame rates (>15fps at 640x480)
It runs smoothly and continous, I can't write the code to display it on the window, but I believe it is should be > 15 fps

Be reasonably commented and written in an understandable manner--we will read your code.
I commented every part that is important to render the whole thing

Be submitted along with JPEG frames for the required animation (see below).
Saved in 'screenshots' forlder

Be submitted along with a readme file documenting your program's features and describing the approaches you took to each of the open-ended problems we posed here (rendering the sky and determining coaster normals in particular). This is especially crucial if you have done something spectacular for which you wish to receive extra credit!
Right now, I have not figured out how to solve the seams between the skies yet (I don't see anyone solved that....). for the normals and  binormals, I take the approach listed on the website, take a random vector and the tangent of first point on the spline, then, compute the normal on first point, then compute the binormals and normals in the further points in turn.

Extra Credits:
1. Render the rail bar in texture map:
press 1 then, press d, as you can see, the rail bar is texture mapped. I rendered the rail bar not only as texture map but also model it as a cube, I drawed four sides of it. (front, up, back, bottom, but not left and right, because these two can't be seen either way). I drawed two lines in the front to make it obvious to see, however, viewers may see the lines are not continuous, the truth is that it is absolutely continuous, the reason, I guess, it is due to the line and the squares are too close to each other, they may produce aliasing, I don't want to fix that, because the purpose I draw it is to show people I used 3D texture map (four images on each rail bar) instead of just a single image it doesn't influence the quality of view at all. 
2. Render the railing: press f after press 1 then d, we will see upper side of the rails, it also used technique introduced above. Again,I drawed the line to make the view obvious,  viewers may see the lines are not continuous, but it doesn't effect the view of the overall rail.
2. Line Tube:
press 1 then, the tube is shown, I simulated the tube as people walk in it, I move the camera at the center of the four corners, give viewer a first-perspective view.
3. Camera Movement:
press 1 then, press d; we can move the camera up and down, to see the bottom of the rail or the overall view of the rail. Press 'a' to stop the camera
4. Line Rail:
press 1 then, press t, the line rail is shown, I rendered it as a draft of the real rail.

Test:
Can we use the further points as the camera focus points instead of the multiplication of tangent?
Answer: No, because the camera will just face to some random direction (down, up but not front) if we don't use tangent to compute focus points.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named assign2.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
