cmake_minimum_required(VERSION 3.21)
project(GeminiVisionPracticeAssistant VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
include(CTest)

set(APP_SOURCES
    src/main.cpp
    src/core/models.h src/core/responsevalidator.h src/core/responsevalidator.cpp
    src/core/trigger-server.h src/core/trigger-server.cpp
    src/core/fingerprint.h src/core/fingerprint.cpp
    src/config/appsettings.h src/config/appsettings.cpp
    src/capture/iscreenshotprovider.h src/capture/mock-screenshot-provider.h src/capture/mock-screenshot-provider.cpp
    src/capture/windows-screenshot-provider.h src/capture/windows-screenshot-provider.cpp
    src/vision/igemini-provider.h src/vision/gemini-provider.h src/vision/gemini-provider.cpp
    src/vision/local-agent-provider.h src/vision/local-agent-provider.cpp
    src/core/practice-controller.h src/core/practice-controller.cpp
    src/overlay/overlay-window.h src/overlay/overlay-window.cpp
    src/ui/settings-dialog.h src/ui/settings-dialog.cpp
    src/ui/main-window.h src/ui/main-window.cpp
    src/moc_headers.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${APP_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)
qt_wrap_cpp(GEMINI_MOCS
    src/capture/iscreenshotprovider.h src/capture/mock-screenshot-provider.h src/capture/windows-screenshot-provider.h
    src/vision/igemini-provider.h src/vision/gemini-provider.h src/vision/local-agent-provider.h
    src/core/practice-controller.h src/core/trigger-server.h src/overlay/overlay-window.h
    src/ui/settings-dialog.h src/ui/main-window.h)
target_sources(${PROJECT_NAME} PRIVATE ${GEMINI_MOCS})
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(${PROJECT_NAME} PRIVATE user32 gdi32 windowsapp)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_executable(${PROJECT_NAME}_tests
        tests/test_core.cpp
        src/core/responsevalidator.cpp src/core/fingerprint.cpp
        src/config/appsettings.cpp
    )
    target_include_directories(${PROJECT_NAME}_tests PRIVATE src)
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE Qt6::Core Qt6::Gui Qt6::Network Qt6::Test)
    add_test(NAME core_tests COMMAND ${PROJECT_NAME}_tests)
    set_tests_properties(core_tests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()
