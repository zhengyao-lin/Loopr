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
	   10			= 10
	   (byte)260	= 4
	   'a'			= 97
	   true			= 1
	   false		= 0
	   10.500000l	= 10.500000

	   null	= (null)
	   Uninitialized local variable = (null)
	   test string: This is a test string
	   array[1][2][3] = 12

	LOGIC:
	   10 == 11   ... false
	   120 == 120 ... true
	   11 != 12   ... true
	   1 != 1     ... false
	   12 > 13    ... false
	   12 > 12    ... false
	   13 < 14    ... true
	   12 < 11    ... false

	press a key to exit...
then type enter to exit

**NOTE:**Loopr for Windows is comming soon
<br/><br/>
