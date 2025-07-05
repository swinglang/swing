# Installation

Whee can be installed and used in various ways. Here is how to get started:

## Requirements:

- ```gcc```
- ```g++```
- ```libyaml-cpp0.8```
-  ```git```

## (RECOMMENDED) Using WheeUp
### Debian
1. Install WheeUp
```
curl https://raw.githubusercontent.com/WheeLang/wheeup-installer/main/debian.sh | sudo bash
```
2. Install Whee
```
wheeup install stable
```
## From Whee Installer Repository:
Debian/Ubuntu/Mint:
```
git clone https://github.com/WheeLang/whee-installer.git .whee-installer && cd .whee-installer && chmod +x install.sh && chmod +x debian.sh && ./debian.sh && cd - && rm -rf .whee-installer
```
Fedora/RHEL/CentOS:
```
git clone https://github.com/WheeLang/whee-installer.git .whee-installer && cd .whee-installer && chmod +x install.sh && chmod +x fedora.sh && ./fedora.sh && cd - && rm -rf .whee-installer
```
Arch Linux:
```
git clone https://github.com/WheeLang/whee-installer.git .whee-installer && cd .whee-installer && chmod +x install.sh && chmod +x arch.sh && ./arch.sh && cd - && rm -rf .whee-installer
```
No Dependency Installation:
```
git clone https://github.com/WheeLang/whee-installer.git .whee-installer && cd .whee-installer && chmod +x install.sh && ./install.sh && cd - && rm -rf .whee-installer
```
