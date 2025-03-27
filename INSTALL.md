## Installation

Whee can be installed and used in various ways. Here is how to get started:

### Requirements:

```glibc``` (on Linux and macOS) or ```mingw-w64``` (on Windows)
    
```git```

### From Whee Installer Repository:
1. **Git Clone the Whee Installer Repository**
```
git clone https://github.com/NarpLang/whee-installer.git
cd whee-installer
```
2. **Run the Installer**
```
sudo ./install.sh
```
3. **Progress through the installation with Bitzy**
```
🐶 Hi! I'm Bitzy. I will walk you through the installation! But first, are you sure you want to install Whee? (Y/N)
Requirments:
- glibc (for Linux & macOS) or mingw-w64 (for Windows)
> Y
🐶 Ok, let's install Whee!

🐕[==========] Downloading NarpLang/exotic.git
🐕[==========] Downloading NarpLang/whee.git
🐕[==========] Downloading NarpLang/whee-mng.git
🐕[==========] Downloading Leon8326/linguist.git

🐕Unpacking exotic...
🐕Unpacking libexotic-main...
🐕Unpacking libexoticc...
🐕Unpacking whee...
🐕Unpacking whee-to-c...
🐕Unpacking ewc...
🐕Unpacking whee-extra...
🐕Unpacking wheecfg...
🐕Unpacking github-linguist...

🐕Installing exotic...
🐕Installing whee...
🐕Installing whee-extra...
🐕Installing github-linguist...

🐶 All done! Now, Whee is intalled, try it using the 'whee' command.
```
### From wheepy
WheePython (wheepy) alows you to run Whee inside Python.

**Requires:**
```Python 3.x```
1. **Create a python venv (Optional, but recomended)**
```
python3 -m venv wheepy-venv
```
2. **Install wheepy**
```
pip install wheepy
```
