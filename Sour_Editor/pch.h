// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

#include <cmath>
#include <limits>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <array>
#include <set>
#include <algorithm>
#include <math.h>

#include <execution>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "EditorTools.h"
#include "Bowyer-Watson Accel/Delunay.h"
#include "InputMapping.h"
#include "EventHandler.h"

typedef std::array<float, 2> float2;
static const int pixelToMeterRatio = 10000;

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#define DEBUG 1
#else
#define DEBUG 0
#define DBG_NEW new
#endif
// TODO: add headers that you want to pre-compile here

#endif //PCH_H

