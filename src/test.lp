.maxstack 5
.entry

.def float1
.def double1
.def integer1
.def object1

ldstr "你好，我是林正耀0222你好"
stloc "integer1"
ldloc "integer1"
ldstr "。我考砸了。"
addstr
popstr

ldloc "integer1"

popstr
ldb.i32 22
ldb.i32 2222
mulb
popb

ldb.b8 240
ldb.i32 240
mulb
ldb.s8 5
divb
popb

//ldb.i32 '\x76';
//popb;

ldnull
popstr

ldb.f8 3.141592654
stloc "float1"

loop0:
goto loop1;

ldloc "float1"
bx
unbx
popf

//goto 40;
loop1:

ldb.8 2000
ldb.8 5
ldb.i32 500
ldb.i32 200
ldb.i32 100

addb
subb
mulb
divb
popb

ldb #-12
popb

loop2:
loop3:
loop4:
