$rate = 10000
For ($i=9; $i -lt 50; $i++)
{
    "set term pngcairo"  | Set-Content 'gnuplot_script.txt'
    "set output 'trajectory_" + ($i-8) + ".png'" | Add-Content 'gnuplot_script.txt'
    "set xrange [0:0.005]" | Add-Content 'gnuplot_script.txt'
    "set yrange [0:0.005]" | Add-Content 'gnuplot_script.txt'
    "set zrange [0:0.01]" | Add-Content 'gnuplot_script.txt'
    "splot 'plot_" + ($i*$rate) + ".txt' u 1:2:3 w p pt 7 ps 2 lt rgb '#0015ff'," | Add-Content 'gnuplot_script.txt'-NoNewline
    " 'plot_" + (($i-1)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 1.8 lt rgb '#3f4fff'," | Add-Content 'gnuplot_script.txt' -NoNewline
    " 'plot_" + (($i-2)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 1.6 lt rgb '#606eff'," | Add-Content 'gnuplot_script.txt' -NoNewline
    " 'plot_" + (($i-3)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 1.4 lt rgb '#6d7aff'," | Add-Content 'gnuplot_script.txt' -NoNewline
    " 'plot_" + (($i-4)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 1.2 lt rgb '#7a86ff'," | Add-Content 'gnuplot_script.txt' -NoNewline
    " 'plot_" + (($i-5)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 1 lt rgb '#8994ff'," | Add-Content 'gnuplot_script.txt' -NoNewline
    " 'plot_" + (($i-6)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 0.8 lt rgb '#99a2ff'," | Add-Content 'gnuplot_script.txt' -NoNewline
    " 'plot_" + (($i-7)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 0.6 lt rgb '#a8afff'," | Add-Content 'gnuplot_script.txt' -NoNewline
    " 'plot_" + (($i-8)*$rate) + ".txt' u 1:2:3 w p pt 7 ps 0.4 lt rgb '#c4c9ff'" | Add-Content 'gnuplot_script.txt'
    gnuplot .\gnuplot_script.txt
}


