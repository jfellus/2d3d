cmake_minimum_required (VERSION 2.6)
project (culture3dcloud)
include_directories ("${PROJECT_SOURCE_DIR}/src")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

set(CMAKE_BUILD_TYPE Release)
add_definitions(-DRETIN_DISABLE_JAVA)
add_definitions(-DTHREAD_POOL_USE_POSIX_THREADS)

IF(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}  -g -Wall -std=c99   -pedantic-errors -Wno-long-long -mfpmath=sse -msse")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-sign-compare -std=c++0x -Wno-long-long -mfpmath=sse -msse")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
#SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -march=native")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

set(core_dir src/retin/toolbox/core)
file(GLOB core_headers ${core_dir}/core.h ${core_dir}/arrays.h ${core_dir}/auto_array_ptr.h ${core_dir}/string.h ${core_dir}/fvec_reader.h  ${core_dir}/siftgeo_reader.h ${core_dir}/fvec_writer.h ${core_dir}/work_task.h ${core_dir}/thread_pool.h ${core_dir}/work_set.h ${core_dir}/work_queue.h ${core_dir}/work_runnable.h ${core_dir}/SharedMatrix.h ${core_dir}/sharedmem/shared_mem.h ${core_dir}/sharedmem/matrix_writer.h ${core_dir}/sharedmem/matrix_reader.h)
file(GLOB core_sources ${core_dir}/SharedMatrix.cpp ${core_dir}/sharedmem/shared_mem.cpp ${core_dir}/sharedmem/matrix_reader.cpp ${core_dir}/sharedmem/matrix_writer.cpp)   

set(algebra_dir src/retin/toolbox/algebra)
file(GLOB algebra_headers ${algebra_dir}/core.h ${algebra_dir}/vector_uchar.h ${algebra_dir}/vector_short.h ${algebra_dir}/vector_float.h ${algebra_dir}/vector_double.h ${algebra_dir}/matrix_float.h  ${algebra_dir}/matrix_double.h ${algebra_dir}/vector.h ${algebra_dir}/matrix.h  ${algebra_dir}/pca.h)
file(GLOB algebra_sources ${algebra_dir}/core.c ${algebra_dir}/vector_uchar.c ${algebra_dir}/vector_short.c ${algebra_dir}/vector_float.c ${algebra_dir}/vector_double.c ${algebra_dir}/matrix_float.c ${algebra_dir}/matrix_double.c ${algebra_dir}/pca.cpp )

set(imcodec_dir src/retin/toolbox/imcodec)
file(GLOB imcodec_headers ${imcodec_dir}/ImageCodec.h   ${imcodec_dir}/ImageCodecPNG.h   ${imcodec_dir}/ImageCodecJPEG.h)
file(GLOB imcodec_sources ${imcodec_dir}/ImageCodec.cpp ${imcodec_dir}/ImageCodecPNG.cpp ${imcodec_dir}/ImageCodecJPEG.cpp)

set(codec3d_dir src/retin/toolbox/codec3d)
file(GLOB codec3d_headers ${codec3d_dir}/Codec3D.h ${codec3d_dir}/Codec3dOFF.h ${codec3d_dir}/Codec3DPLY.h ${codec3d_dir}/ply/ply.h)
file(GLOB codec3d_sources ${codec3d_dir}/Codec3D.cpp ${codec3d_dir}/Codec3DOFF.cpp ${codec3d_dir}/Codec3DPLY.cpp ${codec3d_dir}/ply/ply.cpp)

set(document_dir src/retin/toolbox/document)
file(GLOB document_headers ${document_dir}/Mesh.h ${document_dir}/basic/Polygon.h ${document_dir}/basic/Vector3d.h)
file(GLOB document_sources ${document_dir}/Mesh.cpp ${document_dir}/basic/Polygon.cpp ${document_dir}/basic/Vector3d.cpp)

set(quantization_dir src/retin/toolbox/quantization)
file(GLOB quantization_headers ${quantization_dir}/AbstractCluster.h ${quantization_dir}/AbstractQuantizer.h ${quantization_dir}/ElbgQuantizer.h ${quantization_dir}/NaiveSplittingQuantizer.h ${quantization_dir}/DiagGmmQuantizer.h)
file(GLOB quantization_sources ${quantization_dir}/AbstractCluster.cpp ${quantization_dir}/AbstractQuantizer.cpp ${quantization_dir}/ElbgQuantizer.cpp ${quantization_dir}/NaiveSplittingQuantizer.cpp ${quantization_dir}/DiagGmmQuantizer.cpp)

set(color_dir src/retin/feature/color)
file(GLOB color_headers ${color_dir}/ImageProcessing.h)

set(dfeat_dir src/retin/feature/dfeat)
file(GLOB dfeat_headers ${dfeat_dir}/AbstractDaveFeature.h   ${dfeat_dir}/DaveDSift.h   ${dfeat_dir}/DaveFastSift.h   ${dfeat_dir}/DaveGradient.h ${dfeat_dir}/DaveHog.h)
file(GLOB dfeat_sources ${dfeat_dir}/AbstractDaveFeature.cpp ${dfeat_dir}/DaveDSift.cpp ${dfeat_dir}/DaveFastSift.cpp ${dfeat_dir}/DaveGradient.c ${dfeat_dir}/DaveHog.cpp)

