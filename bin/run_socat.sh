#!/bin/bash
while [ 1 ]
do
  socat -x -x UDP:localhost:6449 /dev/cu.SparkFun-BT-SerialPort,clocal=1,nonblock=1,cread=1,cs8,ixon=1,ixoff=1
  sleep 1
done

