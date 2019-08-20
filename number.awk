#
# Tiny BASIC Interpreter and Compiler Project
# Script to add line numbers to a BASIC source file
#
# Released as Public Domain By Damian Gareth Walker 2019
# Created: 14-Aug-2019
#

# Line numbering starts at 1 unless an alternative is specified
BEGIN {
  n = 1;
}

# If there's a line number, check it and adopt it
/^\s*[0-9]+/ {
  if($1 >= n) {
    n = $1;
  } else {
    print $0;
    print "Invalid line number: ", $1;
    exit;
  }
}

# After checking for a line number, completely skip comments
/^\s*[0-9]*\s*REM/ || /^\s*[0-9]*\s*$/ {
  next
}

# If there's no line number, stick one in now
!/^\s*[0-9]+/ {
  $0 = n $0;
}

# Format the output, assuming 3-digit line numbers
!(/^\s*[0-9]*\s*REM/ || /^\s*[0-9]*\s*$/) {
  $1 = "";
  printf "%3d%s\n", n, $0;
  n = n + 1;
}
