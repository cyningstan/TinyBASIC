    PRINT "A little Tiny BASIC demonstration."
    LET X=5
    PRINT "X=",X
    LET Y=6
    PRINT "Y=",Y
    LET Z=Y*X
    PRINT "Z=",Y,"*",X,"=",Z
    IF Z=Y*X THEN PRINT "You should see this."
    IF Z<>Y*X THEN PRINT "You shouldn't see this."
    IF Z>10 THEN IF Z<40 THEN PRINT "You should see this too."
    IF Z>10 THEN IF Z>=40 THEN PRINT "You shouldn't see this either."