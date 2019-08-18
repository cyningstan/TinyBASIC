CFLAGS = -Wall

tiny: tiny.o tokeniser.o token.o parser.o statement.o expression.o

tiny.o: tiny.c token.h tokeniser.h parser.h

tokeniser.o: tokeniser.c tokeniser.h token.h

token.o: token.c token.h

parser.o: parser.c parser.h tokeniser.h token.h statement.h expression.h errors.h

statement.o: statement.c statement.h expression.h

expression.o: expression.c expression.h