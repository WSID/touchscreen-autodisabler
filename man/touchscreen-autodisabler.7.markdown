# touchscreen-autodisabler(7) - Automatically enables and disables touchscreen.

## SYNOPSIS

touchscreen-autodisabler  
touchscreen-autodisabler {-h, -v}

systemctl --user {start, stop, ...} touchscreen-autodisabler[.service]

## DESCRIPTION

Automatically disables touchscreen by tablet stylus and eraser's proximity
states.

### HOW IT RUNS.

This application gets events from X Server for the tablet. When pen stylus
come to tablet, X Server sent event that stylus's state has been changed.
If such is detected, this will grab touchscreen so that touch event would
not be sent to any other X Clients.

When pen gets out, this will ungrab touchscreen, so it can back to work.

### SINGLE INSTANCE

This is intended to be a **single instance** service-like application. When
this application is running, it acquires name **"wsid.Tad"** on DBus. Also,
it exposes [methonds and properties on there][DBUS INTERFACE].

This application comes with a systemd user service file, systemctl(1) can
start and stop it, and even automatically run at the start.

        systemctl --user start touchscreen-autodisabler
        # Starts touchscreen autodisabler

        systemctl --user enable touchscreen-autodisabler
        # Make systemd automatically start touchscreen autodisabler

## OPTIONS

There are nothing much about commandline options, as many options have been
replaced by GSettings and DBus.

 * -h, --help:
     Prints brief help for touchscreen-autodisabler. Nothing much on there
     though... :(

 * -v, --version:
     Prins version of touchscreen-autodisabler.

## GSETTINGS

This application relies on GSettings to store options. You may edit settings
by gsettings(1) or dconf-editor(1). For device names, both of xinput id and
actual name are acceptable. When device list is empty, this will try to get all
of appropriate devices

### SCHEMA wsid.Tad @ /wsid/Tad

 * watch: **as**:
    Devices to be watched.
    Tablet stylus and eraser devices to notify proximity state change.

 * control: **as**:
    Devices to be controlled.
    Touchscreen devices to be enabled and disabled.

 * transition: **s** = {"default", "clear-to-enable"}:
    State transition mode.
    Determines how internal state transit, and enable/disables devices.

    "default" : Simple state transition. Disable on come, Enable on gone.

    "clear-to-enable": Delay touchscreen enabling, until touching is cleared.
    Hand may remain on the touchscreen after pen is removed. This mode will
    prevent unintended touch event in such case.

## DBUS INTERFACE

This application exposes its state and methods through DBus.

### INTERFACE wsid.Tad @ OBJECT /wsid/Tad

 * _property_ State: **u** (read):
   State of this application. Each value means.  
   0: Neutural state.  
   1: Pen state.  
   2: Rest Pen state. (Hand is resting on the touchscreen.)  
   3: Rest Still state. (Pen is out but hand is still resting.)  
   4: Touching state. (Touchscreen is in use)

 * _property_ ControlEnabled: **b** (read):
   Controlled devices are enabled or not.

 * _method_ ResetState (): () => ():
   Resets its state into neutural state.

 * _method_ Quit (): () => ():
   Quits touchscreen autodisabler

 * _signal_ Transit: => (u, u, b):
               stimulus: u: A Event.  
               state: u: New state.  
               control-enabled: b: Whether controlled devices should be enabled.  
    Emitted when its state is changed.
       
## SEE ALSO

xinput (1)

## COPYRIGHT

Copyright 2014-2016, WSID

## AUTHOR

WSID <jongsome@gmail.com> <http://wsidre.egloos.com>
