cmake_minimum_required(VERSION 3.5)
project(QImWidgets LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Designer Svg uiplugin REQUIRED)

set(widgets_hdr
"widgets/ImArea.h"
"widgets/ImPushButton.h"
)

set(widgets_src
"widgets/ImArea.cpp"
"widgets/ImPushButton.cpp"
)

add_library(${PROJECT_NAME}
   ${widgets_hdr}
   ${widgets_src}
)
target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_LIST_DIR}/widgets/"
)
target_link_libraries(${PROJECT_NAME}
QImguiWidget
Qt5::Widgets
Qt5::Designer
)


set(plugins "ImAreaPlugin" "ImPushButtonPlugin")
foreach(plugin ${plugins})

add_library("${PROJECT_NAME}_${plugin}" SHARED
"plugins/${plugin}.h"
"plugins/${plugin}.cpp"
)
set_target_properties("${PROJECT_NAME}_${plugin}" PROPERTIES 
ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PlatformDir}/lib/plugin"
LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PlatformDir}/lib/plugin"
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PlatformDir}/bin/plugin"
)
target_compile_definitions("${PROJECT_NAME}_${plugin}" PRIVATE
-DQT_PLUGIN
-DQT_NO_DEBUG
-DQT_SHARED
-DQDESIGNER_EXPORT_WIDGETS
)

target_link_libraries("${PROJECT_NAME}_${plugin}" PRIVATE
Qt5::Widgets
Qt5::Designer
Qt5::Svg
${PROJECT_NAME}
)
endforeach()




