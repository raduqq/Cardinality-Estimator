#!/bin/bash

total='0'
CODING_STYLE_BONUS=0
MAX_BONUS=20

valgrind_test() {
    (time timeout 50 valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 -q ./$EXEC < $test_file) &>/dev/null
    if [ "$?" != "0" ]; then
        echo -n "DETECTED MEMORY LEAKS - "
        return 1
    else
        return 0
    fi
}

valgrind_test_hll() {
    (time timeout 50 valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 -q ./$EXEC $test_file) &>/dev/null
    if [ "$?" != "0" ]; then
        echo -n "DETECTED MEMORY LEAKS - "
        return 1
    else
        return 0
    fi
}

freq() {
    freq_points=(4 4 4 4 4 5)

    echo "Testing Frequency Vector"

    for i in `seq 0 5`; do
        echo -n "$i. "
        test_file="tests_freq/test${i}.txt"
        ref_file="tests_freq/test${i}.ref"

        EXEC=freq
        ./$EXEC < "$test_file" | sort -nk 1 | diff - "$ref_file" > /dev/null
        TEST_OK=$?
        valgrind_test 
        VALGRIND_OK=$?
        [[ $TEST_OK -eq 0 && $VALGRIND_OK -eq 0 ]] \
            && echo "passed .......... ${freq_points[i]} points" \
            && total="$total+${freq_points[i]}" \
            || echo "failed";

    done
    echo ""
}

hsh() {
    hash_points=(3 4 5 6 7)

    echo "Testing Hashtable" 

    for i in `seq 0 4`; do
        echo -n "$i. "
        test_file="tests_hash/test${i}.txt"
        ref_file="tests_hash/test${i}.ref"

        EXEC=hash
        ./$EXEC < "$test_file" | sort -k 1 | diff - "$ref_file" > /dev/null
        TEST_OK=$?
        valgrind_test 
        VALGRIND_OK=$?
        [[ $TEST_OK -eq 0 && $VALGRIND_OK -eq 0 ]] \
            && echo "passed .......... ${hash_points[i]} points" \
            && total="$total+${hash_points[i]}" \
            || echo "failed";

    done

    echo ""
}

hll() {
    hll_ref_results=(10000 15000 20000 25000 25000)
    hll_ref_tolerances=(1000 1500 2000 2500 2500)
    hll_points=(4 5 6 7 8)

    echo "Testing HyperLogLog"

    for i in ${!hll_ref_results[@]}; do
        echo -n "$i. "
        test_file="tests_hll/test${i}.txt"

        EXEC=hll
        result=`./hll "$test_file"`

        difference=$(( ${hll_ref_results[i]} - $result ))
        abs_diff=${difference#-}

        points=`echo "(${abs_diff} < ${hll_ref_tolerances[i]}) * ${hll_points[i]}" | bc`
        valgrind_test_hll 
        VALGRIND_OK=$?
        [[ $points -gt 0 && $VALGRIND_OK -eq 0 ]] \
            && echo "passed .......... $points points" \
            || echo "failed";
        total="$total+$points"
    done

    echo ""
}

function checkBonus {
    printf '%*s\n' "${COLUMNS:-$(($(tput cols) - $ONE))}" '' | tr ' ' -
    echo "" > checkstyle.txt

    echo -ne "Coding style Errors:\n"
    for entry in *.{c,h}; do
    echo $entry
        if [[ -f $entry ]]; then
            ./cpplint.py "$entry" > checkstyle.txt
            YOUR_BONUS=`cat checkstyle.txt | grep "Total errors found: "| tr -dc '0-9'`
            ERRORS_NUMBER=$(( $ERRORS_NUMBER+$YOUR_BONUS ))
        fi
    done

    if [[ $ERRORS_NUMBER != "0" ]]; then
        printf '\nBonus: %*sFAIL' "${COLUMNS:-$(($(tput cols) - $MESSAGE_SIZE))}" '' | tr ' ' -
        echo -ne "\n\t\tYou have a total of $ERRORS_NUMBER coding style errors.\n"

    else
        echo -ne "\n\tNone?\n\t\tWow, next level!"
        printf '\n\nBonus: %*sOK' "${COLUMNS:-$(($(tput cols) - $MESSAGE_SIZE + $TWO ))}" '' | tr ' ' -

        CODING_STYLE_BONUS=$(($total * $MAX_BONUS / 80))
        CODING_STYLE_BONUS=$(($CODING_STYLE_BONUS))
    fi
    rm -f checkstyle.txt
}

function printBonus {
    if [[ $ERRORS_NUMBER -gt 0 ]]; then
    printf '\n%*s' "${COLUMNS:-$(tput cols)}" '' | tr ' ' -
        echo -ne  "\n\tAnd you almost did it!\n"
    else
        echo -ne "\n\tDamn!\n"
    fi
    echo -ne "\n\t\tYou got a bonus of $CODING_STYLE_BONUS/$MAX_BONUS.\n\n"
}

make && (echo ""; freq; hsh; hll; echo "total = $(echo $total | bc)/80"; checkBonus; printBonus; make clean &> /dev/null)
