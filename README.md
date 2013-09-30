# What's this?
This is a NPAPI Plugin skeleton for Linux.

# Files

    .
    |-- Makefile         - Makefile
    |-- REAME            - this file 
    |-- include          - NPAPI header files 
    |-- main.cpp         - Plugin source
    `-- test_plugin.html - test contents

# Build and Install
## Linux

    $ make
    $ sudo cp libtestplugin.so /usr/lib/mozilla/plugins/

Exec browser and access to "about:plugins".
"Test Plug-in" is displyed, plugin install is success.

# Test plugin

    $ xdg-open ./test_plugin.html

