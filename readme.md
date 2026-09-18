# PTT Phone Player

Arduino firmware for a PTT phone installation. It reads the phone's
dial pulses and plays audio files through a DFPlayer Mini. The current access
code is `9574`; a correct code plays `mp3/0004.mp3`.

## Update the firmware

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Open `ptt-phone-player.ino` in Arduino IDE.
3. Install the required libraries from **Tools > Manage Libraries**:
	- `DFRobotDFPlayerMini`
	- `SoftwareSerial` (normally included with Arduino IDE)
4. Connect the Arduino board by USB.
5. Select the board in **Tools > Board**. The firmware is tested on Arduino
	Uno, Leonardo, and Mega.
6. Select the matching USB port in **Tools > Port**.
7. Click **Upload**.
8. Wait for **Done uploading**.

## Change the MP3 files

The audio files are stored on the DFPlayer Mini's microSD card, not in the
Arduino project.

1. Power off the phone/player and remove the microSD card.
2. Insert the card into a computer.
3. Keep the folder named `mp3` at the root of the card.
4. Put the audio files in that folder and name them exactly:

	```text
	mp3/0001.mp3  # line tone
	mp3/0002.mp3  # busy tone
	mp3/0003.mp3  # wrong number tone
	mp3/0004.mp3  # correct-code audio
	```

5. Eject the card safely, reinstall it in the DFPlayer Mini, and power on the
	device.

### Windows

Copy or replace the files directly in the `mp3` folder. Confirm that Windows
has not added a second extension such as `.mp3.mp3`.

### macOS

macOS can create hidden files on removable media. These files can interfere
with the DFPlayer Mini. After copying the audio files, open **Terminal**, move
to the SD card (replace `CARD_NAME` with its name), and run:

```bash
cd /Volumes/CARD_NAME
dot_clean .
```

This merges/removes the macOS hidden files. Eject the card with Finder before
putting it back in the player.

If the player still plays old files, delete the mp3 folder and copy fresh files
