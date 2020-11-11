# difftime - Print out the time difference from two files
`difftime` is a simple utility to calculate and print out the delta time from
two files or a file and now. It's usefull to make a gate in a script, based on
time.

## Install
Just compile and install
```
make
sudo make install
```

## How it works
With no arguments, it fail with exit code not zero.

The first argument must be a file. With no second argument, it print out the
time differnce between the last modification time of the file and now. 
If the file doesn't exists it fail with exit code not zero.
The output is in seconds by default.

```
difftime -P -A /var/spool/gate
2345
```
 
Passing the second argument, the difference is between the modification time of
the second and the first.

```
difftime -P /var/spool/older /var/spool/newer
-1234
difftime -P /var/spool/newer /var/spool/older
1234
```

## Options
You can customize the output and choose which time to compare.
The output options are in upper case, the time options lower.

### Output
The default output is in seconds. Other options are:
- `-M` in minutes
- `-H` in hours
- `-D` in days

It returns `0` if the amount of seconds cannot reach a single unit of the
selected time.

By default `difftime` print out the number without the `\n` character at the
end, so you can use it in a variable:
```
DT=$(difftime -A /var/spool/gate) 
[ $DT -gt 120 ] && echo "too old"
```
If you need to get the result to print out, use the `-P` option, that add the
newline character at the end.
```
difftime -P -A /var/spool/gate
12345
```

By default the output is signed (`-` as prefix if negative). If you want an absolute
value, use the `-A` option.
```
difftime -P -A /var/spool/older /var/spool/newer
1234
```

### Time Field
By default `difftime` compare the modification date of the files.
You can change that with the time options:
- `-a` access time
- `-m` modification time
- `-c` change time

Each option is valid for the corresponding file. For example you can check the
access time of the first file and the change time of the second.
```
difftime -P -D -A -a /var/spool/base -c /var/spool/gate
3
```

### Help and Version
You can access to the *help* and show the *version* with these options:
- `-h` for the help
- `-v` for the version

## Show the file times
Displaying the times of the file is not the purpose of `difftime`. You can
check them with the standard UNIX command `stat`.

## Examples

### Ensure a command doesn't execute before a certain time
```
#!/bin/bash

# Very simple time gate

SPOOL_FILE="spool"
# time in seconds
TIME_GATE=10

DT=$(./difftime -A $SPOOL_FILE 2> /dev/null ) || DT=$TIME_GATE

[ $DT -lt $TIME_GATE ] && exit 1

echo "Do the stuff"

# reset the spool time
touch $SPOOL_FILE
```
