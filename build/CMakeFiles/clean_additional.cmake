# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\HardwareSimulationDebugger_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\HardwareSimulationDebugger_autogen.dir\\ParseCache.txt"
  "HardwareSimulationDebugger_autogen"
  )
endif()
