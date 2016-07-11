#[Loopr](https://github.com/Byrd-Lin/Loopr "Loopr Github Page")

#####Loopr is a framework designed for L1 Assembler to compile and execute.#####
<br/>
##Build & Test

###Linux?<br/>
First, clone the source code with the clone URL above<br/>
Then, compile it:<br/>

    #cd into the source dir
    cd src
    make

**NOTE:** you can use make clean to clean up the source code<br/><br/>
Start the test:

    ./testbed loop.lp

<br/>
If everything is OK, the output will be like this:

    ---Basic Instruction Test---

    LOAD:
    10		= 10
    (byte)260	= 4
    'a'		= 97
    true		= 1
    false	= 0
    10.500000l	= 10.500000

    null = (null)
    Uninitialized local variable = (null)
    test string: This is a test string
    array[1][2][3] = 12
    box&unbox: Orignal Data

    LOGIC:
    10 == 11   ... false
    3.1 == 3.1 ... false
    11 != 12   ... true
    1 != 1     ... false
    -2.0 > 1.0 ... false
    12 > 12    ... false
    13 < 14    ... true
    12 < 11    ... false

    if (true == true)  ... is true
    if (false == true) ... is false
    if (5 == (byte)5)  ... is true

    OPERATION:
    1 + 5 = 6
    20 - 352 = -332
    16 / 5 = 3
    20 * 720 = 14400

    (20 + 720) / 3 * 5 + 2015 - 200 = 3045
    (20.0 + 720.0) / 3.0 * 5.0 + 2015.0 - 200.0 = 3048.333333

    NAMESPACE:
    namespace TestSpace recieved: send from TestMain
    I'm the one you used...
    it's test function in test_sub

    press a key to exit...
then type enter to exit

**NOTE:** Loopr for Windows is coming soon
<br/><br/>
