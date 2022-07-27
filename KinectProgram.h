#pragma once

#include <iostream>
#include <cmath>
#include "stdafx.h"

#define PI 3.14159

class Coordinate {
public:
    int x;
    int y;
    int z;
    USHORT depth;
    RGBQUAD* pixel;
};

class KinectProgram
{
public:
    /*
        Method where a new depth is retrieved, as well as its index
        and corresponding RGB pixel.
    */
    const void UpdateDepth(int index, USHORT depth, RGBQUAD* pRGBX);

    /*
        Using all the data from the current frame, analyze it.
    */
    const void AnalyzeFrame();

    // Destroy the current set of coordinates in positions
    const void DestroyFrame();

private:
    static const USHORT DEPTH_FRAME_WIDTH = 512;
    static const USHORT DEPTH_FRAME_HEIGHT = 424;
    static const USHORT CAMERA_ANGLE = 0; 
    static const USHORT SIZE_OF_TOP_VIEW_X = 2250; // in mm
    static const USHORT SIZE_OF_TOP_VIEW_Y = 3900; // in mm

    static const UCHAR MARGIN_OF_ERROR = 50;

    USHORT camera_height = 550;

    USHORT depths[DEPTH_FRAME_HEIGHT][DEPTH_FRAME_WIDTH];
    RGBQUAD* pixels[DEPTH_FRAME_HEIGHT][DEPTH_FRAME_WIDTH];

    /*
        A list of positions: x, y, z
        x - x position from a top-down view
        y - y position from a top-down view
        z - z position above/below the top-down view
        Also stores the addresses of the pixels each position
        points to in a standard camera view window.
    */
    Coordinate positions[DEPTH_FRAME_HEIGHT* DEPTH_FRAME_WIDTH];

    float floor_average;
    int frame_num = 0;


    // Code to run on first frame
    const void Init();

    // Method to perform floor detection via a simple algorithm
    const void BasicFloorDetection(Coordinate &coordinate);

    // Adaptive floor detection
    const void CalibrateFloorDetection();

    const void FloorDetection();

    // Convert degrees to radians
    static const float ToRadians(float angle)
    {
        return angle * PI / 180;
    }

    /*
    Convert an x - y coordinate to a single index for
    the pixel buffer and depth buffer.
    */
    static const int ConvertCoordinate(int x, int y)
    {
        return y * DEPTH_FRAME_WIDTH + x;
    }

    // Uses integer division to calculate the y index value of an item in a list
    static const int GetYValue1D(int index_in_1D_list)
    {
        return index_in_1D_list / DEPTH_FRAME_WIDTH;
    }

    // Uses integer modulus to calculate the x index value of an item in a list
    static const int GetXValue1D(int index_in_1D_list)
    {
        return index_in_1D_list % DEPTH_FRAME_WIDTH;
    }

    // Get the x angle from a pixel position
    static const float PixelXAngle(int x_position)
    {
        return (float)std::atan((7.0 * (x_position - (DEPTH_FRAME_WIDTH / 2.0))) / (DEPTH_FRAME_WIDTH / 2.0 * 10));
    }

    // Get the y angle from a pixel position
    static const float PixelYAngle(int y_position)
    {
        return (float)std::atan((sqrt(3) * (((DEPTH_FRAME_HEIGHT / 2.0) - y_position))) / (DEPTH_FRAME_HEIGHT / 2.0 * 3));
    }

    // Convert a x distance relative to the camera to a pixel location
    static const int TopViewPlotDepthX(float x_distance);

    // Convert a y distance relative to the camera to a pixel location
    static const int TopViewPlotDepthY(float y_distance);
};
