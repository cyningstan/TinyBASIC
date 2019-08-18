  1 PRINT "Think of a number."
  2 INPUT S
  3 GOSUB 200
  4 LET G=R-(R/10*10)
  5 GOSUB 200
  6 LET H=R-(R/10*10)
  7 LET M=0
 10 PRINT "Where is the mugwump? Enter column then row."
 11 INPUT X,Y
 12 IF X>=0 THEN IF X<=9 THEN IF Y>=0 THEN IF Y<=9 THEN GOTO 20
 13 PRINT "That location is off the grid!"
 14 GOTO 10
 20 LET M=M+1
 21 PRINT "The mugwump is..."
 22 LET D=0
 23 LET C=G-X
 24 GOSUB 60
 25 LET C=H-Y
 26 GOSUB 60
 27 IF D=0 THEN GOTO 40
 28 PRINT "...",D," cells away."
 29 IF M>10 THEN GOTO 50
 30 PRINT "You have taken ",M," turns so far."
 31 GOTO 10
 40 PRINT "...RIGHT HERE!"
 41 PRINT "You took ",M," turns to find it."
 42 END
 50 PRINT "You have taken too long over this. You lose!"
 51 END
 60 IF C<0 THEN LET C=-C
 61 LET D=D+C
 62 RETURN
200 LET S=32767-S*13
201 LET R=S/2
202 IF R<0 THEN LET R=-R
203 RETURN
