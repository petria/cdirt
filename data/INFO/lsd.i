LSD(1)              UNIX Programmer's Manual               LSD(1)



NAME
     lsd - turn off your mind, relax, and float downstream

SYNOPSIS
     lsd [ -dsS ] brandname user ...

DESCRIPTION
     Disturbs the given users according to dosage, set, and setting.
     If more than one user is specified, each will be affected by the
     others, according to the arguments they give to lsd on their
     own terminals or machines.  It is recommended that one of the users
     in a group specify a dose of 0, and that at least one other user be
     experienced in using lsd.

     Brandname is usually a blotter pattern, but may be used to specify
     a non-blotter form, such as "windowpane" or "pyramid."

     There are a number of options, each of which can have a strange and
     not completely predictable effect on the users:


     -d   Dosage.  A dosage of 1 to 3 is recommended for first time
          users, although strength varies with brandname and storage
          conditions.

     -s   Set.  Recommended values are "calm," "happy," and "groovy."
          Sets like "angry," "frustrated," and "bummed" may cause file
          system damage, and should be avoided.  For these sets, other
          utilities are available (see "valium").

     -S   Setting.  Recommended are "familiar," "interesting," and
          "comfortable."  Hostile and challenging settings are reserved
          for experienced users.

FILES
     /etc/trips is a log of system-wide use of lsd since booting.

BUGS
     When invoked with incorrect arguments, lsd will print out a random,
     Unixy-sounding error message and remove this man page.  This policy
     prevents misuse by undergraduates and fundamentalist Christians.

     User state should return to normal in twelve hours or less after
     invoking lsd.  With extreme parameters, however, this cannot always
     be guaranteed.

     The output device is assumed to be 80 columns wide.

 

