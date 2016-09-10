cmake_minimum_required(VERSION 3.6)
project(HBreakout)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z -mconsole")
set(SOURCE_FILES main.cpp BasicDTMenager.hpp WIN_TimeMenager.cpp WIN_TimeMenager.hpp FT_to_SDL_helpers.cpp FT_to_SDL_helpers.hpp SDL_memory.hpp error_log.hpp error_log.cpp FontCache.cpp FontCache.hpp FontGlyph.cpp FontGlyph.hpp FontMenager.cpp FontMenager.hpp FPSWidget.cpp FPSWidget.hpp SDL_helpers.cpp SDL_helpers.hpp intersection2D.cpp intersection2D.hpp TimeMenager.cpp TimeMenager.hpp CollisionTester.cpp CollisionTester.hpp GameInterfaces.cpp GameInterfaces.hpp VecCoords.hpp Vector.hpp StaticSquareCollider.cpp StaticSquareCollider.hpp Ball.cpp Ball.hpp Brick.cpp Brick.hpp Bounds.cpp Bounds.hpp Paddle.cpp Paddle.hpp Game.cpp Game.hpp op_type.hpp)
add_executable(HBreakout ${SOURCE_FILES})

find_package(SDL2 REQUIRED)
target_include_directories(HBreakout SYSTEM PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(HBreakout ${SDL2_LIBRARIES} -lSDL2_Image)

# Freetype 2
#find_package(Freetype REQUIRED)
set(FREETYPE_INCLUDE_DIRS "C:/dev-env/include/freetype2/")
set(FREETYPE_LIBRARIES "-LC:/dev-env/lib/ -lfreetype")
target_include_directories(HBreakout SYSTEM PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(HBreakout ${FREETYPE_LIBRARIES})

install(TARGETS HBreakout
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
install(DIRECTORY main_folder/fonts main_folder/images DESTINATION /)