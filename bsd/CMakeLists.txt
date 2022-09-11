cmake_minimum_required(VERSION 3.24)

project(bsd C CXX ASM)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/sys/net src_net)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet src_netinet)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/sys/netinet6 src_netinet6)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/sys/netgraph src_netgraph)

add_library(bsd STATIC SHARED ${src_net} ${src_netinet} ${src_netinet6} ${src_netgraph})