for fil in `/bin/cat pg.lst | awk '{print $1}'`
do
 echo $fil doing
 rsh $fil "killall $1"
 echo $fil done
done



