    REM **********************************************************
    REM * Tiny BASIC Testbed Program                             *
    REM *                                                        *
    REM * This is the celebrated FizzBuzz program in Tiny BASIC. *
    REM *                                                        *
    REM * Copyright (C) Damian Gareth Walker 2019                *
    REM * Created: 04-Aug-2019                                   *
    REM **********************************************************

    REM Initialise
    LET C = 1

    REM Check for relevant factors
100 IF C/15*15 = C THEN GOTO 115
    IF C/5*5 = C THEN GOTO 105
    IF C/3*3 = C THEN GOTO 103

    REM No factors, so print the number
    PRINT C
    GOTO 199

    REM Divides by 3, so print "Fizz"
103 PRINT "Fizz"
    GOTO 199

    REM Divides by 5, so print "Buzz"
105 PRINT "Buzz"
    GOTO 199

    REM Divides by both 3 and 5, so print "FizzBuzz"
115 PRINT "FizzBuzz"

    REM Increment C and repeat until 100 reached
199 LET C=C+1
    IF C<=100 THEN GOTO 100
    END

