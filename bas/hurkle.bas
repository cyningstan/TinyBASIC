    REM
    REM Hunt the Hurkle
    REM A Demonstration Program for Tiny BASIC
    REM

    REM --- Variables
    REM     G: hurkle column
    REM     H: hurkle row
    REM     M: moves taken
    REM     R: random number seed
    REM     X: player guess column
    REM     Y: player guess row

    REM --- Initialise the random number generator
    PRINT "Think of a number."
    INPUT R

    REM --- Initialise the game
    GOSUB 200
    LET G=R-(R/10*10)
    GOSUB 200
    LET H=R-(R/10*10)
    LET M=0

    REM --- Input player guess
 10 PRINT "Where is the hurkle? Enter column then row."
    INPUT X,Y
    IF X>=0 THEN IF X<=9 THEN IF Y>=0 THEN IF Y<=9 THEN GOTO 20
    PRINT "That location is off the grid!"
    GOTO 10

    REM --- Process player guess
 20 LET M=M+1
    PRINT "The Hurkle is..."
    IF G<X THEN IF H<Y THEN PRINT "...to the northwest."
    IF G=X THEN IF H<Y THEN PRINT "...to the north."
    IF G>X THEN IF H<Y THEN PRINT "...to the northeast."
    IF G>X THEN IF H=Y THEN PRINT "...to the east."
    IF G>X THEN IF H>Y THEN PRINT "...to the southeast."
    IF G=X THEN IF H>Y THEN PRINT "...to the south."
    IF G<X THEN IF H>Y THEN PRINT "...to the southwest."
    IF G<X THEN IF H=Y THEN PRINT "...to the west."
    IF G=X THEN IF H=Y THEN GOTO 40
    IF M>6 THEN GOTO 50
    PRINT "You have taken ",M," turns so far."
    GOTO 10

    REM --- Player has won
 40 PRINT "...RIGHT HERE!"
    PRINT "You took ",M," turns to find it."
    END

    REM --- Player has lost
 50 PRINT "You have taken too long over this. You lose!"
    END

    REM --- Random number generator
    REM     Input:   R - current seed
    REM     Outputs: R - updated seed
200 LET R=8173*R+35
    LET R=R-R/16384*16384
    RETURN