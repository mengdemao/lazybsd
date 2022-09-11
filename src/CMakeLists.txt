aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/bypass src_bypass)
add_library(bypass STATIC ${src_bypass})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/wrapper src_wrapper)
add_library(wrapper STATIC ${src_wrapper})