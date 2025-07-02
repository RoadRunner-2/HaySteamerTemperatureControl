# CMakeList.txt : CMake project for Sandbox, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Sandbox")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

# Add source to this project's executable.
add_executable (Sandbox 
	"Sandbox.cpp" 
	"Sandbox.h"
	"../TempProbe.h"
	"../TaskScheduler.h"
	"../StateMachine.h"
	"../Services.h"
	"../Parameter.h"
	"../Keypad.h"
	"../Display.h")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Sandbox PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}../)

# Add unit tests.
add_executable(UnitTests SandboxTests/Test_ParameterEditor.cpp)
target_link_libraries(UnitTests Test_ParameterEditor)
set_target_properties(UnitTests PROPERTIES CXX_STANDARD 14)

include(GoogleTest)
gtest_discover_tests(UnitTests)

# TODO: Add tests and install targets if needed.
