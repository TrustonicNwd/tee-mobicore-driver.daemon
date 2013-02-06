#!/bin/bash

echo "This script has to be run in Misc folder with root permissions"

cp -rf cgi-bin /var/www
cp -f index.html /var/www
cp -f httpd.conf /etc/apache2/

if [ -d /etc/apache2/conf.d ]
then
    cp -f httpd.conf /etc/apache2/conf.d/httpd.local.conf
else
    cp -f httpd.conf /etc/apache2/
fi

/etc/init.d/apache2 restart

echo "apache2 is now ready to emulate SE"
