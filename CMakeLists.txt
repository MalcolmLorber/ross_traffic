INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR} ${ROSS_BINARY_DIR})

SET(airport_srcs
airport.c		airport.h)

ADD_EXECUTABLE(traffic ${traffic_srcs})

TARGET_LINK_LIBRARIES(traffic ROSS m)
