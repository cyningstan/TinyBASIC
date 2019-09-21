    REM --- Variables
    REM     P - player number (various uses)
    REM     R - top-left square
    REM     S - top square
    REM     T - top-right square
    REM     U - left square
    REM     V - centre square
    REM     W - right square
    REM     X - bottom-left square
    REM     Y - bottom square
    REM     Z - bottom-right square

    REM --- Subroutine to check for a player's move
    REM     Inputs:  P   - player number
    REM     Changes: M   - the square the player wishes to place their piece
    REM              R-Z - the board
100 PRINT "Move? "
    INPUT M
    IF M<1 THEN GOTO 100
    IF M>9 THEN GOTO 100
    GOSUB 220
    IF N<>0 THEN GOTO 100


    REM --- Subroutine to check for a win
    REM     Inputs:  R-Z - board squares
    REM     Outputs: P   - the winning player (0 for neither)
200 LET P=0
    IF R>0 THEN IF R=S THEN IF S=T THEN LET P=R
    IF U>0 THEN IF U=V THEN IF V=W THEN LET P=U
    IF X>0 THEN IF X=Y THEN IF Y=Z THEN LET P=X
    IF R>0 THEN IF R=U THEN IF U=X THEN LET P=R
    IF S>0 THEN IF S=V THEN IF V=Y THEN LET P=S
    IF T>0 THEN IF T=W THEN IF W=Z THEN LET P=T
    IF R>0 THEN IF R=V THEN IF V=Z THEN LET P=R
    IF T>0 THEN IF T=V THEN IF V=X THEN LET P=T
    RETURN

    REM --- Subroutine to see what piece is in a square
    REM     Inputs:  M   - the square to check
    REM     Outputs: N   - the piece in that square
220 LET N=0
    IF M=1 THEN LET N=R
    IF M=2 THEN LET N=S
    IF M=3 THEN LET N=T
    IF M=4 THEN LET N=U
    IF M=5 THEN LET N=V
    IF M=6 THEN LET N=W
    IF M=7 THEN LET N=X
    IF M=8 THEN LET N=Y
    IF M=9 THEN LET N=Z
    RETURN