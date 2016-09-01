# https://www.raspberrypi.org/forums/viewtopic.php?f=33&t=84696

while true
do
    echo -----------------------------------------
    for gpiopin in 17 18
    do
        FILE=/sys/class/gpio/gpio$gpiopin
        if [ ! -e $FILE ];
        then
            echo "File $FILE does not exist."
        fi

        cat $FILE/value
    done

    sleep 0.5
done
