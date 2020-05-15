execute_process(COMMAND git submodule update --recursive --init -- Cinder
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Configuring and adding Cinder
set(CINDER_BOOST_USE_SYSTEM ON CACHE BOOL "Cinder Option:")
set(CINDER_VERBOSE ON CACHE BOOL "Cinder Verbose output")
add_subdirectory(Cinder)

# Configuring all the cinder blocks
add_subdirectory(blocks)
