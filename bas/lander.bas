    REM
    REM --- Lunar Lander Game
    REM --- A demonstration program for Tiny BASIC
    REM

    REM --- Variables:
    REM     A: altitude
    REM     B: fuel to burn this turn
    REM     F: fuel remaining
    REM     T: time elapsed
    REM     V: velocity this turn
    REM     W: velocity next turn

    REM --- Initialise the Program
    LET A=1000
    LET B=0
    LET F=150
    LET V=50
    LET T=0

    REM --- Main Loop
 10 PRINT "Time:",T," Altitude:",A," Velocity:",V," Fuel:",F," Thrust:",B
 11 IF F>30 THEN PRINT "Thrust (0-30)?"
    IF F<31 THEN PRINT "Thrust (0-",F,")?"
    INPUT B
    IF B>=0 THEN IF B<=30 THEN IF B<=F THEN GOTO 20
    GOTO 11
 20 LET W=V-B+5
    LET F=F-B
    LET A=A-(V+W)/2
    LET V=W
    LET T=T+1
    IF A>0 THEN GOTO 10

    REM --- End of Game
    IF V<5 THEN GOTO 40
    PRINT "You crashed!"
    GOTO 60
 40 IF A>0 THEN GOTO 50
    PRINT "Perfect landing!"
    GOTO 60
 50 PRINT "Touchdown."
 60 IF A<0 THEN LET A=0
    PRINT "Time:",T," Altitude:",A," Velocity:",V," Fuel:",F," Thrust:",B
    END