#!/bin/bash
echo -n 4 > /sys/class/thermal/cooling_device0/cur_state
sleep 5
echo -n 0 > /sys/class/thermal/cooling_device0/cur_state
declare -a CpuTemps=(65000 60000 50000 40000 20000)
declare -a CoolState=(4 3 2 1 0)
declare -a Percents=(100 70 50 20 0)
DefaultDuty=0
DefaultPercents=0

while true
do
  temp=$(cat /sys/class/thermal/thermal_zone0/temp)
	INDEX=0
	FOUNDTEMP=0
	DUTY=$DefaultDuty
	PERCENT=$DefaultPercents
	
	for i in 0 1 2 3 4; do
		if [ $temp -gt ${CpuTemps[$i]} ]; then
			INDEX=$i
			FOUNDTEMP=1
			break
		fi	
	done
	if [ ${FOUNDTEMP} == 1 ]; then
		DUTY=${CoolState[$i]}
		PERCENT=${Percents[$i]}
	fi

	if [[ $last_dutu -eq '0' ]]; then
		if [[ $DUTY -ne '0' ]]; then 
			echo -n 4 > /sys/class/thermal/cooling_device0/cur_state;
			sleep 1s;
			echo -n $DUTY > /sys/class/thermal/cooling_device0/cur_state;
		else
			echo -n 0 > /sys/class/thermal/cooling_device0/cur_state;
		fi
	else
		echo -n $DUTY > /sys/class/thermal/cooling_device0/cur_state;
	fi

	last_dutu=$DUTY
	echo "temp: $temp, duty: $DUTY, ${PERCENT}%"
	sleep 60s;
done