set(depthimages_dir src/retin/feature/depthimages)
file(GLOB depthimages_headers ${depthimages_dir}/DepthImages.h ${depthimages_dir}/DepthCamera.h src/common/Renderer3D.cpp)
file(GLOB depthimages_sources ${depthimages_dir}/DepthImages.cpp  src/common/Renderer3D.h)

set(vlat_dir src/retin/feature/vlat)
file(GLOB vlat_headers ${vlat_dir}/StandardVlatBuilder.h   ${vlat_dir}/StandardPrePcaVlatBuilder.h   ${vlat_dir}/Bag.h   ${vlat_dir}/PackedVlat.h   ${vlat_dir}/FastPackedVlat.h   ${vlat_dir}/PackedVlatBuilder.h   ${vlat_dir}/VlatBuilder.h ${vlat_dir}/AbstractVlatBuilder.h ${vlat_dir}/AbstractVlatCluster.h)
file(GLOB vlat_sources ${vlat_dir}/StandardVlatBuilder.cpp ${vlat_dir}/StandardPrePcaVlatBuilder.cpp ${vlat_dir}/Bag.cpp ${vlat_dir}/PackedVlat.cpp ${vlat_dir}/FastPackedVlat.cpp ${vlat_dir}/PackedVlatBuilder.cpp)

file(GLOB r3d_sources src/common/vlat.cpp src/common/2D3D.cpp src/common/tools.cpp src/common/Renderer3D.cpp)
file(GLOB r3d_headers src/common/vlat.h src/common/tools.hsrc/common/2D3D.h src/common/Renderer3D.h)

file(GLOB common_sources src/common/vlat.cpp src/common/tools.cpp src/common/config_reader.cpp)
file(GLOB common_headers src/common/vlat.h src/common/tools.h src/common/config_reader.h)





add_library(retin_algebra ${algebra_sources})
add_library(retin_quantization ${quantization_sources})
add_library(retin_demo2d3d ${r3d_sources} ${common_sources} ${dfeat_sources})
add_library(retin_dfeat ${dfeat_sources})
add_library(retin_codecs ${imcodec_sources} ${codec3d_sources})
add_library(retin_core ${core_sources} ${document_sources})
add_library(retin_vlat ${common_sources} ${vlat_sources})

set(retin_libs pthread retin_core retin_vlat retin_algebra retin_quantization retin_dfeat retin_demo2d3d pthread)

add_executable(compute_dictionary src/compute_dictionary.cpp)
add_executable(compute_vlat src/compute_vlat.cpp)
add_executable(compute_projectors src/compute_projectors.cpp)
add_executable(compute_compact src/compute_compact.cpp)
add_executable(compute_matrix src/compute_matrix.cpp)
add_executable(similarity_search src/similarity_search.cpp)
add_executable(similarity_server src/similarity_server.cpp)
add_executable(free_memory src/free_memory.cpp)
add_executable(docid_to_path src/docid_to_path.cpp)






set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
find_package(Boost COMPONENTS thread)

find_package(OSMesa)
if(OSMESA_FOUND)
	include_directories(${OSMESA_INCLUDE_DIR})
	add_definitions(-DRETIN_ENABLE_OSMESA)
endif(OSMESA_FOUND)

find_package(OpenGL COMPONENTS GLU)
if(OPENGL_FOUND)
	include_directories(${OpenGL_INCLUDE_DIR})
endif(OPENGL_FOUND)

find_package(PNG)
if(PNG_FOUND)
	include_directories(${PNG_INCLUDE_DIR})
	target_link_libraries(retin_codecs ${PNG_LIBRARIES} )
	add_definitions(-DRETIN_ENABLE_PNG)
endif(PNG_FOUND)

find_package(JPEG)
if(JPEG_FOUND)
	include_directories(${JPEG_INCLUDE_DIR})
	target_link_libraries(retin_codecs ${JPEG_LIBRARIES})
	add_definitions(-DRETIN_ENABLE_JPEG)
endif(JPEG_FOUND)








if(OPENGL_FOUND AND OSMESA_FOUND) 
	add_definitions(-DRETIN_3D)
	add_library(retin_depthimages ${depthimages_sources})
	set(retin_libs ${retin_libs} retin_demo2d3d)
	
	target_link_libraries(retin_depthimages retin_dfeat retin_core retin_codecs)
	target_link_libraries(retin_depthimages ${OSMESA_LIBRARY} )
	target_link_libraries(retin_demo2d3d retin_depthimages retin_vlat)

	target_link_libraries(retin_depthimages ${OPENGL_LIBRARIES} ${OPENGL_glu_LIBRARY})
	
	add_executable(create_thumbnails src/create_thumbnails.cpp)
	target_link_libraries(create_thumbnails ${retin_libs})
	
endif(OPENGL_FOUND AND OSMESA_FOUND)



target_link_libraries(retin_quantization retin_algebra retin_core)
target_link_libraries(retin_dfeat retin_codecs)
	

target_link_libraries(compute_dictionary ${retin_libs})
target_link_libraries(compute_vlat ${retin_libs})
target_link_libraries(compute_projectors ${retin_libs})
target_link_libraries(compute_compact ${retin_libs} retin_core)
target_link_libraries(compute_matrix ${retin_libs})
target_link_libraries(similarity_search ${retin_libs})
target_link_libraries(similarity_server ${retin_libs} microhttpd)
target_link_libraries(free_memory ${retin_libs})
target_link_libraries(docid_to_path ${retin_libs})
