#!/bin/sh

PREFIX=$1
TEXINFO=texinfo-4.8
#MPFR=mpfr-2.3.2
MPFR=mpfr-3.0.0
#GMP=gmp-4.2.4
GMP=gmp-4.3.2
MPC=mpc-0.9
#BINUTILS=binutils-2.18
BINUTILS=binutils-2.21
#GCC=gcc-4.3.2
GCC=gcc-4.6.0
#NEWLIB=newlib-1.17.0
NEWLIB=newlib-1.19.0
HOST=`uname -p`
#TARGETS="arm-elf i386-elf h8300-elf sh-elf"

#LD_LIBRARY_PATH=$PREFIX/lib:$LD_LIBRARY_PATH
LD_RUN_PATH=$PREFIX/lib:$LD_RUN_PATH
export LD_RUN_PATH
PATH=$PREFIX/bin:$PATH

build()
{
    if [ ! -d $1 ]
    then
	echo "$1 not found"
	exit 1
    fi
    echo "build $1"
    pwd
    WORKDIR=$1/.objs.$2
    rm -rf $WORKDIR
    mkdir $WORKDIR
    [ $? -eq 0 ] || exit 1
    cd $1/.objs.$2
    ../configure --prefix=$PREFIX $3 && $MAKE all && $MAKE install
    [ $? -eq 0 ] || exit 1
    cd ../..
}

build_all()
{
    build $BINUTILS $GNUARCH "--target=$GNUARCH"
    build $GCC $GNUARCH "--with-gmp=$PREFIX --with-mpfr=$PREFIX --target=$GNUARCH --enable-languages="c" --with-newlib --with-headers=$PWD/$NEWLIB/newlib/libc/include"
    build $NEWLIB $GNUARCH "--target=$GNUARCH"
}

echo "Host system: $HOST prefix=$PREFIX"

if [ ! -d $PREFIX ]
then
    echo 'no install dir.'
    exit 1
fi

if [ ! -f $PREFIX/bin/makeinfo ]
then
    if [ `uname` = "Linux" ]
    then
	patch -p0 -E < ../patch/texinfo-4.8.linux-patch
    fi
    build $TEXINFO $HOST
fi

if [ ! -f $PREFIX/lib/libgmp.a ]
then
    build $GMP $HOST
fi

if [ ! -f $PREFIX/lib/libmpfr.a ]
then
    build $MPFR $HOST --with-gmp=$PREFIX
fi

if [ ! -f $PREFIX/lib/libmpc.a ]
then
    build $MPC $HOST --with-gmp=$PREFIX
fi

for GNUARCH in $TARGETS
do
    echo "Target system: $GNUARCH"
    build_all
done
