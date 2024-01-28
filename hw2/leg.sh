#!/usr/bin/bash

# Проверяет и выводит соответствующее сообщение о том, положительное число, отрицательное или ноль

var=$1

if [ $var -lt 0 ]; then
	echo "Value < 0"
elif [ $var -eq 0 ]; then
	echo "Value == 0"
elif [ $var -gt 0 ]; then
	echo "Value > 0"
fi
