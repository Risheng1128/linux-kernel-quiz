set title "Time"
set xlabel "test"
set ylabel "time(ns)"
set terminal png font " Times_New_Roman,12 "
set output "time.png"
set xtics 0 ,10, 50

plot \
"out" using :1 with linespoints linewidth 2 title "gcd", \
"out" using :2 with linespoints linewidth 2 title "gcdctz", \