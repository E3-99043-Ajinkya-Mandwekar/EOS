/*rite a shell script to display menu like “1. Date, 2. Cal, 3. Ls, 4. Pwd, 5. Exit” and
execute the commands depending on user choice.*/

!#/bin/bash

echo "Date"
echo "Cal"
echo "Ls"
echo "Pwd"
echo "Exit"

read -p "Enter the choice : "choice
case $choice in
    1)
        date
        ;;
    2)
        cal
        ;;
    3)
        ls
        ;;
    4)
        pwd
        ;;
    5)
        exit
        ;;
    *)
        echo "Invalid choice"
        ;;
esac

