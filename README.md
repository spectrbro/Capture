# Capture

A MVP basic application used to capture Windows Mouse events using RawInput and Hooks for keyboard, 
to send to a Mac where syntetic events are created to control the Mac from Windows

# Approach

The current approach relies on RawInput for capturing mouse movement,
with an additional hook on top installed to swallow events for when you want to send events to Mac. 
This will make Windows unresponsive and forward events over UDP. The hotkey per now is hardcoded to PageUp.

The keyboard relies entirely on a hook to capture, send and swallow events.

The reason I went for hooks in both cases is because it was the only way I managed to swallow events.
Combining RawInput and hooks for keyboard did not work either, since the hook made it so no 
events actually got captured when I wanted to swallow the events and rely entirely on RawInput for
captruing events.

# Issues

- Hard coded hotkey
- Hard coded IP
- No encryption of data sent from one machine to the other
- MacOS relies on CGEventCreateKeyboardEvent which is at the Quartz level and not native driver level. So when you let the application take control of the Mac
  and you use shortcuts for Mission Control (Ctrl+Left/Right/Up), modifier keys tend to get stuck. I am currently looking to move over to IOHID framework, but I am not sure how
  open the API is.


# How to build

Development is done on M1 Mac with mingw64 compiler installed through brew. A mingw toolchain is included based upon that.

```sh
# Builds Mac-Listen
cmake --preset debug && cmake --build --preset debug
cmake --preset release && cmake --build --preset release
```

```sh
#Builds Windows-Capture
cmake --preset windows-debug && cmake --build --preset windows-debug
```