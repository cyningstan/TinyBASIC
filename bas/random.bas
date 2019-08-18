    REM
    REM Random Number Testbed
    REM A Demonstration Program for Tiny BASIC
    REM

    REM --- Variables
    REM     C: counter for random numbers
    REM     R: random number generated
    REM     S: random number seed

    REM --- Initialise the random number generator
    PRINT "Think of a number."
    INPUT S

    REM --- Generate and print 10 random numbers
    LET C=0
 10 GOSUB 200
    LET C=C+1
    LET R=R-(R/10*10)
    PRINT C,R
    IF C<10 THEN GOTO 10
    END

    REM --- Random number generator
200 LET S=32767-S*13
    LET R=S/2
    IF R<0 THEN LET R=-R
    RETURN

