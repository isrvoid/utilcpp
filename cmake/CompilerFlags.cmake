set(TEMP_GCC_WARNINGS "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wconversion -Wstrict-prototypes")
set(TEMP_GPP_WARNINGS "-Wall -Wextra -pedantic -Wpointer-arith -Wcast-align -Wwrite-strings -Wredundant-decls -Winline -Wno-long-long -Wno-conversion -Wzero-as-null-pointer-constant")
# TODO remove "no" from -Wno-conversion

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TEMP_GCC_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TEMP_GPP_WARNINGS}")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${TEMP_GCC_WARNINGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${TEMP_GPP_WARNINGS}")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${TEMP_GCC_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${TEMP_GPP_WARNINGS}")

# build type macro definitions for the linker
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
	# -DNDEBUG is already included in CMAKE_C_FLAGS_RELEASE
	add_definitions(-DRELEASE)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	add_definitions(-DDEBUG)
endif()
