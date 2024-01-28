#!/usr/bin/bash

# Выводит все числа от 1 до введенного числа. Если число отрицательное или ноль, то выводится сообщение об ошибке

end=$1

if [ $end -le 0 ];
then
	echo "Upper bound must be positive"
else
	start=1
	while [ $start -le $end ]
	do
		echo $start
		(( start++ ))
	done
fi
