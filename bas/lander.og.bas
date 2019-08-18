  1 LET A=1000
  2 LET B=0
  3 LET F=150
  4 LET V=50
  5 LET T=0
 10 PRINT "Time:",T," Altitude:",A," Velocity:",V," Fuel:",F," Thrust:",B
 11 IF F>30 THEN PRINT "Thrust (0-30)?"
 12 IF F<31 THEN PRINT "Thrust (0-",F,")?"
 13 INPUT B
 14 IF B>=0 THEN IF B<=30 THEN IF B<=F THEN GOTO 20
 15 GOTO 11
 20 LET W=V-B+5
 21 LET F=F-B
 22 LET A=A-(V+W)/2
 23 LET V=W
 24 LET T=T+1
 25 IF A>0 THEN GOTO 10
 26 IF V<5 THEN GOTO 40
 27 PRINT "You crashed!"
 28 GOTO 60
 40 IF A>0 THEN GOTO 50
 41 PRINT "Perfect landing!"
 42 GOTO 60
 50 PRINT "Touchdown."
 60 IF A<0 THEN LET A=0
 61 PRINT "Time:",T," Altitude:",A," Velocity:",V," Fuel:",F," Thrust:",B
