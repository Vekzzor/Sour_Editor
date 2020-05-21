// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <math.h>
#include <set>
#include <sstream>
#include <stack>
#include <string>

#include <execution>

#include "Bowyer-Watson/Delunay.h"
#include "ECS/ComponentManager.h"
#include "ECS/Coordinator.h"
#include "ECS/ECS.h"
#include "ECS/EntityManager.h"
#include "ECS/SystemManager.h"
#include "EditorSystems.h"
#include "EditorTools.h"
#include "EventHandler.h"
#include "InputMapping.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "imgui_SFML/imgui-SFML.h"
#include "imgui_SFML/imgui.h"
#include <Box2D/box2d.h>
#include <SFML/Graphics.hpp>
#include "Inspector.h"
#include "Components.h"
#include "Voronoi_Tessellation.h"
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )

static const int kPixelToMeterRatio = 10000;
//Source hammer unit == 1.904 cm, 1 m == 52.52 hammer units
static const float kUnitScale = 52.52;
#define Sour_PI 3.14159265359f

template <class Container, class F>
auto erase_where(Container& _c, F&& _f, int _offset = 0)
{
	return _c.erase(std::remove_if(_c.begin() + _offset, _c.end(), std::forward<F>(_f)), _c.end());
}

#ifdef _DEBUG
#	define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#	define DEBUG 1
#else
#	define DEBUG 0
#	define DBG_NEW new
#endif
// TODO: add headers that you want to pre-compile here

#endif //PCH_H


/*Google naming conventions
Optimize for readability using names that would be clear even to people on a different team.

File names: 
my_useful_class.cc <- prefer if no consisten local pattern
my-useful-class.cc
myusefulclass.cc

Type Names: classes, structs, type aliases, enums, and type template parameters
Capital each word, no underscores
class UrlTable { ...
class UrlTableTester { ...
struct UrlTableProperties { ...

Variable names:all lowercase with underscores between words
	Common Variable names: 
		std::string table_name;

	Class Data Members: 
		add trailing underscore
			std::string table_name_; 

	Struct Data Members:  
		same as common variable names

	Constant Names: 
		start with lowercase k 
			const int kDaysInAWeek = 7;
		underscores can be used when capitalization cannot be used for seperation
			const int kAndroid8_0_0 = 24;  // Android 8.0.0

	Function Names: 
		Regular functions have mixed case; accessors and mutators may be named like variables.
			AddTableEntry()
			DeleteUrl()
			OpenFileOrDie()
	Enum Names:
		Enumerators (for both scoped and unscoped enums) should be named either like constants or like macros: either kEnumName or ENUM_NAME.
			enum UrlTableErrors {
				kOk = 0,
				kErrorOutOfMemory,
				kErrorMalformedInput,
			};
			enum AlternateUrlTableErrors {
			  OK = 0,
			  OUT_OF_MEMORY = 1,
			  MALFORMED_INPUT = 2,
			};
*/

/*Additional Conventions
	Function parameters starts with a _
		_variable
*/