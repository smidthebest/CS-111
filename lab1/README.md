## UID: 305935705

## Pipe Up

This is a application which mirrors the behavior of pipes and strings the output from multiple commands together. 

## Building

`make`
`./pipe [YOUR COMMANDS HERE]`

## Running

`./pipe ls cat wc`
which returns: `6    6   51` when run in the lab1 directory. 
It is running ls and then ouputting that to cat which in turn is outputting that to wc which is returning 
the word count statistics of ls of the lab1 directory. 

## Cleaning up

Simply run 
`make clean`