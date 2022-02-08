# OpenCV Shape Detection 

This project uses the OpenCV library for Windows to detect and track four-sided polygons captured by web-camera video. The default video source is 
a web-camera. The video capturing device may be changed by providing a ID of the desired video source. The program features two methods
of shape detection: shape matching by simply identifying polygons with 4 sides and shape matching using Hu moments to compare a polygon to a basic
rectangle or square. The Hu moments method is used by default.

Hu momemts are central image momemnts that use the pixel location and intensity of an image to define their value. Effectively, it describes 
the centre of mass of the image. Furthermore, these moments do not change with the scaling, rotation, or translation of the image. This makes 
shapes easier to compare to determine likeness. In this program, the Hu moments of a basic rectangle and square are compared to a 
polygon in question to find the 'distance' between the shapes. The closer the distance to these basic shapes, the better the polyon matches them. 

If a four-sided polygon is detected, a green outline is drawn on the contours of the shape in the video. The program also displays a window where
binarized version (black or white pixels only) of the current video frame are displayed. An example of the program output is shown below:

![shape_detection](https://user-images.githubusercontent.com/43174428/152913239-8a43ddae-21be-4918-a4ef-a09ed0c1e898.png)

Pressing the ESC key during execution will end the program and pressing the 's' key will save current frame of the video as an image to your current
directory.



