cmake_minimum_required(VERSION 3.12)

# Disallow MSVC since it's Windows-only, and the project targets Linux. Windows
# builds are still possible with clang.
if(MSVC)
  message(SEND_ERROR "MSVC is not supported. Please use clang instead.")
endif()

# Keep the project name in a variable because it's not finalized yet and needs
# to be easily changeable.
set(PROJECT_NAME "cs3307-group-project")
project(${PROJECT_NAME} CXX)

# Add source files here as they are created.
set(SRC_FILES
    "src/CreateAccountPage.cpp"
    "src/File.cpp"
    "src/FileStoragePage.cpp"
    "src/Folder.cpp"
    "src/LoginPage.cpp"
    "src/main.cpp"
    "src/SharingLink.cpp"
    "src/StorageApplication.cpp"
    "src/StorageElement.cpp"
    "src/FileViewPage.cpp"
    "src/FolderStoragePage.cpp"
    "src/User.cpp"
    "src/FileWidget.cpp"
    "src/FolderWidget.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

# Set the compiler to use standard C++20 (no compiler-specific extensions).
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

# https://stackoverflow.com/a/50882216/3410752
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

# Link the Wt library
find_package(Wt REQUIRED Wt HTTP)
target_link_libraries(${PROJECT_NAME} Wt::Wt Wt::HTTP Wt::Dbo Wt::DboSqlite3)
target_compile_definitions(${PROJECT_NAME} PRIVATE HPDF_DLL)
