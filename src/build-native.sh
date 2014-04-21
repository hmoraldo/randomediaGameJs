export CPPFILES="*.cpp"
export GCCOPTS="-Wno-write-strings"
export LUAFILES=$(echo lib/lua-5.2.3/src/{lapi.c,lcorolib.c,ldump.c,llex.c,lopcodes.c,lstrlib.c,lauxlib.c,lctype.c,lfunc.c,lmathlib.c,loslib.c,ltable.c,lundump.c,lbaselib.c,ldblib.c,lgc.c,lmem.c,lparser.c,ltablib.c,lvm.c,lbitlib.c,ldebug.c,linit.c,loadlib.c,lstate.c,ltm.c,lzio.c,lcode.c,ldo.c,liolib.c,lobject.c,lstring.c})

gcc -c -g -Ilib/lua-5.2.3/src/ $LUAFILES  && \
ar rcs liblua.a *.o && \
g++ -g -Ilib/lua-5.2.3/src/ $CPPFILES $GCCOPTS liblua.a  -o randomedia  `sdl-config --cflags --libs` -I/usr/include/SDL   -lSDL -lSDL_image && \
echo ok



