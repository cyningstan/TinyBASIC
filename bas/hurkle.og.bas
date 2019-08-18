  1 PRINT "Think of a number."
  2 INPUT S
  3 GOSUB 200
  4 LET G=R-(R/10*10)
  5 GOSUB 200
  6 LET H=R-(R/10*10)
  7 LET M=0
 10 PRINT "Where is the hurkle? Enter column then row."
 11 INPUT X,Y
 12 IF X>=0 THEN IF X<=9 THEN IF Y>=0 THEN IF Y<=9 THEN GOTO 20
 13 PRINT "That location is off the grid!"
 14 GOTO 10
 20 LET M=M+1
 21 PRINT "The Hurkle is..."
 22 IF G<X THEN IF H<Y THEN PRINT "...to the northwest."
 23 IF G=X THEN IF H<Y THEN PRINT "...to the north."
 24 IF G>X THEN IF H<Y THEN PRINT "...to the northeast."
 25 IF G>X THEN IF H=Y THEN PRINT "...to the east."
 26 IF G>X THEN IF H>Y THEN PRINT "...to the southeast."
 27 IF G=X THEN IF H>Y THEN PRINT "...to the south."
 28 IF G<X THEN IF H>Y THEN PRINT "...to the southwest."
 29 IF G<X THEN IF H=Y THEN PRINT "...to the west."
 30 IF G=X THEN IF H=Y THEN GOTO 40
 31 IF M>6 THEN GOTO 50
 32 PRINT "You have taken ",M," turns so far."
 33 GOTO 10
 40 PRINT "...RIGHT HERE!"
 41 PRINT "You took ",M," turns to find it."
 42 END
 50 PRINT "You have taken too long over this. You lose!"
 51 END
200 LET S=32767-S*13
201 LET R=S/2
202 IF R<0 THEN LET R=-R
203 RETURN
