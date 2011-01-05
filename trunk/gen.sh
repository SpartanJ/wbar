#!/bin/sh

set -e

red()
{
    echo "\033[0;31m$1\033[0m"
}

green()
{
    echo "\033[0;32m$1\033[0m"
}

blue()
{
    echo "\033[0;34m$1\033[0m"
}

function_init()
{
    function_clear
    touch AUTHORS NEWS README THANKS TODO ChangeLog
    mkdir -p m4

    if [ ! -e /usr/bin/autoreconf ] ; then
        red "/usr/bin/autoreconf not found. Install autoconf"
        exit 0
    fi

    green "Execute autoreconf"
    autoreconf --install 3>/dev/null 2>/dev/null 1>/dev/null
    autoreconf --force 3>/dev/null 2>/dev/null 1>/dev/null

    if [ ! -z "$header" ] ; then

        if [ ! -e /usr/bin/autoheader ] ; then
            red "/usr/bin/autoheader not found. Install autoconf"
            exit 0
        fi
        
        green "Execute autoheader"
        autoheader 1>/dev/null

    fi

    if [ ! -z "$libtool" ] ; then

        if [ ! -e /usr/bin/libtoolize ] ; then
            red "/usr/bin/libtoolize not found. Install libtool"
            exit 0
        fi

        green "Execute libtoolize"
        libtoolize --force --copy --automake 1>/dev/null

    fi

    if [ ! -z "$gettext" ] ; then

        if [ ! -e /usr/bin/glib-gettextize ] ; then
            red "/usr/bin/glib-gettextize not found. Install libglib2.0-dev"
            exit 0
        fi

        green "Execute glib-gettextize"
        glib-gettextize --copy --force 1>/dev/null

    fi

    if [ ! -z "$intltool" ] ; then

        if [ ! -e /usr/bin/intltoolize ] ; then
            red "/usr/bin/intltoolize not found. Install intltool"
            exit 0
        fi

        green "Execute intltoolize"
        intltoolize --copy --force --automake 1>/dev/null

    fi

    if [ ! -e /usr/bin/autoconf ] ; then
        red "/usr/bin/autoconf not found. Install autoconf"
        exit 0
    fi

    green "Execute autoconf"
    autoconf 1>/dev/null

    if [ ! -e /usr/bin/automake ] ; then
        red "/usr/bin/automake not found. Install automake"
        exit 0
    fi

    green "Execute automake"
    automake -a --gnu -c --foreign 1>/dev/null
}

function_i18n()
{
    if [ -d debian/po ]; then

        if [ ! -e /usr/bin/debconf-updatepo ] ; then
            red "/usr/bin/debconf-updatepo not found. Install po-debconf"
            exit 0
        fi

        cd debian
        green "Execute debconf-updatepo"
        debconf-updatepo 1>/dev/null
        cd ..

    fi

    if [ -d debian/po4a ] && [ -e debian/po4a/config ] ; then

        if [ ! -e /usr/bin/po4a ] ; then
            red "/usr/bin/po4a not found. Install po4a"
            exit 0
        fi

        green "Execute po4a"

        po4a -k 100 \
             -M "UTF-8" \
             --package-name $package\
             --package-version $version \
             --msgid-bugs-address "$DEBFULLNAME <$DEBEMAIL>"\
             --rm-backups \
             debian/po4a/config | perl -ne 'print if /\%/'

    fi

    if [ -d po ] && [ ! -z "$intltool" ] ; then

        if [ ! -e /usr/bin/intltool-update ] ; then
            red "/usr/bin/intltool-update not found. Install intltool"
            exit 0
        fi

        cd po
        green "Execute intltool-update"
        resp=$(intltool-update --maintain 1>/dev/null 2>&1)
        cd ..

        if [ ! -z "$resp" ]; then
            green "Required ./configure"
            only_configure
            cd po
            green "Execute intltool-update again"
            intltool-update --maintain 1>/dev/null
            cd ..
        fi
    fi
}

only_configure()
{
    if [ ! -e configure ]; then
        red "Script configure does not exist."
        exit 0
    fi

    green "Execute ./configure"

    if [ ! -z "$libtool" ] ; then
        ./configure --prefix=/usr --exec_prefix=/usr --sysconfdir=/etc --enable-static=no 1>/dev/null
    else
        ./configure --prefix=/usr --exec_prefix=/usr --sysconfdir=/etc 1>/dev/null
    fi
}

function_config()
{
    only_configure

    if [ -d po ] && [ -e po/Makefile ] && [ ! -z "$intltool" ] ; then
        cd po
        green "Execute make update-po"
        make update-po | perl -ne 'print if /:$/'
        cd ..
    fi
}

