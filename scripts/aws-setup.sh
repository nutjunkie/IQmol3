yum -y install cmake3 
yum -y install gcc
yum -y install gcc-c++
yum -y install gcc-gfortran
yum -y install git
yum -y install libxml2
yum -y install boost-devel
yum -y install openbabel-devel
yum -y install qt5-qtbase
yum -y install qt5-qtbase-devel
yum -y install zlib-devel
yum -y install openssl-devel
yum -y install boost-devel 
yum -y install freeglut-devel

sudo openssl req -x509 -sha384 -newkey rsa:3072 -nodes -keyout /etc/xrdp/key.pem -out /etc/xrdp/cert.pem -days 365
sudo systemctl enable xrdp
sudo systemctl start xrdp

# set QT=/usr/lib64
# replace cmake with cmake3
# sed -i 's/boost::placeholders:://' src/Grid/*

sudo yum install git git-archive-all rpm-build \
                 'perl(Module::Build)' \
                 'perl(IPC::System::zSimple)' \
                 'perl(Path::Class)'
