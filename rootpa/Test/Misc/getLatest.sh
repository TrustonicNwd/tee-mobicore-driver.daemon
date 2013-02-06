#!/bin/bash


cd `dirname $0`

pwd

cp -rf /var/www/* .

if [ -f /etc/apache2/conf.d/httpd.local.conf ]
then
    cp -f /etc/apache2/conf.d/httpd.local.conf ./httpd.conf
else
    cp -f /etc/apache2/httpd.conf .
fi

ls
