cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("pi-weatherapp")

# 1) Find libcurl (must be after project())
find_package(CURL REQUIRED)

# Add source to this project's executable.
add_executable (pi-weatherapp "pi-weatherapp.cpp" "pi-weatherapp.h")

# 2) Link libcurl to your executable target (must be after add_executable)
target_link_libraries(pi-weatherapp PRIVATE CURL::libcurl)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET pi-weatherapp PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
