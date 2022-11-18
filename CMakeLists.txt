cmake_minimum_required(VERSION 3.5)
project(QImguiWidget LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Designer Svg uiplugin REQUIRED)


# 查找git
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/.git")
    # 根据需要更新子模块
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/imgui")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


include_directories(PRIVATE ${CMAKE_CURRENT_LIST_DIR}/imgui/)
include_directories(PRIVATE ${CMAKE_CURRENT_LIST_DIR}/)
set(src
    ${CMAKE_CURRENT_LIST_DIR}/QImguiWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui.cpp
)

set(hdr
    ${CMAKE_CURRENT_LIST_DIR}/QImguiWidget.h
    ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui.h
)

add_library(${PROJECT_NAME}
  ${src}
  ${hdr}
)
target_include_directories(${PROJECT_NAME} PUBLIC 
${hdr})
target_link_libraries(${PROJECT_NAME} 
Qt5::Widgets
Qt5::Designer
)


add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/QtCreatorDesignerPlugin/")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/examples/")