# dial_player
Plays videos on microcontroller input.

It was used as part of a project, where the user could play videos on a screen by dialing numbers on an old rotary phone.

It was intended to run on a Raspberry Pi 3 Model b+ with Debian Bookworm and a Desktop Environment.

# how to use it?

- decide for one video playing as the "screen saver" it will play in a loop until there is input. rename this video to default.
- rename the other videos after the input that should start them.
- move all those videos into the directory that is configured in the 
- start the input device that is configured
- start the program


# how does it work?


It reads input from a microcontroller and checks it against video files in a configured directory.
MPV-Player will then play the video, which name matches the input string.

