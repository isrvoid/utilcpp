add_app_sources(
	blockstore.cpp
	semaphore.cpp
	lengthencoding.cpp
	)

add_subdirectory(sha1)
add_subdirectory(crc)

add_subdirectory(test)
