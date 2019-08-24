    PRINT "Fibonnaci numbers:"
    LET A=0
    LET B=1
    PRINT A
100 PRINT B
    LET B=A+B
    LET A=B-A
    IF B<=1000 THEN GOTO 100