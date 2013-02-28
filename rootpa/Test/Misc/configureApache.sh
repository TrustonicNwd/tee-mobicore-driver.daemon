#!/bin/bash
echo "This script has to be run with root permissions"
cd $(dirname $0)

cp -rf cgi-bin /var/www
cp -f index.html /var/www

if [[ -d /etc/apache2/conf.d ]]
then
    cp -f httpd.conf /etc/apache2/conf.d/httpd.local.conf
else
    cp -f httpd.conf /etc/apache2/
fi

/etc/init.d/apache2 restart

echo "apache2 is now ready to emulate SE"
