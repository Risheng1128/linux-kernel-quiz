set title "Time"
set xlabel "target char position in string"
set ylabel "time(ns)"
set terminal png font " Times_New_Roman,12 "
set output "time.png"
set xtics 0 ,100, 1000

plot \
"final" using :1 with linespoints linewidth 2 title "origin", \
"final" using :2 with linespoints linewidth 2 title "opt", \
"final" using :3 with linespoints linewidth 2 title "x86-64", \