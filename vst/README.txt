To test the VST plugin on linux,
- install wine staging, yabridge, and yabridgectl
- configure your path with yabridgectl add "$HOME/.wine/drive_c/Program Files/Common Files/VST3"
- Move your cross compiled windows vst3 plugins to that directory
- Run yabridgectl sync
- Run yabridgectl status to check status
- Then you're able to test the plugins with your linux daw of choice
- Note that you may need winetricks dxvk if your ui isn't repainting when slider values change
- Also note that your wine version may have a "top-left" offset issue where mouse clicks aren't
  at their correct coordinates. This was the case for me and I needed to click "emulate a virtual
  desktop" in winecfg for the coordinates to be correct.
