    REM
    REM --- Tiny BASIC Interpreter and Compiler Project
    REM --- Hammurabi Demostration Game
    REM
    REM --- Released as Public Domain by Damian Gareth Walker 2019
    REM --- Created: 25-Aug-2019
    REM

    REM --- Variable List
    REM
    REM A - A random number returned by the Random Number Generator
    REM B - The amount of land bought/sold by the player
    REM D - The number of people who died of starvation
    REM F - The amount of grain fed to the people
    REM G - The quantity of stored grain
    REM I - The number of immigrants on a given turn
    REM L - How much land the city owns
    REM P - The number of people in the city
    REM R - The amount of grain eaten by rats
    REM S - The amount of grain planted as seed
    REM T - Time played
    REM V - The value of land per acre
    REM Y - The grain yield of each piece of land
    REM Z - The random number seed

    REM --- Initialise the random number seed
    PRINT "Think of a number."
    INPUT Z

    REM --- Initialise the game
    LET D=0
    LET G=2800
    LET I=5
    LET L=1000
    LET P=100
    LET R=200
    LET T=1
    LET Y=3

    REM --- Print the report
 20 PRINT "Hammurabi, I beg to report to you,"
    PRINT "In year ",T,", ",D," people starved, and ",I," came to the city."
    PRINT "You harvested ",Y," bushels of grain per acre."
    PRINT "Rats destroyed ",R," bushels."
    PRINT "Population is now ",P,", and the city owns ",L," acres of land."
    PRINT "You have ",G," bushels of grain in store."

    REM --- Buy land
    GOSUB 250
    LET V=17+A-A/10*10
    PRINT "Land is trading at ",V," bushels per acre."
 30 PRINT "How many acres do you wish to buy?"
    INPUT B
    IF B>G/V THEN GOTO 30
    IF B>0 THEN GOTO 40

    REM --- Sell land
 35 PRINT "How many acres do you wish to sell?"
    INPUT B
    IF B>L THEN GOTO 35
    LET B=-B

    REM --- Feed the people
 40 PRINT "How many bushels do you wish to feed the people?"
    INPUT F
    IF F>=0 THEN IF F<=G-B*V THEN GOTO 45
    GOTO 40

    REM --- Plant with seed
 45 PRINT "How many acres do you wish to plant with seed?"
    INPUT S
    IF S>=0 THEN IF S/2<=G-F-B*V THEN IF S<=10*P THEN IF S<=L+B THEN GOTO 50
    GOTO 45

    REM --- Work out the result
 50 LET L=L+B
    LET G=G-B*V-F-S/2

    REM Yield
    GOSUB 250
    LET Y=1+A-A/5*5

    REM Rats
    GOSUB 250
    LET A=1+A-A/5*5
    LET R=0
    IF A>2 THEN GOTO 65
    GOSUB 250
    IF G>0 THEN LET R=1+A-A/G*G

    REM Recalculate grain
 65 LET G=G+S*Y-R
    IF G<0 THEN LET G=0

    REM Immigration/Birth
    GOSUB 250
    LET A=1+A-A/5*5
    LET I=A*(20*L+S)/P/100+1

    REM Feeding the people
    IF P<=F THEN GOTO 80
    LET D=P-F
    IF D>P*45/100 THEN GOTO 200
 80 LET P=P-D+I
    IF P<=0 THEN GOTO 210
    LET T=T+1
    IF T<=10 THEN GOTO 20

    REM --- Victory
    PRINT "You have ruled well for 10 years."
    PRINT "You leave your city with ",P," people, "
    PRINT L," acres of land and ",G," bushels of grain stored."
    END

    REM --- Too many people starved
200 PRINT "You let too many people starve in one year!"
    PRINT "Riots in the streets have driven you from your throne."
    END

    REM --- Everybody starved
210 PRINT "You have starved your entire kingdom to death!"
    END

    REM --- Random Number Generator
250 LET Z=8173*Z+35
    LET Z=Z-Z/16384*16384
    LET A=Z
    RETURN