function_debian()
{
    if [ ! -d debian ] || [ ! -e debian/changelog ]; then
        red "debian/changelog not found."
        exit 0
    fi

    if [ ! -d debian ] || [ ! -e debian/control ]; then
        red "debian/control not found."
        exit 0
    fi

    if [ ! -x /usr/bin/pdebuild ] ; then
        red "/usr/bin/pdebuild not found."
        exit 0
    fi

    if [ ! -f Makefile ] ; then
        only_configure
        [ -f Makefile ] || return
    fi

    debpackage=$(head -1 debian/changelog | perl -ne 'print $1 if /^([a-z0-9\-]+)\s+/')
    debversion=$(head -1 debian/changelog | perl -ne 'print $1 if /\s*\(\s*([0-9\.]+)/')
    debrevision=$(head -1 debian/changelog | perl -ne 'print $1 if /-([0-9\.]+)\s*\)/')

    blue "Distributed $package"_"$version-$debrevision"

    if [ "$package" != "$debpackage" ] || [ "$version" != "$debversion" ] || [ -z "$debrevision" ] ; then
        red "Problem name or version in debian/changelog file"
        exit 0
    fi

    debdist=$(head -1 debian/changelog | perl -ne 'print $1 if /.*\)\s*([a-z]+)\s*;/')
    debarch=$(grep -s -m 1 '^Architecture:' debian/control | perl -ne 's/Architecture:\s*//;print')

    dir=$PWD

    rm -r -f $package*.tar.gz $desdir/$package* 1>/dev/null
    make dist 1>/dev/null

    mv $package-$version.tar.gz $desdir/$package"_"$version.orig.tar.gz
    tar xf $desdir/$package"_"$version.orig.tar.gz -C $desdir
    cp -r debian/ $desdir/$package-$version/ 1>/dev/null

    cd $desdir/$package-$version 1>/dev/null

    if [ -d debian ] ; then
        rm -r -f `find debian -iname '*' | perl -ne 'print if /~/ || /\/\./'` 2>/dev/null 1>/dev/null
    fi

    if [ ! -e $HOME/.pbuilderrc ]; then
        red "$HOME/.pbuilderrc not found."
        exit 0
    fi

    if [ ! -x /usr/bin/pdebuild ] ; then
        red "/usr/bin/pdebuild not found. Install pbuilder"
        exit 0
    fi

    pbuilderbasedir=$(grep -s '^BASE_DIR' "$HOME/.pbuilderrc" | perl -ne 's/.*=//;print')

    green "Execute pdebuild"

    pdebuild -- --basetgz "$pbuilderbasedir/$debdist-base-$debarch.tgz" \
                --aptcache "$pbuilderbasedir/$debdist-base-$debarch/aptcache/" \
                --architecture "$debarch" \
                --distribution "$debdist" \
             | perl -ne 'if (/^W:/ || /^E:/ || /error/i || /warning/i) {print "\033[0;31m$_\033[0m" ;} else {print}' || return

    cd $dir

    repository=$(grep -s '^REPOSITORY' "$HOME/.pbuilderrc" | perl -ne 's/.*=//;print')
    packagenames=$(cat debian/control | perl -ne 'print "$1 " if /^Source:\s*([a-z0-9\-]+)/;print "$1 " if /^Package:\s*([a-z0-9\-]+)/')

    if [ ! -x /usr/bin/lintian ] ; then
        red "/usr/bin/lintian not found. Install lintian"
    else
        green "Execute lintian"
        lintian $repository/$package"_"$version-$debrevision"_"$debarch.changes | perl -ne 'print if /^E:/; print if /^W:/' || true
    fi

    if [ ! -x /usr/bin/reprepro ] ; then
        red "/usr/bin/reprepro not found. Install reprepro"
        exit 0
    fi

    green "Execute reprepro"

    reprepro -Vb $repository remove $debdist $packagenames 2>/dev/null 1>/dev/null || true
    reprepro -Vb $repository --delete include $debdist $repository/$package"_"$version-$debrevision"_"$debarch.changes 1>/dev/null || true
}

function_check()
{
    [ -f Makefile ] || function_configure

    green "Execute make check"

    make check 1>/dev/null
}

function_clear()
{
    rm -r -f $package*.tar.gz $desdir/$package* 1>/dev/null

    if [ -d debian ]; then
       rm -f `find debian -iname '*' | perl -ne 'print if (/~/ || /\/\./) && ! /\/\.svn/'` 2>/dev/null 1>/dev/null
    fi

    if [ -f Makefile ]; then
        make clean 3>/dev/null 2>/dev/null 1>/dev/null || true
        make distclean 3>/dev/null 2>/dev/null 1>/dev/null || true
    fi

    rm -r -f autom4te.cache aclocal.m4 config.guess config.log config.status config.sub configure depcomp install-sh libtool ltmain.sh missing stamp-h1 intltool-extract.in intltool-merge.in intltool-update.in

}

function_start()
{

    if [ -e configure.ac ]; then
	configfile="configure.ac"
    elif [ -e configure.in ]; then
	configfile="configure.in"
    else
	red "Config file not found"
	exit 0
    fi

    package=$(cat $configfile | perl -ne 'print $1 if /AC_INIT\s*\(\s*\[*\s*([a-z0-9\-]+)\s*\[*/') || true
    version=$(cat $configfile | perl -ne 'if ( s/AC_INIT\s*\(\s*\[*\s*([a-z0-9\-]+)\s*\[*//) { print $1 if /([0-9\.]+)/ }') || true
    header=$(grep -s 'AM_CONFIG_HEADER' configure.ac) || true
    intltool=$(grep -s 'AC_PROG_INTLTOOL' configure.ac) || true
    gettext=$(grep -s 'AM_GLIB_GNU_GETTEXT' configure.ac) || true
    libtool=$(grep -s 'AC_PROG_LIBTOOL' configure.ac) || true
    desdir="/tmp"

    if [ -z "$package" ] || [ -z "$version" ] ; then
        return
    fi

    blue "Package $package-$version"

    for i in $@ ; do

    if [ "$i" = "init" ] ; then 
        function_init
    elif [ "$i" = "i18n" ] ; then
        function_i18n
    elif [ "$i" = "config" ] ; then
        function_config
    elif [ "$i" = "all" ] ; then
        function_init
        function_i18n
        function_config
    elif [ "$i" = "check" ] ; then
        function_check
    elif [ "$i" = "debian" ] ; then
        function_debian
    elif [ "$i" = "clear" ] || [ "$i" = "clean" ] ; then
        function_clear
    fi

    done 

}

function_start $@
green "Finished"
echo ""

exit 0

