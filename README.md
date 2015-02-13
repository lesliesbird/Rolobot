# Rolobot
Bot building assistant for TinyMUSH and other MU* variants.

Rolobot is a powerful utility bot program designed for use with TinyMUSH. It will probably work fine with other TinyMU* variants, although I have not tested it on those. Rolobot was originally written in the ARexx scripting language on the Amiga computer. It was been rewritten from the ground up in C for multi-platform compatibility. It has been tested on Linux, Mac OSX, Windows, Amiga (of course!) and even Android. Binaries are included for Windows, Amiga, Mac OSX and Android. A simple build.sh script will build it on most Linux
platforms, even the Raspberry Pi. Please note that the Android version requires a ROOTED device.

Usage is simple.

rolobot [configfile.cfg]

Rolobot will load from the default rolobot.cfg if no extra parameter is given.

It is best to create a new folder to use as your work directory and cd into it before invoking Rolobot. Rolobot will only have access to the files in your work directory.

Please see the rolobot.cfg for information on creating your own config file, as well as the example.cfg provided. If you do not want to enable the system speech function, simply put a single character (N) as the name of your speech command.

Of course, you will first need to create a player or robot username on the MUSH before you can log Rolobot into the MUSH.

Once Rolobot is logged into the robot account, you can then log in yourself as a player and use Rolobot as your personal building assistant. Start off by either say "[botnickname] help" or p botname=botnickname help. Rolobot will respond in kind, either by saying or paging you a list of commands.

Rolobot is not designed to be a chatbot, or to fool other players. While Rolobot does have a limited social repertoire, its primary purpose is building utility. Rolobot is an easy way to import and export files and mushobjects, turn ASCII art into MUSH-friendly descriptions (and vice-versa), and to build complicated sequences of rooms and exits quickly by following instructions in an offline textfile you create.

To quit Rolobot, either issue the bot's QUIT command that you set in the config file, or simply CRTL-C in Rolobot's terminal window.

The name 'Rolobot' came about simply because 'Rolo' was the name of the robot account it was originally designed to use. Rolo was a multidimensional being that appeared to change size and shape as different parts appeared in our three dimensional space. Rolo was an alien genie of sorts, and not a chewy caramel candy.  


Leslie Bird
amiga1@comcast.net

