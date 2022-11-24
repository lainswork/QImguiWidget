if(NOT DEFINED QImguiWidget_Name)
    set(QImguiWidget_Name "QImguiWidget")
endif()

if(NOT DEFINED PlatformDir)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
        set(PlatformDir "x86")
    else()
        set(PlatformDir "x64")
    endif()
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(imgui REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(src
${CMAKE_CURRENT_LIST_DIR}/src/QImguiWidget.cpp
)
set(hdr
${CMAKE_CURRENT_LIST_DIR}/hdr/QImguiWidget/QImguiWidget.h
)
add_library(${QImguiWidget_Name}
  ${src}
  ${hdr}
)
target_include_directories(${QImguiWidget_Name} PUBLIC
${CMAKE_CURRENT_LIST_DIR}/hdr/
)
target_link_libraries(${QImguiWidget_Name} Qt5::Widgets imgui::imgui)


