cmake_minimum_required(VERSION 3.5)

project(QImguiWidget LANGUAGES CXX)
set(QImguiWidget_Name ${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include("${CMAKE_CURRENT_LIST_DIR}/QImguiWidgetConfig.cmake")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/example")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/QtDesignerPlugin")