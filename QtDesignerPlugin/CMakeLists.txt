cmake_minimum_required(VERSION 3.5)

project(QImguiWidgetQtDesignerPlugin LANGUAGES CXX)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED
${CMAKE_CURRENT_LIST_DIR}/QImguiWidgetQtDesignerPlugin.h
${CMAKE_CURRENT_LIST_DIR}/QImguiWidgetQtDesignerPlugin.cpp
)

target_link_libraries(${PROJECT_NAME}
QImguiWidget
)