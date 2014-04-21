export EMOPTS=" -Qunused-arguments -s ERROR_ON_UNDEFINED_SYMBOLS=1  -s TOTAL_MEMORY=268435456 -Wno-deprecated-writable-strings -Wno-invalid-source-encoding "
export CPPFILES="*.cpp"
export LUAFILES=$(echo lib/lua-5.2.3/src/{lapi.c,lcorolib.c,ldump.c,llex.c,lopcodes.c,lstrlib.c,lauxlib.c,lctype.c,lfunc.c,lmathlib.c,loslib.c,ltable.c,lundump.c,lbaselib.c,ldblib.c,lgc.c,lmem.c,lparser.c,ltablib.c,lvm.c,lbitlib.c,ldebug.c,linit.c,loadlib.c,lstate.c,ltm.c,lzio.c,lcode.c,ldo.c,liolib.c,lobject.c,lstring.c})

emcc -Ilib/lua-5.2.3/src/ $LUAFILES $EMOPTS -o lualib.bc && \
em++ -Ilib/lua-5.2.3/src/ lualib.bc $CPPFILES  -o randomedia.html   --preload-file data/  $EMOPTS  && \
echo ok

# to test: python -m SimpleHTTPServer 8000

