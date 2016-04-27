cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${ROSS_SOURCE_DIR} ${ROSS_BINARY_DIR})

# All source files for the model
SET(traffic_srcs
  traffic.c
  traffic.h
)

# Set the executable name here: model
# You will be executing the ./model binary
ADD_EXECUTABLE(ross_traffic ${traffic_srcs})

TARGET_LINK_LIBRARIES(ross_traffic ROSS m)
## Alternatively, use this command with RIO
#TARGET_LINK_LIBRARIES(model ROSS RIO m)
