# https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=84696

for gpiopin in 11 12 17 18
do
	FILE=/sys/class/gpio/gpio$gpiopin
	if [ ! -e $FILE ];
	then
   		echo "File $FILE does not exist."
   		echo "echo $gpiopin > /sys/class/gpio/export"
   		echo $gpiopin > /sys/class/gpio/export
	fi

   	echo "echo in > $FILE/direction"
   	echo in > $FILE/direction

   	echo "echo rising > $FILE/edge"
   	echo rising > $FILE/edge

   	echo "Configured GPIO: $FILE"
done

