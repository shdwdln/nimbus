#!/bin/bash

# Command line arguments
multi=false;
while getopts "hm" option
do
    case $option in
	h | "?" ) 
        echo "SINGLE TARGET MODE:";
        echo "  `basename $0` [<target> [<main cpp>]]";
        echo "  e.g.: `basename $0` water_free_surface_3d main.cpp";
        echo "  Note:";
        echo "    If no <main cpp> is given, it tries to use <target>.cpp or main.cpp.";
        echo "    If no <target> is given, it tries to use the name of the current directory.";
        echo;
        echo "MULTIPLE TARGETS MODE:";
        echo "  `basename $0` -m [<targets...>]";
        echo "  e.g.: `basename $0` -m arraysinfo gridinfo triinfo";
        echo "  Note:";
        echo "    In this mode, build system assumes each target has a corresponding <target>.cpp file.";
        echo "    If no <targets...> are given, it tries to use targets for all cpp files in current directory.";
        exit;;
	m   ) echo "[multi]"; multi=true;;
    esac
done
shift $(($OPTIND - 1))

# Preserve contents above CUT OFF POINT from previous Makefile
if [ -e Makefile ]; then
    grep "^# CUT OFF POINT" Makefile > /dev/null
    if [ $? == 0 ]; then
	mv Makefile /tmp/old_makefile
	cat /tmp/old_makefile | awk 'BEGIN { echo=1 } /^\# CUT OFF POINT/ {echo=0} {if (echo==1) print $0}' > Makefile
    else
	rm -f Makefile
	touch Makefile
    fi
else
    touch Makefile
fi

cat >> Makefile << EOF
# CUT OFF POINT (lines above will be preserved, lines below will be overwritten)
# Set any necessary USE_* variables (e.g. USE_OPENGL=yes) above CUT OFF POINT.
EOF

if $multi; then
    if [ -z "$1" ]; then
	allfiles=`find ./ -mindepth 1 -maxdepth 1 -name \*.cpp -printf "%f "`;
	main_programs=${allfiles//.cpp/};
    else
	main_programs=${*//.cpp/};
    fi

    echo "TARGETS = $main_programs";

    cat >> Makefile << EOF
# GENERATED BY create_makefile.sh -m $main_programs

TARGETS = $main_programs

# This part generated by dependencies.pl [...] | src_list.pl
EOF

    dependencies.pl $main_programs | src_list_nolib.pl >> Makefile

else
    if [ -z "$1" ]; then
	main_program=`basename \`pwd\``;
    else
	main_program=$1;
    fi

    main_cpp="";
    if [ -z "$2" ]; then
	    if [ -e "$main_program.cpp" ]; then
		    main_cpp="$main_program.cpp";
	    elif [ -e "main.cpp" ]; then
		    main_cpp="main.cpp";
	    else
		    echo "Cannot determine main cpp file";
		    exit;
	    fi
    else
	    main_cpp=$2;
    fi

    echo "TARGETS = $main_program, MAIN_SRC = $main_cpp";

    cat >> Makefile << EOF
# GENERATED BY create_makefile.sh $main_program $main_cpp

TARGETS = $main_program
MAIN_SRC = $main_cpp

# This part generated by dependencies.pl $main_cpp | src_list.pl
EOF

    dependencies.pl $main_cpp | src_list_nolib.pl >> Makefile
fi

rel_dir=`relative_directory.pl $PWD $PHYSBAM`

cat >> Makefile << EOF
PUBLIC_LIBRARY_DIR = ${rel_dir}Public_Library
PERSONAL_LIBRARIES_DIR = ${rel_dir}Personal_Libraries

include ${rel_dir}Public_Library/Makefile.common
EOF
