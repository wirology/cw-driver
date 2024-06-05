<h1>Unix Command Line CW Keyer Mini Manual.</h1>
<br><br>
License: GPL
<br><br>
<pre>
With C++ dev environment, type "make release" to build tools, which are
  #1 a "driver" binary for morse code from keyboard input
  #2 a "ascii2pcm" binary for morse code from stdin.
Without C++ dev, the provided binaries may work.
</pre>
<br>
To send training texts, use shell scripts in "training" subfolder.
<br><br>
Application is based on Randall S. Bohn's morse code generator tool cwtext-0.96 generating 8bit pcm from ascii text.<br>
https://sourceforge.net/projects/cwtext/files/cwtext/cwtext%200.96/
Some minor changes to the original tool ('+' added, '=' fixed, evelope slopes adapted to high speed):<br>
generated from "diff -ruN backup/cwtext-0.96/cwtext-0.96 cwtext-0.96 > cwtext-0.96.patch"<br>
original files can be patched with "cd backup/cwtext-0.96/; patch -p0 < ../../cwtext-0.96.patch"<br>
<br>
To run on ubuntu, aside of gcc C++ development, I needed the following two repositories for SoX:<br>
apt-get install sox libsox-fmt-all<br>
<br>
Initially I had a shell echo script piping texts to the tool. This, however, didn't fully work with function key and special
command input extensions. There seemed to be race conditions.
<br>
As a solution I coded a simple driver shell replacement in C++ 11. To build, type
make release
<br><br>
remove debug info:
strip driver
<br><br>
For CW operation a transceiver bridge is recommended. For me, the minimalist circuit in
circuits/simple_bridge_with_transformer.png did the trick without need for USB or power supply, with speeds
of up to 60wpm. All bridge parts were taken from an abandoned TV's main board and the little transformer
I found worked best with high pitch 3000Hz sidetone. For more agreeable values, simply put your preferences
in run_cwtext_from_driver.sh. Replace sidetone=3000 by sidetone=800.
<br><br>
Of late, I put some more effort into bridge circuit development.
<br><br>
To run the driver, from this directory, simply type
$> ./driver <br>
<br>
Print help:<br>
$> ./driver -help 
<br>
