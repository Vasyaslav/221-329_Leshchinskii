# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\crypt1_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\crypt1_autogen.dir\\ParseCache.txt"
  "crypt1_autogen"
  )
endif()
