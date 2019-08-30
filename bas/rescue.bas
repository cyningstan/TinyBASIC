    REM
    REM Tiny BASIC Interpreter and Compiler Project
    REM Intergalactic Space Rescue Demonstration Game
    REM
    REM Released as Public Domain by Damian Gareth Walker, 2019
    REM Created: 30-Aug-19
    REM

    REM --- Variables
    REM
    REM C - Counter for examining locations
    REM D - Distance from vessel to wreck
    REM H - Hyperspaces in this level
    REM L - Current level
    REM P - Position argument for subroutines
    REM Q - Temporary store
    REM R - Random number seed and result
    REM S - Random seed temporary store
    REM T - Location Type
    REM V - Position of rescue vessel
    REM W - Position of shipwreck

    REM --- Initialise the game
    LET L=0
    PRINT "Fools! Don't they know that space is DANGEROUS?"
    PRINT ""
    PRINT "You are the captain of a rescue ship. You must travel from"
    PRINT "level to level, responding to the distress calls of"
    PRINT "stricken starships, searching them out and rescuing their"
    PRINT "passengers."

    REM --- Main Game Loop
  8 PRINT ""
    PRINT "*** Level: ",L
    GOSUB 7
    GOSUB 10
    GOSUB 11
    GOTO 8


    REM Subroutine to initialise the game level
    REM Inputs:  L - level number
    REM Outputs: H - number of hyperspaces (via subroutine)
    REM          V - location of rescue vessel
    REM          W - location of shipwreck

    REM --- Count the hyperspaces
  7 LET P=99
    GOSUB 2

    REM --- Initialise rescue vessel
  4 GOSUB 1
    LET P=R-R/100*100
    GOSUB 2
    IF T<>0 THEN GOTO 4
    LET V=P

    REM --- Initialise shipwreck
  5 GOSUB 1
    LET P=R-R/100*100
    GOSUB 2
    IF T<>0 THEN GOTO 5
    IF P=V THEN GOTO 5
    LET W=P

    REM --- Check distance
    GOSUB 6
    IF D<10 THEN GOTO 4

    REM --- Fill fuel tank and return
    LET F=18
    RETURN

    REM --- Subroutine to make one move
    REM Changes: F - Fuel in tank
    REM          V - Vessel location

    REM --- Print status
 10 PRINT "Location of vessel: ",V-V/10*10,",",V/10
    GOSUB 6
    PRINT "Distance to wreck: ",D
    PRINT "Fuel left in tank: ",F

    REM --- Ask and verify direction
    PRINT "Move (2=down 4=left 6=right 8=up)?"
 12 INPUT M
    IF M<>2 THEN IF M<>4 THEN IF M<>6 THEN IF M<>8 THEN GOTO 12
    IF M=2 THEN IF V>=90 THEN GOTO 12
    IF M=4 THEN IF V-V/10*10=0 THEN GOTO 12
    IF M=6 THEN IF V-V/10*10=9 THEN GOTO 12
    IF M=8 THEN IF V<10 THEN GOTO 12
    PRINT ""

    REM --- Move the ship
    IF M=2 THEN LET V=V+10
    IF M=4 THEN LET V=V-1
    IF M=6 THEN LET V=V+1
    IF M=8 THEN LET V=V-10

    REM --- React to what's there
    LET P=V
    GOSUB 2
    GOSUB 13+T

    REM --- Continue or end the game
    IF V<>W THEN IF F>0 THEN GOTO 10
    RETURN

    REM --- End of level subroutine
    REM Inputs:  F - Fuel remaining
    REM          V - Rescue vessel location
    REM          W - Shipwreck location
    REM Changes: L - Level number

    REM --- Check for out of fuel
 11 IF F>0 THEN GOTO 17
    PRINT "You ran out of fuel! Try again."
    RETURN

    REM --- Advance to another level
 17 IF L=99 THEN GOTO 18
    LET L=L+F
    IF L>99 THEN LET L=99
    PRINT "Well done! You advance to level ",L
    RETURN

    REM --- Complete victory
 18 PRINT "Victory!"
    PRINT "You have finished all the missions."
    PRINT "Enjoy your well-deserved retirement."
    END

    REM --- Ascertain the distance between vessel and wreck
    REM Inputs:  V - vessel position
    REM          W - wreck position
    REM Outputs: D - distance
  6 LET Q=(V-V/10*10)-(W-W/10*10)
    IF Q<0 THEN LET Q=-Q
    LET D=Q
    LET Q=V/10-W/10
    IF Q<0 THEN LET Q=-Q
    LET D=D+Q
    RETURN

    REM Deep space subroutine
    REM Outputs: F - fuel is reduced by 1
 13 PRINT "You are in deep space."
    LET F=F-1
    RETURN

    REM Planet subroutine
    REM Outputs: F - fuel is restored to full
 14 PRINT "You land on a planet. Your fuel is restored!"
    LET F=18
    RETURN

    REM Asteroids subroutine
    REM Outputs: F - fuel is reduced by a random amount
 15 PRINT "Asteroid field! Safe navigation saps your fuel."
    GOSUB 1
    LET F=R-R/F*F
    RETURN

    REM Hyperspace subroutine
    REM Outputs: F - fuel is reduced by 1
    REM          V - vessel arrives at another hyperspace
 16 PRINT "((HYPERSPACE!))"
    PRINT "You are transported to another part of the galaxy."
  9 GOSUB 1
    LET P=R-R/100*100
    IF P=V THEN GOTO 9
    GOSUB 2
    IF T<>3 THEN GOTO 9
    LET V=P
    LET F=F-1
    RETURN

    REM --- Ascertain what's at a given location
    REM Inputs:  L - map level
    REM          P - position to examine
    REM Changes: C - location counter
    REM          Q - temporary store
    REM          S - Used to preserve and restore random seed
    REM Outputs: H - a count of hyperspaces if P=99
    REM          T - location type at position
  2 LET S=R
    LET R=1+L*40
    IF P=99 THEN LET H=0
    LET C=-1
  3 GOSUB 1
    LET C=C+1
    IF C<=P THEN IF P<99 THEN GOTO 3
    LET Q=R-R/100*100
    LET T=0
    IF Q<10 THEN LET T=1
    IF T=0 THEN IF Q<20+L*2/10 THEN LET T=2
    IF T=0 THEN IF Q<30+L*4/10 THEN LET T=3
    IF T=3 THEN IF P=99 THEN LET H=H+1
    IF C<P THEN GOTO 3
    LET R=S
    RETURN

    REM --- Random Number Generator Subroutine
    REM Inputs:  R - random number seed
    REM Outputs: R - random number generated
  1 LET R=5*R+35
    LET R=R-R/4096*4096
    RETURN