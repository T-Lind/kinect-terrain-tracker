#include "KinectProgram.h"

const void KinectProgram::Init()
{
	CalibrateFloorDetection();

	++frame_num;
}

const void KinectProgram::UpdateDepth(int index, USHORT depth, RGBQUAD* pRGBX)
{
	using std::cos;
	using std::sin;

	float x_angle = PixelXAngle(GetXValue1D(index));
	float y_angle = PixelYAngle(GetYValue1D(index)) + ToRadians(CAMERA_ANGLE);

	depths[GetYValue1D(index)][GetXValue1D(index)] = depth;
	pixels[GetYValue1D(index)][GetXValue1D(index)] = pRGBX;

	positions[index].x = -sin(x_angle) * cos(y_angle) * depth;
	positions[index].y = cos(x_angle) * cos(y_angle) * depth;
	positions[index].z = depth * sin(y_angle);
	positions[index].pixel = pRGBX;
}

const void KinectProgram::AnalyzeFrame()
{
	if (frame_num  == 0)
	{
		Init();
		return;
	}

	// Mark the center of the frame
	pixels[DEPTH_FRAME_HEIGHT/2][DEPTH_FRAME_WIDTH/2]->rgbBlue = 255;

	FloorDetection();

	// Draw a box where the camera is, 5x5
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < 5; x++) {
			pixels[DEPTH_FRAME_HEIGHT - 1 - y][DEPTH_FRAME_WIDTH / 2 - 2 + x]->rgbRed = 255;
		}
	}

	++frame_num;
}

const void KinectProgram::BasicFloorDetection(Coordinate& coordinate)
{

	if (coordinate.z < -camera_height)
	{
		USHORT x_index = TopViewPlotDepthX(coordinate.x);
		USHORT y_index = TopViewPlotDepthY(coordinate.y);

		if (x_index > 0 && x_index < DEPTH_FRAME_WIDTH && y_index > 0 && y_index < DEPTH_FRAME_HEIGHT) {
			pixels[y_index][x_index]->rgbGreen = (200000 / (coordinate.y) - 15);
		}
	}
}

const void KinectProgram::CalibrateFloorDetection()
{
	int sum = 0;
	int cnt = 0;
	for (int i = 0; i < sizeof(positions) / sizeof(Coordinate);i++)
	{
		const Coordinate coordinate = positions[i];
		if (coordinate.z < -camera_height)
		{
			sum += abs(coordinate.z);
			++cnt;

			// Draw floor
			USHORT x_index = TopViewPlotDepthX(coordinate.x);
			USHORT y_index = TopViewPlotDepthY(coordinate.y);

			if (x_index > 0 && x_index < DEPTH_FRAME_WIDTH && y_index > 0 && y_index < DEPTH_FRAME_HEIGHT)
			{
				pixels[y_index][x_index]->rgbGreen = (200000 / (coordinate.y) - 15);
			}

		}
	}

	floor_average = abs((float)sum / cnt);
}

const void KinectProgram::FloorDetection()
{
	if (frame_num % 100 == 0)
	{
		CalibrateFloorDetection();
	}

	for (int i = 0; i < sizeof(positions) / sizeof(Coordinate); i++)
	{
		const Coordinate coordinate = positions[i];
		if (floor_average - MARGIN_OF_ERROR < abs(coordinate.z) && abs(coordinate.z) < floor_average + MARGIN_OF_ERROR)
		{
			// Draw floor
			USHORT x_index = TopViewPlotDepthX(coordinate.x);
			USHORT y_index = TopViewPlotDepthY(coordinate.y);

			if (x_index > 0 && x_index < DEPTH_FRAME_WIDTH && y_index > 0 && y_index < DEPTH_FRAME_HEIGHT)
			{
				pixels[y_index][x_index]->rgbGreen = (200000 / (coordinate.y) - 15);
			}

		}
	}

}

const int KinectProgram::TopViewPlotDepthX(float x_distance)
{
	float distance_percent = x_distance / (SIZE_OF_TOP_VIEW_X);

	if (distance_percent > 1)
	{
		distance_percent = 1;
	}

	return (int)(distance_percent * (DEPTH_FRAME_WIDTH / 2) + (DEPTH_FRAME_WIDTH / 2));
}

const int KinectProgram::TopViewPlotDepthY(float y_distance)
{
	float distance_percent = y_distance / (SIZE_OF_TOP_VIEW_Y);

	if (distance_percent > 1)
	{
		distance_percent = 1;
	}

	return (int)(DEPTH_FRAME_HEIGHT - distance_percent * DEPTH_FRAME_HEIGHT);
}
