// opencv_tutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core_c.h>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

string videoFName = "./webcam_vid.mp4";
string imgFName = "./webcam_pic.jpg";

void myCallBackFunc(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_RBUTTONDOWN) {
        cout << "Right mouse button was clicked down at " << x << "position and " << y << " position." << endl;
    }
}

// returns frame height and width of video
Size getVideoSize(VideoCapture& v) {
    int webcam_frame_width = int(v.get(CAP_PROP_FRAME_WIDTH));
    int webcam_frame_height = int(v.get(CAP_PROP_FRAME_HEIGHT));

    Size webcam_frame_size(webcam_frame_width, webcam_frame_height);
    return webcam_frame_size;
}

// initiates video capture
void capture(bool precision) {

    VideoCapture webcam(0); // default webcam
    if (webcam.isOpened() == false) {
        cout << "Cannot open the video camera" << endl;
        exit(1);
    }

    Size webcam_frame_size = getVideoSize(webcam);
    int frames_per_second = 60;

    // read images of square and rectangle in grayscale
    Mat imgSquare = imread("square.png", IMREAD_GRAYSCALE);
    Mat imgRectangle = imread("rectangle.jpg", IMREAD_GRAYSCALE);

    // resize thresholded square and rectangle to frame image
    resize(imgRectangle, imgRectangle, webcam_frame_size, 0, 0, INTER_LINEAR);
    resize(imgSquare, imgSquare, webcam_frame_size, 0, 0, INTER_LINEAR);

    // create binarized images of previous images
    Mat thresholdSquare;
    Mat thresholdRectangle;
    threshold(imgSquare, thresholdSquare, 80, 255, THRESH_BINARY_INV);
    threshold(imgRectangle, thresholdRectangle, 80, 255, THRESH_BINARY_INV);

    //imshow("square", thresholdSquare);

    VideoWriter webcamVideoWrite(videoFName, VideoWriter::fourcc('M', 'P', '4', '2'),
        frames_per_second, webcam_frame_size); // assigning filename, media format, fps, and frame size for video

    if (webcamVideoWrite.isOpened() == false) // always check if video file creation is successful
    {
        cout << "Cannot save the video to a file" << endl; // inform user, if error
        cin.get(); //wait for any key press
        exit(-1);
    }

    string webcam_window = "Webcam Feed";
    namedWindow(webcam_window, WINDOW_AUTOSIZE); // create window to see video
    setMouseCallback(webcam_window, myCallBackFunc, NULL); // set callback function on window

    string thresholdWindow = "Threshold Window"; // create window for thresholded image
    namedWindow(thresholdWindow, WINDOW_AUTOSIZE);



    // continue to read frames from capture device
    while (true) {

        // read a frame from the video capture device
        Mat frame;
        bool bSuccess = webcam.read(frame);

        // ERROR IN READING FRAME
        if (bSuccess == false) {
            cout << "An error occured" << endl;
            cin.get();
            break;
        }

        // inverted read frame
        Mat invertedFrame;
        flip(frame, invertedFrame, 1);

        // write frame to video
        webcamVideoWrite.write(invertedFrame);

        // Removing noise from frame by converting to greyscale and applying Gaussian filter
        Mat imgThresholded;
        Mat imgGrayScale; cvtColor(invertedFrame, imgGrayScale, COLOR_BGR2GRAY);
        GaussianBlur(imgGrayScale, imgGrayScale, Size(3, 3), 0);

        // Turning greyscale image into inverse binarized intensity image to easily obtain contours of shapes
        threshold(imgGrayScale, imgThresholded, 80, 255, THRESH_BINARY_INV);


        // vectors to store contours and vertices of shapes
        vector<vector<Point>> storage;
        vector<Point> contours;
        vector<Point> result;

        // finding contours in video frame
        findContours(imgThresholded, storage, RETR_LIST, CHAIN_APPROX_SIMPLE);
        for (int i = 0; i < storage.size(); i++) {
            contours = storage[i];

            // approximating contours and vertices of shapes found in thresholded image
            approxPolyDP(contours, result, arcLength(contours, true) * 0.025, true);

            // calculating areas of contoured shapes
            double area = fabs(contourArea(result));

            // shape detection of squares and rectangles
            if (precision) {

                // use hu moments to determine if approximated polygon is a rectangle or square
                // matchShapes function will determine the distance between stock image of a square/rectangle and contour of polygon
                // the closer the distance is to 0, the better the match
                double dRect = matchShapes(thresholdRectangle, result, 2, 0);
                double dSquare = matchShapes(thresholdSquare, result, 2, 0);

                // draw lines of shape
                if (result.size() == 4 && area >= 1000 && (dRect < 3 || dSquare < 3)) {
                    line(invertedFrame, result[0], result[1], Scalar(0, 255, 0), 7);
                    line(invertedFrame, result[1], result[2], Scalar(0, 255, 0), 7);
                    line(invertedFrame, result[2], result[3], Scalar(0, 255, 0), 7);
                    line(invertedFrame, result[3], result[0], Scalar(0, 255, 0), 7);
                }
            }
            else {
                if (result.size() == 4 && area >= 1000) {
                    line(invertedFrame, result[0], result[1], Scalar(0, 255, 0), 7);
                    line(invertedFrame, result[1], result[2], Scalar(0, 255, 0), 7);
                    line(invertedFrame, result[2], result[3], Scalar(0, 255, 0), 7);
                    line(invertedFrame, result[3], result[0], Scalar(0, 255, 0), 7);
                }
            }

        }

        // show captured frame
        imshow(webcam_window, invertedFrame);

        // show threshold frame
        imshow(thresholdWindow, imgThresholded);

        // wait 10 milliseconds for key input 
        int key = waitKey(10);

        // if key is ESC, then exit
        if (key == 27) {
            cout << "Stopping video..." << endl;
            destroyWindow(webcam_window);
            break;
        }

        // if key is 's', then save the current frame as an image
        else if (key == 115) {
            Mat changedFrame;

            // decrease brightness by 150
            invertedFrame.convertTo(changedFrame, -1, 1, -150);
            // save image
            bool isSaveSuccess = imwrite(imgFName, changedFrame);
            if (isSaveSuccess == false) {
                cout << "Cannot save image..." << endl;
                cin.get();
                destroyWindow(webcam_window);
                break;
            }
        }
    }

    webcamVideoWrite.release();
}

int main()
{
    
    capture(true);

    return 0;

}
