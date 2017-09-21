Checking OSDD files {#page_epsg_osdd_check}
============

[TOC]

This page provides information about the EPSG OSDD Check tool.
An OSDD file has to be valid in respect to the OSDD specification and it must
have a valid CRC checksum to be accepted by a design tool.
The EPSG OSDD Check enables users to do the checks for own / customized
OSDD files.


# EPSG OSDD Check {#sect_epsg_osdd_check}

The EPSG OSDD Check can be found at https://www.ethernet-powerlink.org/powerlink/conformity/description-file-check.

To check own / customised files, the following steps need to be carried out:


 - Log in to the EPSG page. A new account can be registered for free, if necessary.
 - Upload the file.
 - After a short time, the results of the check will be sent to the e-mail
   address provided at registration.
   Check spam mail, in case an expected respond takes too long.

 ![EPSG OSDD Check - procedure](web_osdd_checker_procedure.png)
@image latex web_osdd_checker_procedure.png "EPSG OSDD Check - procedure" width=0.75\textwidth

# Limitations {#sect_epsg_osdd_check_limitations}

Nevertheless there are some limitations:

 - Only valid XML files can be processed.
 - The check tool is not able to calculate checksums.

# Securing OSDD files {#sect_secure_osdd}

For development and testing purposes, an OSDD test vendor ID has been
introduced. Using this ID (12:34:56) in the OSDD file may disable the CRC
check of related design tools. This allows to use the file also with invalid
checksum. Tools may indicate the usage of a file with a test vendor ID with
additional measures.

To obtain a CRC secured OSDD with an official vendor ID, the device needs to
pass the openSAFETY Confromance Assessment. A CRC secured version of
the OSDD file is provided by the Conformance Assessment Center
upon successful completion of the assessment.
