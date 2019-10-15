    REM
    REM --- Tiny BASIC Interpreter and Compiler Project
    REM --- Hunt the Wumpus Demonstration Game
    REM
    REM --- Released as Public Domain by Damian Gareth Walker 2019
    REM --- Created: 08-Aug-2019
    REM

    REM --- Variable List
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
    REM K - origin location of arrow in motion (before L)
    REM L - previous location of arrow in motion
    REM M - menu option for move or shoot
    REM N - range for arrow shot
    REM P - parameter to the "exits" routine
    REM Q - number of arrows in quiver
    REM R - random number
    REM S - random number generator seed
    REM W - Wumpus position

    REM --- Intialise the random number generator
 32 PRINT "Think of a number"
    INPUT S

    REM --- Initialise the player and hazard positions
    LET A=0
    LET B=0
    LET C=0
    LET H=0
    LET I=0
    LET W=0

    REM --- Fill the player's quiver
    LET Q=5

    REM --- Distribute the player and hazards across the map
    GOSUB 150
    LET A=J
    GOSUB 150
    LET B=J
    GOSUB 150
    LET C=J
    GOSUB 150
    LET H=J
    GOSUB 150
    LET I=J
    GOSUB 150
    LET W=J

    REM --- Introductory text
    PRINT "You enter the caves to Hunt the Wumpus!"

    REM --- Main Game Loop
 64 PRINT "You are in room ",C
    LET P=C
    GOSUB 202
    GOSUB 80
    PRINT "Exits are ",E,",",F,",",G
 69 PRINT "1:Move or 2:Shoot?"
    INPUT M
    IF M<1 THEN GOTO 69
    IF M>2 THEN GOTO 69
    IF M=1 THEN GOSUB 143
    IF M=2 THEN GOSUB 164
    GOTO 64

    REM --- Subroutine to check for hazards

    REM Has the player encountered a bat?
 80 IF C<>A THEN IF C<>B THEN GOTO 88
    PRINT "A bat swoops down and picks you up..."
    GOSUB 126
    PRINT "...and drops you down"
    LET P=C
    GOSUB 202

    REM Has the player fallen in a pit?
 88 IF C<>H THEN IF C<>I THEN GOTO 93
    PRINT "You fall down a bottomless hole into the abyss!"
    END

    REM Has the player startled the wumpus?
 93 IF C<>W THEN GOTO 99
    PRINT "You stumble upon the wumpus!"
    GOSUB 116
    PRINT "The wumpus runs away!"

    REM Is there a pit nearby?
 99 IF E<>H THEN IF F<>H THEN IF G<>H THEN GOTO 101
    GOTO 102
101 IF E<>I THEN IF F<>I THEN IF G<>I THEN GOTO 105
102 PRINT "You feel a cold wind blowing from a nearby cavern."

    REM Is the wumpus nearby?
105 IF E<>W THEN IF F<>W THEN IF G<>W THEN GOTO 109
    PRINT "You smell something terrible nearby."

    REM Is there a bat nearby?
109 IF E<>A THEN IF F<>A THEN IF G<>A THEN GOTO 111
    GOTO 112
111 IF E<>B THEN IF F<>B THEN IF G<>B THEN GOTO 113
112 PRINT "You hear a loud squeaking and a flapping of wings."
113 RETURN

    REM --- Relocate the Wumpus
116 GOSUB 158
    LET R=R-(R/4*4)
    IF R=1 THEN LET W=E
    IF R=2 THEN LET W=F
    IF R=3 THEN LET W=G
    IF W<>C THEN RETURN
    PRINT "The wumpus eats you!"
    END

    REM --- Relocate bat and player
126 GOSUB 158
    LET R=R-(R/4*4)
    IF R=0 THEN RETURN
    LET P=C
    GOSUB 202
    IF R=1 THEN LET D=E
    IF R=2 THEN LET D=F
    IF R=3 THEN LET D=G
    IF D<>A THEN IF D<>B THEN GOTO 136
    GOTO 126
136 PRINT "...moves you to room ",D,"..."
    IF A=C THEN LET A=D
    IF B=C THEN LET B=D
    LET C=D
    GOTO 126

    REM --- Subroutine to move
143 PRINT "Where?"
    INPUT D
    IF D<>E THEN IF D<>F THEN IF D<>G THEN GOTO 143
    LET C=D
    RETURN

    REM -- Find a random unoccupied position
150 GOSUB 158
    LET J=1+R-R/20*20
    IF J<>A THEN IF J<>B THEN IF J<>C THEN GOTO 154
    GOTO 150
154 IF J<>H THEN IF J<>I THEN IF J<>W THEN RETURN
    GOTO 150

    REM --- Random number generator
158 LET S=5*S+35
    LET S=S-S/4096*4096
    LET R=S
    RETURN

    REM --- Subroutine to shoot
164 PRINT "Shoot how far (1-5)?"
    INPUT N
    IF N<1 THEN GOTO 164
    IF N>5 THEN GOTO 164
168 LET P=C
    LET L=0
170 GOSUB 202
    LET K=L
    LET L=P
    PRINT "Arrow is next to rooms ",E,",",F,",",G 
174 PRINT "Shoot where?"
    INPUT P
    IF P<>E THEN IF P<>F THEN IF P<>G THEN GOTO 189
    IF P=K THEN GOTO 191
    IF P=W THEN GOTO 196
    LET N=N-1
    IF N>0 THEN GOTO 170
    LET Q=Q-1
    PRINT "The arrow startles the wumpus."
    LET P=W
    GOSUB 202
    GOSUB 116
    IF Q=0 THEN GOTO 193
    PRINT "You have ",Q," arrows left."
    RETURN
189 PRINT "The arrow can't reach there."
    GOTO 174
191 PRINT "The arrow can't double back on itself."
    GOTO 174
193 PRINT "You used your last arrow!"
    PRINT "Your demise is now inevitable."
    END
196 PRINT "You hit the wumpus!"
    END

    REM --- Subroutine to set the exits
    REM Input: P - current position
    REM Outputs: E, F, G (exits)
202 IF P>=1 THEN IF P<=20 THEN GOTO 205
    PRINT "Illegal position ",P
    END
205 GOTO 200+10*((14+P)/10)

    REM --- Outer caves
210 LET E=P-1
    IF E=0 THEN LET E=5
    LET F=P+1
    IF F=6 THEN LET F=1
    LET G=4+2*P
    RETURN

    REM --- Middle caves
220 LET E=P-1
    IF E=5 THEN LET E=15
    LET F=P+1
    IF F=16 THEN LET F=6
    IF P/2*2<>P THEN LET G=13+P/2
    IF P/2*2=P THEN LET G=P/2-2
    RETURN

    REM --- Inner caves
230 LET E=P-1
    IF E=15 THEN LET E=20
    LET F=P+1
    IF F=21 THEN LET F=16
    LET G=2*P-25
    RETURN