# Filename: .cshrc
# Description: Sources in on the class MASTER version for settings information
# 
# Please (DO NOT) edit this file unless you are sure of what you are doing.
# This file and other dotfiles have been written to work with each other.
# Any change that you are not sure off can break things in an unpredicatable
# ways.

# Set the Class MASTER variable and source the class master version of .cshrc
if !($?MASTER) then
	setenv MASTER \
		`echo $USER | sed '/[0-9][a-z][a-z]$/s/..$//' | sed 's/[_-]..$//'`
endif
if (-e ~$MASTER/adm/class.cshrc) source ~$MASTER/adm/class.cshrc
