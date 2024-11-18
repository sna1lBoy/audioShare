# audio share
a GUI* tool to make the process of playing sound along with screen share on discord for linux a bit easier

## how does it work?
- audio share uses pulseaudio or pipewire to create two virtual sinks: one that receives audio from select application(s) and one that adds your audio input (ie a microphone) to the application audio
- the first sink allows you to control what application sounds to share while also allowing you to hear the application audio yourself
- the second sink sends the application audio through your sound input so that discord hears all audio streams as a singular input
- application audio can be easily shared and unshared through a simple UI that displays all pulseaudio application outputs

## installation
### a brief warning
all development and testing was done on an x86-64 machine running arch linux and x11, i cannot guarantee this program will work on other architectures, distros, or display servers. that being said, if audio share breaks for you, please open an issue detailing the problem along with your specs and i may be able to fix it

### prerequisites 
audio share requires a pulseaudio or pipewire server to be set up to your preferences (and working) as well as GTK installed

if you require a different binary than the one in releases then audio share can be compiled with
```
gcc `pkg-config --cflags gtk+-3.0` -o ./bin/audioShare ./src/main.c `pkg-config --libs gtk+-3.0`
```
### script
for a simple installation, open the command line and enter
```
bash <(curl -s https://raw.githubusercontent.com/sna1lBoy/audioShare/main/install.sh)
```
to manually install
- download or clone the source code
-  move the file in ./bin/ to /bin/audioShare
- give it execute permissions
- move the files in ./resources/ to /etc/audioShare/

### in discord (optional)
for best results, disable noise suppression (aka krisp) and disable "automatically determine input sensitivity" while also moving the slider below all the way to the left

## configuration
open `/etc/audioShare/config.ini` in a text editor of your choice, you should see places to enter a source and a sink. these will be your default input and outputs, respectively, which you can find the name of by using `pactl list short sources` and `pactl list short sinks` and looking for the sink and source you mainly use

for example, the names will probably be in a similar form to
```
source = alsa_input.usb-DCMT_Technology_USB_Lavalier_Microphone_214b206000000178-00.mono-fallback
sink = alsa_output.pci-0000_00_1f.3.analog-stereo
```
you also need to set the latency (in milliseconds), which, in my testing, worked best at 1 for pulseaudio and 5 for pipewire however if you notice any distortion or roboticism then increase it

---
\* gee-you-eye
