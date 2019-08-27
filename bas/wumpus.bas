    REM
    REM ** Hunt the Wumpus Game
    REM ** A Test Program for Tiny BASIC
    REM

    REM -- Variable List
    REM
    REM A - Bat 1 position
    REM B - Bat 2 position
    REM C - Player position
    REM D - Destination to move or shoot
    REM E - exit 1
    REM F - exit 2
    REM G - exit 3
    REM H - Hole 1 (bottomless pit) position
    REM I - Hole 2 (bottomless pit) position
    REM J - Randomised position for player or hazard
    REM M - menu option for move or shoot
    REM N - range for arrow shot
    REM P - parameter to the "exits" routine
    REM R - random number
    REM S - random number generator seed
    REM W - Wumpus position

    REM -- Intialise the random number generator
  1 PRINT "Think of a number"
    INPUT S

    REM -- Initialise the player and hazard positions
    LET A=0
    LET B=0
    LET C=0
    LET H=0
    LET I=0
    LET W=0

    REM -- Distribute the player and hazards across the map
    GOSUB 710
    LET A=J
    GOSUB 710
    LET B=J
    GOSUB 710
    LET C=J
    GOSUB 710
    LET H=J
    GOSUB 710
    LET I=J
    GOSUB 710
    LET W=J

    REM -- Introductory text
    PRINT "You enter the caves to Hunt the Wumpus!"

    REM -- Main Game Loop
500 PRINT "You are in room ",C
    LET P=C
    GOSUB 900
    GOSUB 600
    PRINT "Exits are ",E,",",F,",",G
510 PRINT "1:Move or 2:Shoot?"
    INPUT M
    IF M<1 THEN GOTO 510
    IF M>2 THEN GOTO 510
    IF M=1 THEN GOSUB 700
    IF M=2 THEN GOSUB 800
    GOTO 500

    REM -- Subroutine to check for hazards

    REM Has the player encountered a bat?
600 IF C<>A THEN IF C<>B THEN GOTO 610
    PRINT "A bat swoops down and picks you up..."
    GOSUB 680
    PRINT "...and drops you down"
    LET P=C
    GOSUB 900

    REM Has the player fallen in a pit?
610 IF C<>H THEN IF C<>I THEN GOTO 620
    PRINT "You fall down a bottomless hole into the abyss!"
    END

    REM Has the player startled the wumpus?
620 IF C<>W THEN GOTO 630
    PRINT "You stumble upon the wumpus!"
    GOSUB 670
    PRINT "The wumpus runs away!"

    REM Is there a pit nearby?
630 IF E<>H THEN IF F<>H THEN IF G<>H THEN IF E<>I THEN IF F<>I THEN IF G<>I THEN GOTO 640
    PRINT "You feel a cold wind blowing from a nearby cavern."

    REM Is the wumpus nearby?
640 IF E<>W THEN IF F<>W THEN IF G<>W THEN GOTO 650
    PRINT "You smell something terrible nearby."

    REM Is there a bat nearby?
650 IF E<>A THEN IF F<>A THEN IF G<>A THEN IF E<>B THEN IF F<>B THEN IF G<>B THEN GOTO 660
    PRINT "You hear a loud squeaking and a flapping of wings."
660 RETURN

    REM Relocate the Wumpus
670 GOSUB 750
    LET R=R-(R/4*4)
    IF R=1 THEN LET W=E
    IF R=2 THEN LET W=F
    IF R=3 THEN LET W=G
    IF W<>C THEN RETURN
    PRINT "The wumpus eats you!"
    END

    REM Relocate bat and player
680 GOSUB 750
    LET R=R-(R/4*4)
    IF R=0 THEN RETURN
    LET P=C
    GOSUB 900
    IF R=1 THEN LET D=E
    IF R=2 THEN LET D=F
    IF R=3 THEN LET D=G
    IF D<>A THEN IF D<>B THEN GOTO 690
    GOTO 680
690 PRINT "...moves you to room ",D,"..."
    IF A=C THEN LET A=D
    IF B=C THEN LET B=D
    LET C=D
    GOTO 680

    REM -- Subroutine to move
700 PRINT "Where?"
    INPUT D
    IF D<>E THEN IF D<>F THEN IF D<>G THEN GOTO 700
    LET C=D
    RETURN

    REM -- Find a random unoccupied position
710 GOSUB 750
    LET J=1+R-R/20*20
    IF J<>A THEN IF J<>B THEN IF J<>C THEN IF J<>H THEN IF J<>I THEN IF J<>W THEN RETURN
    GOTO 710

    REM -- Random number generator
750 LET S=8173*S+35
    LET S=S-S/16384*16384
    LET R=S
    RETURN

    REM -- Subroutine to shoot
800 PRINT "Shoot how far (1-5)?"
    INPUT N
    IF N<1 THEN GOTO 800
    IF N>5 THEN GOTO 800
805 LET P=C
810 GOSUB 900
    PRINT "You can shoot into rooms ",E,",",F,",",G 
815 PRINT "Shoot where?"
    INPUT P
    IF P<>E THEN IF P<>F THEN IF P<>G THEN GOTO 815
    IF P=W THEN GOTO 830
    LET N=N-1
    IF N>0 THEN GOTO 810
    PRINT "The arrow startles the wumpus."
    LET P=W
    GOSUB 900
    GOSUB 670
    RETURN
830 PRINT "You hit the wumpus!"
    END

    REM -- Subroutine to set the exits
    REM Input: P - current position
    REM Outputs: E, F, G (exits)
900 IF P>=1 THEN IF P<=20 THEN GOTO 900+4*P
    PRINT "Illegal position ",P
    END
904 LET E=2
    LET F=5
    LET G=8
    RETURN
908 LET E=1
    LET F=3
    LET G=10
    RETURN
912 LET E=2
    LET F=4
    LET G=12
    RETURN
916 LET E=3
    LET F=5
    LET G=14
    RETURN
920 LET E=1
    LET F=4
    LET G=6
    RETURN
924 LET E=5
    LET F=7
    LET G=15
    RETURN
928 LET E=6
    LET F=8
    LET G=17
    RETURN
932 LET E=1
    LET F=7
    LET G=11
    RETURN
936 LET E=10
    LET F=12
    LET G=19
    RETURN
940 LET E=2
    LET F=9
    LET G=11
    RETURN
944 LET E=8
    LET F=10
    LET G=20
    RETURN
948 LET E=3
    LET F=9
    LET G=13
    RETURN
952 LET E=12
    LET F=14
    LET G=18
    RETURN
956 LET E=4
    LET F=13
    LET G=15
    RETURN
960 LET E=6
    LET F=14
    LET G=16
    RETURN
964 LET E=15
    LET F=17
    LET G=18
    RETURN
968 LET E=7
    LET F=16
    LET G=20
    RETURN
972 LET E=13
    LET F=16
    LET G=19
    RETURN
976 LET E=9
    LET F=18
    LET G=20
    RETURN
980 LET E=11
    LET F=17
    LET G=19
    RETURN