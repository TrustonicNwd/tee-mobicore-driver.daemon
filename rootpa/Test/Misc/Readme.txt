This folder contains material that can be used for RootPA testing/debugging, 
such as httpd.conf and content of /var/www for using apache2 server in simple SE stub.

There are three helper scripts for you to use. 

setupApache.sh installs apache and the required modules. It also contains some information for manual configuration. It needs to be executed in your machine only once.

configureApache.sh copies all the needed files from Mish folder to /var/www and /etc/apache2 to let you use the latest scrips

getLatest.sh copies the files back to9 this folder, in case you have modified them directly where they are executed in and would like to commit your chnages.

See the content of the scripts for more details.

This has been verified originally in ubuntu 12.04 and after modifications in ubuntu 12.10. They behave slightly differently (apache2 does not use /etc/apache2/httpd.conf by default in 12.10. 