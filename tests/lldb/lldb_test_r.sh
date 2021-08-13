CURRENT_DIR=$PWD
PARENT_DIR="$(dirname "$PWD")"
PARENT_DIR="$(dirname "$PARENT_DIR")"
PARENT_DIR="$(dirname "$PARENT_DIR")"
OUT_DIR=$PARENT_DIR/out-release

LUA_CPATH=$OUT_DIR/lib/?.so LD_LIBRARY_PATH=$OUT_DIR/lib $OUT_DIR/bin/lua lldb_test.lua
