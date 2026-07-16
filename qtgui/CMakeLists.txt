cmake_minimum_required(VERSION 3.16)
project(NeuroneBiologiqueGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(NeuroneBiologiqueGUI
    main.cpp
    MainWindow.cpp
    Dataset.cpp
    ReseauNeural.cpp
    NeuroneNode.cpp
    SynapseEdge.cpp
    GraphScene.cpp
    PropertyPanel.cpp
    ComponentPalette.cpp
)

target_link_libraries(NeuroneBiologiqueGUI PRIVATE Qt6::Widgets)
target_include_directories(NeuroneBiologiqueGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
