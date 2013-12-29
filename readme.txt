Orbiter 2010 Readme
===================

1. Orbiter distribution formats
-------------------------------
Orbiter 2010 is distributed in two formats:

- Microsoft Installer package: Orbiter100603.msi
- Zip file archive Orbiter100603.zip

The contents of both packages are identical. Users can choose
whichever package format they prefer.

The MSI installer package provides a convenient way to install
Orbiter 2010. It comes with a graphical interface that allows to
select the installation location, component selection, destop
icon, and start menu item. It also provides a removal function.
This method is particularly useful for new users.

The zip-file archive package must be extracted manually into a
target directory, using an extraction utility such as winzip or
7-zip. The directory structure of the zip archive must be
preserved when extracting to the target folder.
This method is self-contained and portable. It does not
modify the registry, and does not add or change files outside the
installation folder. Multiple installations are possible.
Deinstallation only requires deletion of the Orbiter folder and
all subfolders.


2. Runtime libraries
--------------------
Note: Orbiter requires the VC2005 runtime libraries installed
on the computer. On most systems, these libraries will already
be installed. The MSI installer package will install these
libraries automatically if required (unless disabled by the user
during a custom installation). The zip file package does not
install these libraries by default. If Orbiter fails to launch
with an error message similar to

"It wasn't possible to start this application because the
configuration of the application is incorrect. The reinstallation
of the application might correct this problem."

then the runtime libraries may be missing. You can install them
by executing the vcredist_x86.exe utility in the vcredist folder.