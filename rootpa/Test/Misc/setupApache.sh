#!/bin/bash

# install apache

apt-get install apache2

# install apache perl module

apt-get install libapache2-mod-perl2

# enable support for defining http headers to be returned:

sudo a2enmod headers

# In order to test with https you also need to do some configuration:

# install sll module to apache (assuming mod_ssl is already av

# install certificates

apt-get install ca-certificates

# generate a certificate:

apache2-ssl-certificate

# This should be taken care of when ssl module is take into use. If problems,
# you may need to check and add to /etc/apache2/ports.conf "Listen 443"

# always after adding modules, configuring or even changing script you need to restart 
# apache for example with command

apache2ctl restart

./configureApache.sh
