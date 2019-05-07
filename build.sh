#!/bin/bash
set -e
. versions

VERSION=$1
if [ -z "$VERSION" -o $# -ne 1 ]; then
  printf "Usage: build.sh version\n" >&2
  exit 1
fi

HTTPD=/usr/local/httpd-lg
HTTPD_WS=$WORKSPACE/httpd$HTTPD

SVN_BASE="file:///repo"
svn co --quiet "$SVN_BASE/httpd/$HTTPD_VERSION" httpd

# Update the apr and apu configuration scripts to reference the workspace.
sed -i.bak "s|$HTTPD|$HTTPD_WS|" $HTTPD_WS/bin/apr-1-config
rm $HTTPD_WS/bin/apr-1-config.bak
sed -i.bak "s|$HTTPD|$HTTPD_WS|" $HTTPD_WS/bin/apu-1-config
rm $HTTPD_WS/bin/apu-1-config.bak

# The libaprutil dependency on libapr here needs to point to the workspace.
sed -i.bak "s|$HTTPD/lib/libapr-1.la|$HTTPD_WS/lib/libapr-1.la|" $HTTPD_WS/lib/libaprutil-1.la
rm $HTTPD_WS/lib/libaprutil-1.la.bak

# Create an updated apxs configuration that references the workspace.
# The prefix should point to the intended install location (when libs are
# installed on the system)
sed -e "s|$HTTPD|$HTTPD_WS|" \
 -e "s|prefix = $HTTPD_WS|prefix = $HTTPD|" \
 $HTTPD_WS/build/config_vars.mk >config_vars.mk
printf "libdir = %s/modules\n" "$HTTPD_WS" >>config_vars.mk

# Create an updated apxs itself to read the modified configuration.
sed -e "s|$HTTPD/build|$WORKSPACE/|" $HTTPD_WS/bin/apxs >apxs
chmod +x apxs

mkdir -p empty
./autogen.sh
./configure \
  --prefix="$HTTPD" \
  --libdir="$HTTPD" \
  --disable-mlogc \
  --with-apr="$HTTPD_WS" \
  --with-apu="$HTTPD_WS" \
  --with-apxs="$WORKSPACE/apxs" \
  --with-curl="$PWD/empty" \
  --enable-collection-global-lock

make
export DESTDIR=$WORKSPACE/$VERSION
make install

DESTDIR_HTTPD="$DESTDIR$HTTPD"
mkdir -p $DESTDIR_HTTPD/conf/mod_security_conf
cp -p modsecurity.conf-recommended $DESTDIR_HTTPD/conf/mod_security_conf/modsecurity.conf
cp -p unicode.mapping $DESTDIR_HTTPD/conf/mod_security_conf/
mkdir $DESTDIR_HTTPD/include
cp -p apache2/*.h $DESTDIR_HTTPD/include
